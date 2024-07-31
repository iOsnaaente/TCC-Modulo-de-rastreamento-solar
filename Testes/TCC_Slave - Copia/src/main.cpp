#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "../board_config.h"

// #include "modules/modbus.h"
#include "modules/system/relay.h"

#include "controllers/includes/serialController.h"
#include "controllers/includes/motorController.h"


#include "driver/gpio.h"


/** Protótipo de funções */
void blinkTask(void *pvParameters);


void setup(){
    serial_begin();
    DEBUG_SERIAL("SERIAL", "SERIAL BEGAN" );
    

    xTaskCreate( &blinkTask, "BlinkTask", 1024, NULL, 5, NULL );
}


void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


void blinkTask(void *pvParameters) {
    gpio_set_direction( LED_BOARD, (gpio_mode_t) GPIO_MODE_OUTPUT );
    while(true) {
        gpio_set_level(LED_BOARD, true);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BOARD, false);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}