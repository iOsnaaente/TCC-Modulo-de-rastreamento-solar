#include <sys/cdefs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "pinout.h"

// #include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "esp_timer.h"


#define DEBUG

#define QMC5883L_SENSOR


int speed = 1000;

void setup(void) {

  // Inicializa o Serial
  Serial.begin(115200);
  Serial.println("Serial was initiated");

  // Inicializa a struct Motor com as configurações
  bdc_motor_init_device();
  bdc_motor_set_speed((double)0);
  bdc_motor_forward();

  // Inicializa o contador de pulsos 
  init_pulses_counter();  

  // Inicializa o relé
  init_relay_configurations();
  turn_relay_on();

// Inicializa o sensor de posição
#ifdef QMC5883L_SENSOR
  init_compass();
#endif
}

int i = 5400; 
void loop(void) {
  
  int32_t x = read_compass();
  Serial.print( "Accel:" ); Serial.println(x);
  
  int32_t p = compute_pulses();
  Serial.print( "Pulses:" ); Serial.println(p);
  
  bdc_motor_set_speed( i );
  Serial.print( "Vel:" ); Serial.println(i/50);
  i += 50;
  if ( i > 8000 ){
    i = 0;
  }

  vTaskDelay(pdMS_TO_TICKS(100));
}
