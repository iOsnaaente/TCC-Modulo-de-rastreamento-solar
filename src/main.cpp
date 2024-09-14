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


DataloggerController *datalogger;
MotorController  *controller;
ModbusController *modbus; 
Relay *relay;


void setup(){
    serial_begin();
    DEBUG_SERIAL("SERIAL", "SERIAL BEGAN" );

    /** Inicia o controle dos motores */
    controller = new MotorController( "MotorController", SENSOR_AS5600_I2C, MOTOR_DC ); 
    controller->set_pid_gains( 10.25, 0.42, 4.78 );

    /** Inicia o Log de dados via SDCard */
    // datalogger = new DataloggerController( "DataLogger via SDCard", SDCARD_LOGGING );

    /** Inicia a comunicação WiFi para controle Modbus */ 
    DEBUG_SERIAL( "WIFI SSID", SSID_MASTER );
    DEBUG_SERIAL( "WIFI PSD", PSD_MASTER );
    modbus = new ModbusController( "ModbusController", RTC_SOFT );

    /** Inicializa o Relé de proteção */
    relay = new Relay( "Relay controller" );
    relay->turn_on();

    /* Habilita o motor após ligar o relé */
    controller->start();

    /** Inicia as tasks */
    xTaskCreate( &blinkTask, "BlinkTask", 1024, NULL, 5, NULL );
}


/** Loop principal */
void loop() {
    // Verifica se a data foi atualizada 
    if ( modbus->mb.Ireg(INPUT_YEAR) == 0 ) {
        // Desliga o motor
        controller->stop();
        // Debug 
        DEBUG_SERIAL( "DATETIME", "Waiting datetime update" ); 
    }else{
        if ( ((int16_t) modbus->mb.Ireg(INPUT_ZENITH)) >= 0  ) { 
            
            // Ligar o motor
            controller->start();
            // Atualizar os dados do controlador de posição
            controller->set_target( ((int16_t)(modbus->mb.Ireg(INPUT_SUN_TARGET))) );
            
            DEBUG_SERIAL( "SUN POSITION"  , ((int16_t)(modbus->mb.Ireg(INPUT_SUN_TARGET))) ); 
            DEBUG_SERIAL( "MOTOR POSITION", controller->position ); 

            DEBUG_SERIAL( "SUN ZENITH",  String((int16_t)(modbus->mb.Ireg(INPUT_ZENITH) ))); 
            DEBUG_SERIAL( "SUN AZIMUTH", String((int16_t)(modbus->mb.Ireg(INPUT_AZIMUTH))) + "\n"); 

        }else {
            // Desliga o motor
            controller->stop();
            DEBUG_SERIAL( "MOON SUN", "Goal Pos. = " + String((int16_t)(modbus->mb.Ireg(INPUT_SUN_TARGET))/100 ));
            DEBUG_SERIAL( "MOON SUN", "Zenith = "    + String((int16_t)(modbus->mb.Ireg(INPUT_ZENITH))/100     ));
            DEBUG_SERIAL( "MOON SUN", "Azimuth = "   + String((int16_t)(modbus->mb.Ireg(INPUT_AZIMUTH))/100    )+ "\n");
        }        
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}


/** Task de Led blink */
void blinkTask(void *pvParameters) {
    gpio_set_direction( LED_BOARD, (gpio_mode_t) GPIO_MODE_OUTPUT );
    while(true) {
        gpio_set_level(LED_BOARD, true);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED_BOARD, false);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}