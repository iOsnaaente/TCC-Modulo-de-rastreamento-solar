#include "../src/controllers/includes/serialController.h"
#include "../src/controllers/includes/motorController.h"
#include <esp_timer.h>

#include "../../src/modules/sensors/incremental.h"
#include "../../src/modules/sensors/analog.h"

IncrementalSensor *incremental; 
AnalogSensor *analog; 
L298N *motor;

// Contador de ângulo
double angle_counter = 0.0;
double previous_angle = 0.0;


void sensor_setup(){
    serial_begin();
    motor = new L298N( "Motor", BDC_FORWARD, 0.0  );
    incremental = new IncrementalSensor( "Sensor Incremental", BOARD_SCL, BOARD_SDA ); 
    analog = new AnalogSensor( "Sensor Analogico", (uint8_t)ADC_EXT1 );

    motor->set_direction( BDC_FORWARD );

    // Configura a escala do sensor para PPR * SCALE 
    incremental->set_scale( 2.0 );  
}


unsigned long dt = 0;
double velocity = 0; 

void sensor_loop(){
    // Leitura da posição do sensor
    double pulses = incremental->read_raw(); 

    double velocity = analog->read_scaled()/360;
    motor->set_speed( velocity*BDC_MAX_POWER );

    unsigned long current_time = esp_timer_get_time() / 1000;

    printf( "Sensor:%d,Atuador:%f,Pulses:%f,dt:%ld\n", analog->read_raw(), velocity, pulses, current_time );
    vTaskDelay( 5 / portTICK_PERIOD_MS );
}



void app_main() {
    sensor_setup();
    while (true){
        sensor_loop(); 
    }
}