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
void blinkTask(void *pvParameters);

DataloggerController    *datalogger;
MotorController         *controller;
ModbusController        *modbus; 
Relay                   *relay;


void setup(){

    /** Inicia a serial  */ 
    serial_begin();

    /** Inicia o controle dos motores */
    controller = new MotorController( "MotorController", SENSOR_AS5600_I2C, MOTOR_DC ); 
    // controller->set_pid_gains( 10.25, 0.42, 4.78 );
    controller->set_pid_gains( 15.25, 0.25, 5.78 );

    /** Inicia a comunicação WiFi para controle Modbus */ 
    DEBUG_SERIAL( "WIFI SSID", SSID_MASTER );
    DEBUG_SERIAL( "WIFI PSD", PSD_MASTER );
    modbus = new ModbusController( "ModbusController", RTC_SOFT );


    DEBUG_SERIAL( "RELAY", "PRE RELAY" );
    /** Inicializa o Relé de proteção */
    relay = new Relay( "Relay controller" );
    relay->turn_on();

    /** Habilita o motor após ligar o relé */
    controller->start();

    /** Atualiza a posição do motor no registrador Modbus */
    modbus->mb->Ireg( INPUT_SENSOR_POS, (uint16_t)controller->sensor->raw_position );

    /** Inicia as tasks */
    xTaskCreate( &blinkTask, "BlinkTask", 1024*1, NULL, 5, NULL );
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

    // Verifica se a data foi atualizada 
    if ( modbus->mb->Ireg(INPUT_YEAR) == 0 ) {
        // Desliga o motor
        controller->stop();
        DEBUG_SERIAL( "DATETIME", "Waiting datetime update" ); 

    }else{
        if ( ((int16_t) modbus->mb->Ireg(INPUT_ZENITH)) >= 0  ) { 
            // Ligar o motor
            controller->start();
            
            // Atualizar os dados do controlador de posição
            controller->set_target( ((int16_t)(modbus->mb->Ireg(INPUT_SUN_TARGET)/10)) );
            
            DEBUG_SERIAL( "SUN POSITION"  , ((int16_t)(modbus->mb->Ireg(INPUT_SUN_TARGET)/10)) ); 
            DEBUG_SERIAL( "MOTOR POSITION", controller->position ); 
            DEBUG_SERIAL( "MOTOR SPEED %"  , controller->motor->speed ); 

            // DEBUG_SERIAL( "SUN ZENITH",  String((int16_t)(modbus->mb->Ireg(INPUT_ZENITH) ))); 
            // DEBUG_SERIAL( "SUN AZIMUTH", String((int16_t)(modbus->mb->Ireg(INPUT_AZIMUTH))) + "\n"); 

        }else {
            // Desliga o motor
            controller->stop();
            DEBUG_SERIAL( "MOON SUN", "Goal Pos. = " + String((int16_t)(modbus->mb->Ireg(INPUT_SUN_TARGET))/100 ));
            DEBUG_SERIAL( "MOON SUN", "Zenith = "    + String((int16_t)(modbus->mb->Ireg(INPUT_ZENITH))/100     ));
            DEBUG_SERIAL( "MOON SUN", "Azimuth = "   + String((int16_t)(modbus->mb->Ireg(INPUT_AZIMUTH))/100    )+ "\n");
        }        
    }

    modbus->mb->Ireg( INPUT_SENSOR_POS, (uint16_t)controller->sensor->raw_position );

    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


/** Task de Led blink */
void blinkTask(void *pvParameters) {
    gpio_set_direction( LED_BOARD, (gpio_mode_t) GPIO_MODE_OUTPUT );
    bool state = false;
    while(true) {
        gpio_set_level(LED_BOARD, state = !state );
        vTaskDelay( pdMS_TO_TICKS(500));
    }
}