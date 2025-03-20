#include "../src/controllers/includes/serialController.h"
#include "../src/controllers/includes/motorController.h"
#include <esp_timer.h>

L298N *motor;
AS5600 *sensor; 

#define PERCENTAGE_INCREMENT (-0.21)

// Definições da onda senoidal
const double AMPLITUDE = 1.0;

// Variáveis globais
unsigned long last_update_time = 0;
double current_percentage = 0.0;

// Variaveis do motor
double angle_counter = 0.0;
double previous_angle = 0.0;
double previous_velocity = 0.0;
double velocity = 0.0;
double acceleration = 0.0;

unsigned long current_time;

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
    previous_angle = current_position;
    angle_counter += delta_angle;
    velocity = delta_angle/(esp_timer_get_time()-current_time);
    acceleration = (velocity - previous_velocity);
    previous_velocity = velocity;


    // Tempo atual
    current_time = esp_timer_get_time();
    // Verificar se 5 segundos se passaram
    if ((current_time - last_update_time) >= 2500*1000){
        last_update_time = current_time;
        current_percentage = 1.0;
    }
    
    // Calcular o valor de `out` baseado no percentual
    double out = current_percentage * BDC_MAX_POWER;
    
    DEBUG_SERIAL( "Pos", angle_counter );
    DEBUG_SERIAL( "Vel", velocity );
    DEBUG_SERIAL( "Acel", acceleration );
    DEBUG_SERIAL( "Atuador", out );
    
    if ( out > 0 ){
        motor->set_direction( BDC_FORWARD ); 
    } else {
        out *= -1;
        motor->set_direction( BDC_BACKWARD );  
    }
    motor->set_speed( fabs(out) );
    DEBUG_SERIAL( "Setpoint", out );
    DEBUG_SERIAL( "dt", esp_timer_get_time()/1000.0 );

    vTaskDelay( 1 / portTICK_PERIOD_MS );
}


void app_main(){
    actuator_setup();
    while( true ){
        actuator_loop();
    }
}