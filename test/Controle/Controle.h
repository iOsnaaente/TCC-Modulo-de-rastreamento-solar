#include "../src/controllers/includes/serialController.h"
#include "../src/controllers/includes/motorController.h"
#include <esp_timer.h>

L298N *motor;
AS5600 *sensor; 

// Define o setpoint e o ganho proporcional
double SETPOINT = 0.0;
double Kp =  10.0;        
double Ki =   1.0;         
double Kd =   0.1;        

double input = 0.0;   
double output = 0.0;  

// Variáveis internas do PID
double previous_error = 0.0;
double integral = 0.0;
unsigned long last_time = 0;

// Definições da onda senoidal
const double AMPLITUDE = 7500.0; // Amplitude de 1000, resultando em pico a pico de 2000
double FREQUENCY = 0.020;    // Frequência em Hz (ajustar conforme necessário)


// Contador de ângulo
double angle_counter = 0.0;
double previous_angle = 0.0;

void controll_setup(){
    serial_begin();
    motor = new L298N( "Motor", BDC_FORWARD, 0.0  );
    sensor = new AS5600( "Sensor", AS5600_MODE_I2C ); 
}


void controll_loop(){
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
    double elapsed_time = (current_time - last_time) / 1000000.0; 

    // Cálculo do erro
    double SETPOINT = AMPLITUDE * std::sin(TWO_PI * FREQUENCY * ( current_time / 1000000.0));
    
    double error = SETPOINT - angle_counter;
    integral += error * elapsed_time;
    double derivative = (error - previous_error) / elapsed_time;
    
    // Calcula o sinal de controle PID
    double control_signal = Kp * error + Ki * integral + Kd * derivative;

    // Atualiza o valor anterior do erro
    previous_error = error;
    last_time = current_time;
    
    if ( control_signal > 0 ){
        if ( control_signal > BDC_MAX_POWER ) {
            control_signal = BDC_MAX_POWER;
        } else if ( control_signal < BDC_MIN_POWER ){
            control_signal = BDC_MIN_POWER;
        }
        motor->set_direction( BDC_FORWARD ); 
    } else {
        if ( control_signal < -BDC_MAX_POWER ) {
            control_signal = -BDC_MAX_POWER;
        } else if ( control_signal > -BDC_MIN_POWER  ){ 
            control_signal = -BDC_MIN_POWER;
        }
        motor->set_direction( BDC_BACKWARD );  
    }
    motor->set_speed( fabs(control_signal) );
    
    DEBUG_SERIAL( "Position", angle_counter );
    DEBUG_SERIAL( "Atuador", control_signal*100/BDC_MAX_POWER );
    DEBUG_SERIAL( "Setpoint", SETPOINT );
    DEBUG_SERIAL( "dt", esp_timer_get_time()/1000.0 );

    vTaskDelay( 1 / portTICK_PERIOD_MS );
}
