#include "../src/controllers/includes/serialController.h"
#include "../src/controllers/includes/motorController.h"
#include <esp_timer.h>

L298N *motor;
AS5600 *sensor; 


// Contador de ângulo
double angle_counter = 0.0;
double previous_angle = 0.0;


void sensor_setup(){
    serial_begin();
    motor = new L298N( "Motor", BDC_FORWARD, 0.0  );
    sensor = new AS5600( "Sensor", AS5600_MODE_I2C ); 
}

// Variáveis globais para controle de tempo e direção
unsigned long last_direction_change = 0;
int is_forward = 1; 


void sensor_loop(){
    // Leitura da posição do sensor
    double current_position = sensor->read_scaled(); 
    double current_position_raw = sensor->read_raw();

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

    // Controle de tempo para mudar a direção
    unsigned long current_time = esp_timer_get_time() / 1000;       // Tempo atual em milissegundos
    if (current_time - last_direction_change >= 5000) {             // Muda a direção a cada 5 segundos
        is_forward = is_forward == 1 ? -1 : 1;                                   // Alterna a direção
        last_direction_change = current_time;                       // Atualiza o tempo da última mudança
    }

    // Define a direção e a velocidade do motor
    if (is_forward == 1) {
        motor->set_direction(BDC_FORWARD);
    } else {
        motor->set_direction(BDC_BACKWARD);
    }
    motor->set_speed(BDC_MAX_POWER * 0.5);
    
    DEBUG_SERIAL( "Pos", current_position );
    DEBUG_SERIAL( "Raw", current_position_raw );
    DEBUG_SERIAL( "Acum", angle_counter );
    DEBUG_SERIAL( "Atuador", BDC_MAX_POWER*0.5*is_forward );
    DEBUG_SERIAL( "dt", esp_timer_get_time()/1000.0 );

    vTaskDelay( 1 / portTICK_PERIOD_MS );
}
