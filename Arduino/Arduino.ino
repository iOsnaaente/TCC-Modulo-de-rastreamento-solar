#include <sys/cdefs.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "registradores.h"
#include "pinout.h"

// #include "driver/pulse_cnt.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/uart.h"

#include "esp_timer.h"

// Crie uma instância ModbusIP
#include <ModbusIP_ESP8266.h>
ModbusIP mb;




#define UART_BAUDRATE 115200


#define ENCODER_MAGNETIC 


void setup(void) {

  // Inicializa o Serial
  const uart_config_t uart_config = {
    .baud_rate = UART_BAUDRATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, UART_TXD0, UART_RXD0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
  printf("Serial was initiated\n");

  // Aguarda conexão do mestre
  init_modbus_config();

  // Inicializa a struct Motor com as configurações
  bdc_motor_init_device();
  bdc_motor_set_speed((double)0);
  bdc_motor_forward();

// Inicializa o contador de pulsos
#ifdef ENCODER_INCREMENTAL
  init_pulses_counter();
#endif

#ifdef ENCODER_MAGNETIC
  init_magnetic_encoder();
#endif

  // Inicializa o relé
  init_relay_configurations();
  turn_relay_on();

  // Seta direção do LED da placa
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

  spa_att_location( -29.16530765942215, -54.89831672609559, 101.0, 300 );
  spa_att_datetime(24, 1, 9, 18, 54, 1.1015 );
  spa_att_position();
  printf( "Azimute: %3.4fº \tZenite: %3.4fº", spa_get_azimuth(), spa_get_zenith() ); 

}

int i = 5400;
void loop(void) {

  // Lidar com as solicitações Modbus
  mb.task();
  // Atualiza o endereço relacionado ao LED_BUILTIN da placa
  gpio_set_level(LED_BUILTIN, (uint32_t)mb.Coil(COIL_LED));

  printf( "Encoder read: %3.2f\n", read_angle() );
  printf( "Azimute: %3.4fº \tZenite: %3.4fº\n", spa_get_azimuth(), spa_get_zenith() ); 

  // check_connection();
  vTaskDelay(pdMS_TO_TICKS(100));
}
