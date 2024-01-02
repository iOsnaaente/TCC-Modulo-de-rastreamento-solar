#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "led_strip.h"
#include "sdkconfig.h"

#include "bdc_motor/bdc_motor.h"

#include "pinout.h"

#define BLINK_GPIO      2
#define BLINK_PERIOD    1000

#define BDC_MCPWM_TIMER_RESOLUTION_HZ   10000000                      // 10MHz, 1 tick = 0.1us
#define BDC_MCPWM_FREQ_HZ               25000                         // 25KHz PWM
#define BDC_MCPWM_DUTY_TICK_MAX       \
            (BDC_MCPWM_TIMER_RESOLUTION_HZ / BDC_MCPWM_FREQ_HZ)       // Maximum value we can set for the duty cycle, in ticks


static uint8_t s_led_state = 0;



static void blink_led(void){
    gpio_set_level(BLINK_GPIO, s_led_state);
}

void app_main(void){
    /* Configure the peripheral according to the LED type */
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);


    /** Configurações do Motor via MCPWM */ 
    bdc_motor_t motor;
    motor = (bdc_motor_t) {
        .gpio_IN1 = IN1,
        .gpio_IN2 = IN2,
        .gpio_PWM = ENB,
        .duty = 0,
        .direction = BDC_ENABLED,
        .pwm_freq_hz = BDC_MCPWM_FREQ_HZ,
    };
    
    // Inicializa a struct Motor com as configurações  
    bdc_motor_init_device( motor );
    /* Forward motor" */
    bdc_motor_forward(motor);

    // Liga o relé 
    gpio_set_level( POWER_RELAY, true );


    while (1) {
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay( BLINK_PERIOD / portTICK_PERIOD_MS );
    }
}
