#include "../src/controllers/includes/serialController.h"
#include "../src/controllers/includes/motorController.h"
#include <esp_timer.h>

#include "../../src/modules/sensors/incremental.h"

L298N *motor;
IncrementalSensor *sensor; 


// Contador de ângulo
double angle_counter = 0.0;
double previous_angle = 0.0;


void sensor_setup(){
    serial_begin();
    motor = new L298N( "Motor", BDC_FORWARD, 0.0  );
    sensor = new IncrementalSensor( "Sensor Incremental", BOARD_SCL, BOARD_SDA ); 
    
    // Configura a escala do sensor para PPR * SCALE 
    sensor->set_scale( 2.0 );  
}

// Variáveis globais para controle de tempo e direção
unsigned long last_measured_time = 0;
unsigned long dt = 0;
double velocity = 0; 

float DELTA_INC = 0.01;

void sensor_loop(){
    // Leitura da posição do sensor
    double pulses = sensor->read_raw(); 

    // Controle de tempo para mudar a direção
    unsigned long current_time = esp_timer_get_time() / 1000;       // Tempo atual em milissegundos
    if (current_time - last_measured_time >= 200 ) {             // Muda a direção a cada 5 segundos
        last_measured_time = current_time;
        velocity += DELTA_INC;
        if (velocity > 1){
            velocity = 0;
        }
    }
    motor->set_speed( velocity*BDC_MAX_POWER );
    motor->set_direction(BDC_FORWARD);

    printf( "Pulses:%f,Atuador:%f,dt:%ld\n", pulses, velocity, current_time );
    vTaskDelay( 5 / portTICK_PERIOD_MS );
}



void app_main() {
    sensor_setup();
    while (true){
        sensor_loop(); 
    }
}