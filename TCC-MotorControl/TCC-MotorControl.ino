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


void setup(void) {

  // Inicializa a struct Motor com as configurações
  bdc_motor_init_device();
  /* Forward motor" */
  bdc_motor_set_speed( 1000 );
  bdc_motor_forward();
  
  // Liga o relé
  gpio_set_level(POWER_RELAY, true);
}


void loop(void) {
  /* Forward motor" */
  bdc_motor_forward();
  vTaskDelay(pdMS_TO_TICKS(1000));
  /* Forward motor" */
  bdc_motor_backward();
  vTaskDelay(pdMS_TO_TICKS(1000));
}
