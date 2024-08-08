#include "../src/controllers/includes/serialController.h"
#include "../src/controllers/includes/motorController.h"
#include <esp_timer.h>

L298N *motor;
AS5600 *sensor; 


// Definições da onda senoidal
const double AMPLITUDE = 1.0; // Amplitude de 1000, resultando em pico a pico de 2000
const double FREQUENCY = 0.025;    // Frequência em Hz (ajustar conforme necessário)

// Define a velocidade do motor 
double velocity = 0.0; 

// Contador de ângulo
double angle_counter = 0.0;
double previous_angle = 0.0;


void actuator_setup(){
    serial_begin();
    motor = new L298N( "Motor", BDC_FORWARD, 0.0  );
    sensor = new AS5600( "Sensor", AS5600_MODE_I2C ); 
}


void actuator_loop(){
    // Leitura da posição do sensor
    double current_position = sensor->read_scaled(); 
    // Calculo do ângulo acumulado
    double delta_angle = current_position - previous_angle;
    // Corrige o salto de ângulo se necessário (exemplo: -180 a 180 graus)
    if (delta_angle > 180) {
        delta_angle -= 360;
    } else if (delta_angle < -180) {
        delta_angle += 360;
    }
    angle_counter += delta_angle;
    previous_angle = current_position;

    // Tempo atual
    unsigned long current_time = esp_timer_get_time();
    double velocity = AMPLITUDE * std::sin(TWO_PI * FREQUENCY * ( current_time / 1000000.0));
    
    velocity *= BDC_MAX_POWER; 
    
    DEBUG_SERIAL( "Position", angle_counter );
    DEBUG_SERIAL( "Atuador", velocity );
    
    if ( velocity > 0 ){
        motor->set_direction( BDC_FORWARD ); 
    } else {
        velocity *= -1;
        motor->set_direction( BDC_BACKWARD );  
    }
    motor->set_speed( fabs(velocity) );
    DEBUG_SERIAL( "Setpoint", velocity );
    DEBUG_SERIAL( "dt", esp_timer_get_time()/1000.0 );

    vTaskDelay( 1 / portTICK_PERIOD_MS );
}


void app_main(){
    actuator_setup();
    while( true ){
        actuator_loop();
    }
}