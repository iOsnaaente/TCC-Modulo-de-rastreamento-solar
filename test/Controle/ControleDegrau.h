#include "../src/controllers/includes/serialController.h"
#include "../src/controllers/includes/motorController.h"
#include "../src/modules/system/relay.h"
#include <esp_timer.h>

L298N *motor;
AS5600 *sensor; 

// Filtros para ajuste de controle 
// Integral WINDUP  
#define MAX_INTEGRAL_POS ((double)10000.0) 
#define MAX_INTEGRAL_VEL ((double)10000.0) 

// FEED FORWARD Boost
#define TORQUE_FEEDFORWARD ((double)0.4)

// Definições da onda senoidal
#define AMPLITUDE ((double)180.0)
#define FREQUENCY ((double)(1.0/120.0))
#define OFFSET ((double)180.0)


// Definições de controle  
typedef struct PIDController {
    double setpoint;
    double input;
    double error;
    double Kp;
    double Ki;
    double Kd;
    double prev_error;
    double integral;
    double derivative;
    double output;
} PIDController; 


// Variaveis de controle de posição 
PIDController position = {
    .setpoint = AMPLITUDE,
    .input = 0.0,
    .error = 0.0,
    .Kp = 100.0,
    .Ki = 0.10,
    .Kd = 0.0,
    .prev_error = 0.0,
    .integral = 0.0,
    .derivative = 0.0,
    .output = 0.0,
};

// Variaveis de controle de velocidade  
PIDController velocity = {
    .setpoint = 0.0,
    .input = 0.0,
    .error = 0.0,
    .Kp = 1.0,
    .Ki = 0.50,
    .Kd = 0.0,
    .prev_error = 0.0,
    .integral = 0.0,
    .derivative = 0.0,
    .output = 0.0,
};

// Variaveis de controle global 
double angle_counter = 0.0;
double previous_angle = 0.0; 
double delta_angle = 0.0; 

double last_measurement_time = 0.0;
double measurement_time = 1000.0;

double control_signal = 0.0;
double mean_control_signal = 0.0;

int64_t counter = 0;


void controll_setup(){
    /** Inicia a serial */
    serial_begin();
    /** Inicia a Classe de motor */ 
    motor = new L298N( "Motor", BDC_FORWARD, 0.0  );
    /** Inicia a Classe do sensor */
    sensor = new AS5600( "Sensor", AS5600_MODE_I2C ); 
    /** Inicializa o Relé de proteção */
    relay = new Relay( "Relay controller" );
    relay->turn_on();
}


void controll_loop(){
    // Ler a posição do motor 
    position.input = sensor->read_scaled();


    // Calculo do ângulo acumulado e correção do salto de ângulo
    delta_angle = position.input - previous_angle;
    if (delta_angle > 180) {
        delta_angle -= 360;
    } else if (delta_angle < -180) {
        delta_angle += 360;
    }
    angle_counter += delta_angle;
    previous_angle = position.input;


    // Calculo do tempo de amostragem
    int64_t current_measurement_time = esp_timer_get_time();
    double elapsed_time = (current_measurement_time - last_measurement_time) / 1000.0; 
    last_measurement_time = current_measurement_time;


    // Calculo da malha de controle externa (Posição)
    position.error = position.setpoint - angle_counter;
    position.integral += position.error * elapsed_time;
    if (fabs(position.integral) > MAX_INTEGRAL_POS )
        position.integral = (position.integral > 0) ? MAX_INTEGRAL_POS : -MAX_INTEGRAL_POS;
    position.derivative = (position.error - position.prev_error) / elapsed_time;
    position.prev_error = position.error;
    position.output = \
        ( position.Kp * position.error ) + \
        ( position.Ki * position.integral ) + \
        ( position.Kd * position.derivative );

    
    // Calculo da malha de controle interna (Velocidade)
    velocity.setpoint = position.output;
    velocity.input = sensor->scaled_velocity;
    velocity.error = velocity.setpoint - velocity.input;
    velocity.integral += velocity.error * elapsed_time;
    if (fabs(velocity.integral) > MAX_INTEGRAL_VEL )
        velocity.integral = (velocity.integral > 0) ? MAX_INTEGRAL_VEL : -MAX_INTEGRAL_VEL;
    velocity.derivative = (velocity.error - velocity.prev_error) / elapsed_time;
    velocity.prev_error = velocity.error;
    velocity.output = \
        ( velocity.Kp * velocity.error ) + \
        ( velocity.Ki * velocity.integral ) + \
        ( velocity.Kd * velocity.derivative );


    // Control signal 
    control_signal = velocity.output;


    // Adiciona o torque feedforward boost para suprir o atrito do motor
    if ( fabs(control_signal) < 0.1 ) {
        control_signal = 0.0;
    }else{
        if( control_signal > 0 )
            control_signal += TORQUE_FEEDFORWARD;
        else
            control_signal -= TORQUE_FEEDFORWARD;
    }


    // Limita o sinal de controle 
    if ( control_signal > BDC_MAX_POWER ) {
    control_signal = BDC_MAX_POWER;
    } else if ( control_signal < -BDC_MAX_POWER ) {
    control_signal = -BDC_MAX_POWER;
    }


    // Corrige orientação e atua 
    if ( control_signal > 0.0 ){
        motor->set_direction( BDC_FORWARD ); 
    } else {
        motor->set_direction( BDC_BACKWARD );  
    }
    
    control_signal = fabs(control_signal);
    if ( fabs(position.error) < 0.750  ){
        control_signal = 0.0;
        position.integral = 0.0;
        velocity.integral = 0.0;
    }
        
    motor->set_speed( control_signal );


    // Debug
    if ( counter++ > 1000/1 ){
        
        DEBUG_SERIAL( "\nSetpoint", position.setpoint );
        DEBUG_SERIAL( "Position", String(angle_counter) + "°" );
        DEBUG_SERIAL( "Error", String(position.error) + "°" );
        
        DEBUG_SERIAL( "Delta Angle" , delta_angle );
        DEBUG_SERIAL( "Pos. In." , position.input );
        DEBUG_SERIAL( "Pos. Out.", position.output );
        
        DEBUG_SERIAL( "Vel. In." , velocity.input );
        DEBUG_SERIAL( "Vel. Out.", velocity.output );
        
        DEBUG_SERIAL( "Atuador Abs.", control_signal );
        DEBUG_SERIAL( "Atuador Norm.", (control_signal*100)/BDC_MAX_POWER );
        DEBUG_SERIAL( "dt"     , esp_timer_get_time()/1000000.0 );
        counter = 0;
    }

    vTaskDelay( pdMS_TO_TICKS(1) );
}

void app_main( void ) {
    controll_setup();
    while( true ) {
        controll_loop();
    }
}