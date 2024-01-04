#include <stdio.h>
#include <inttypes.h>

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "pinout.h"

#include "bdc_motor_control.h"
#include "relay_control.h"
#include "pcnt_encoder.h"


void app_main(void){

    // Inicializa a struct Motor com as configurações
    bdc_motor_init_device();
    bdc_motor_set_speed((double)0);
    bdc_motor_forward();

    // Inicializa o contador de pulsos 
    init_pulses_counter();  

    // Inicializa o relé
    init_relay_configurations();
    turn_relay_on();


    int i = 5400;
    bool dir = false;
    while ( true ){

        int32_t p = compute_pulses();
        printf("Vel: %d\tPulses: %ld\n", i, p ); 

        bdc_motor_set_speed( i );
        i += 50;
        if ( i > 8000 ){
            i = 0;
            dir ^= 1;
            if ( dir ){
                bdc_motor_forward();
            }else{
                bdc_motor_backward();
            }
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
