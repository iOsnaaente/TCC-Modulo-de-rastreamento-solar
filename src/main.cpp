#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/** Board configurations */
#include "../board_config.h"

/** Controladores */
#include "controllers/includes/dataloggerController.h"
#include "controllers/includes/modbusController.h"
#include "controllers/includes/motorController.h"
#include "controllers/includes/serialController.h"

/** Módulos */
#include "modules/system/relay.h"


/** Protótipo de funções */
void blink_Task(void *pvParameters);

DataloggerController    *datalogger;
MotorController         *controller;
ModbusController        *modbus; 
Relay                   *relay;


#include "../test/Controle/ControleDegrau.h"

void setup(){

    app_main();
    while( true );

    /** Inicia a serial  */ 
    serial_begin();

    /** Inicia a comunicação WiFi para controle Modbus */ 
    DEBUG_SERIAL( "WIFI SSID", SSID_MASTER );
    modbus = new ModbusController( "ModbusController", RTC_SOFT );

    /** Inicia o controle dos motores */
    controller = new MotorController( "MotorController", SENSOR_AS5600_I2C, MOTOR_DC );  // controller->set_pid_gains( 10.25, 0.42, 4.78 );
    // controller->set_pid_gains( 15.25, 5.25, 10.78 );
    controller->set_pid_gains( 10.0, 0.0, 0.0 );

    /** Inicializa o Relé de proteção */
    relay = new Relay( "Relay controller" );
    relay->turn_on();

    /** Habilita o motor após ligar o relé */
    controller->stop();

    /** Inicia as tasks */
    xTaskCreate( &blink_Task, "BlinkTask", 1024*1, NULL, tskIDLE_PRIORITY, NULL );
}


/** Loop principal */
void loop() {

    // Verifica se tem que atualizar a hora 
    if ( modbus->mb->Coil( COIL_DT_SYNC ) == true ){
        modbus->mb->Ireg( INPUT_YEAR,   modbus->mb->Hreg( HR_YEAR ) );
        modbus->mb->Ireg( INPUT_MONTH,  modbus->mb->Hreg( HR_MONTH ) );
        modbus->mb->Ireg( INPUT_DAY,    modbus->mb->Hreg( HR_DAY ) );  
        modbus->mb->Ireg( INPUT_HOUR,   modbus->mb->Hreg( HR_HOUR ) );
        modbus->mb->Ireg( INPUT_MINUTE, modbus->mb->Hreg( HR_MINUTE ) );
        modbus->mb->Ireg( INPUT_SECOND, modbus->mb->Hreg( HR_SECOND ) );
        modbus->mb->Coil( COIL_DT_SYNC, false );
    }

    switch (modbus->mb->Hreg( HR_STATE ) ){
    case HR_STATE_AUTO: // 0x00 
        // Verifica se a data foi atualizada 
        if ( modbus->mb->Ireg(INPUT_YEAR) == 0 ) {
            // Desliga o motor
            controller->stop();
            DEBUG_SERIAL( "DATETIME", "Waiting datetime update" ); 
        }else{

            if ( ((int16_t) modbus->mb->Ireg(INPUT_ZENITH)) >= 0.0  ) { 
                // Ligar o motor
                controller->start();
                // Atualizar os dados do controlador de posição
                controller->set_target( modbus->mb->Ireg(INPUT_SUN_TARGET) );
            
            }else {
                // Desliga o motor OU levar ele para casa 
                controller->stop();
            }        
        }
        break;

    case HR_STATE_QUADRANT_1: // 0x01
        controller->start();
        controller->set_target( HR_QUADRANT_1_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_1_ANGLE );
        break;
    case HR_STATE_QUADRANT_2: // 0x02
        controller->start();
        controller->set_target( HR_QUADRANT_2_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_2_ANGLE );
        break;
    case HR_STATE_QUADRANT_3: // 0x03
        controller->start();
        controller->set_target( HR_QUADRANT_3_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_3_ANGLE );
        break;
    case HR_STATE_QUADRANT_4: // 0x04
        controller->start();
        controller->set_target( HR_QUADRANT_4_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_4_ANGLE );
        break;
    case HR_STATE_QUADRANT_12: // 0x12
        controller->start();
        controller->set_target( HR_QUADRANT_12_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_12_ANGLE );
        break;
    case HR_STATE_QUADRANT_23: // 0x23
        controller->start();
        controller->set_target( HR_QUADRANT_23_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_23_ANGLE );
        break;
    case HR_STATE_QUADRANT_34: // 0x34
        controller->start();
        controller->set_target( HR_QUADRANT_34_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_34_ANGLE );
        break;
    case HR_STATE_QUADRANT_41: // 0x41
        controller->start();
        controller->set_target( HR_QUADRANT_41_ANGLE );
        modbus->mb->Ireg(INPUT_SUN_TARGET, HR_QUADRANT_41_ANGLE );
        break;

    case HR_STATE_OFF: // 0xFF 
    default:
        controller->stop();
        break;
    }

    // Debug 
    DEBUG_SERIAL( "GOAL POSITION"  , modbus->mb->Ireg(INPUT_SUN_TARGET) ); 
    DEBUG_SERIAL( "SENSOR POSITION", controller->sensor->scaled_position ); 
    DEBUG_SERIAL( "MOTOR POSITION" , controller->position ); 
    DEBUG_SERIAL( "MOTOR SPEED %"  , controller->motor->speed ); 
    modbus->mb->Ireg( INPUT_SENSOR_POS, (float)controller->sensor->scaled_position );
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


/** Task de Led blink */
void blink_Task(void *pvParameters) {
    gpio_set_direction( LED_BOARD, (gpio_mode_t) GPIO_MODE_OUTPUT );
    bool state = false;
    while(true) {
        gpio_set_level(LED_BOARD, state = !state );
        vTaskDelay( pdMS_TO_TICKS(500));
    }
}