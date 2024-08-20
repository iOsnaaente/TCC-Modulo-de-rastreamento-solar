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
    controller->set_pid_gains( 5.25, 1.12, 1.78 );

    /** Inicia o Log de dados via SDCard */
    datalogger = new DataloggerController( "DataLogger via SDCard", SDCARD_LOGGING );
    
    /** Inicia a comunicação WiFi para controle Modbus */ 
    modbus = new ModbusController( "ModbusController", RTC_SOFT );

    /** Inicializa o Relé de proteção */
    relay = new Relay( "Relay controller" );
    relay->turn_on();

    /* Habilita o motor após ligar o relé */
    controller->start();


    /** Inicia as tasks */
    // xTaskCreate( &blinkTask, "BlinkTask", 1024, NULL, 5, NULL );
}


/** Loop principal */
void loop() {
    // Atualizar os dados do controlador de posição
    controller->set_target( modbus->mb.Ireg(INPUT_SUN_TARGET)*50 );
    // Debug 
    DEBUG_SERIAL( "SUN POSITION", modbus->mb.Ireg(INPUT_SUN_TARGET)*50 ); 
    DEBUG_SERIAL( "POSITION", controller->position ); 

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


// #include "../test/Controle/ControleDegrau.h"
// #include "modules/system/relay.h"

// Relay *rela; 

// void setup(void) {
//     serial_begin();
//     rela = new Relay( "relay" );
//     rela->turn_on();
//     app_main();
// }


// void loop(void) {
//     vTaskDelay(1000 / portTICK_PERIOD_MS);
// }