// #include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"
// #include "freertos/semphr.h"
// #include "freertos/task.h"

// /** Board configurations */
// #include "../board_config.h"

// /** Controladores */
// #include "controllers/includes/dataloggerController.h"
// #include "controllers/includes/modbusController.h"
// #include "controllers/includes/motorController.h"
// #include "controllers/includes/serialController.h"

// /** Módulos */
// #include "modules/system/relay.h"


// /** Protótipo de funções */
// void blinkTask(void *pvParameters);


// DataloggerController *datalogger;
// MotorController  *controller;
// ModbusController *modbus; 


// void setup(){
//     serial_begin();
//     DEBUG_SERIAL("SERIAL", "SERIAL BEGAN" );

//     /** Inicia o Log de dados via SDCard */
//     datalogger = new DataloggerController( "DataLogger via SDCard", SDCARD_LOGGING );

//     /** Inicia a comunicação WiFi para controle Modbus */ 
//     modbus = new ModbusController( "ModbusController");

//     /** Inicia a conexão Modbus */
//     modbus->begin_connection();

//     /** Aguarda atualização da hora */
//     DEBUG_SERIAL( "MODBUS REGISTER", "Aguardando atualização de data e hora...\n");
//     while (!modbus->mb.Coil(COIL_DT_SYNC)) {
//         modbus->scan();
//         vTaskDelay(100 / portTICK_PERIOD_MS);
//     };
//     DEBUG_SERIAL( "MODBUS REGISTER", String( modbus->mb.Coil(COIL_DT_SYNC) ) );

//     /** Inicia o controle dos motores */
//     controller = new MotorController( "MotorController", (SensorType_t)SENSOR_ANALOG, (MotorType_t)MOTOR_DC ); 
  
//     /** Escrita de informaçãoes */
//     logger_data_t log = { 0xCA };
//     datalogger->update_data( log, true);

//     /** Inicia as tasks */
//     xTaskCreate( &blinkTask, "BlinkTask", 1024, NULL, 5, NULL );
// }


// /** Loop principal */
// void loop() {
//     vTaskDelay(10 / portTICK_PERIOD_MS);
//     modbus->scan();
// }


// /** Task de Led blink */
// void blinkTask(void *pvParameters) {
//     gpio_set_direction( LED_BOARD, (gpio_mode_t) GPIO_MODE_OUTPUT );
//     while(true) {
//         gpio_set_level(LED_BOARD, true);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//         gpio_set_level(LED_BOARD, false);
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }


#include "../test/Sensor/SensorIncremental.h"
#include "modules/system/relay.h"

Relay *relay;

void setup() {
    relay = new Relay( "Rele de potencia");
    relay->turn_on();
    app_main(); 
}

void loop() {
}
