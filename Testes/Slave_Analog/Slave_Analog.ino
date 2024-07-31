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

#define UART_BAUDRATE 115200
#define LATITUDE -29.16530765942215
#define LONGITUDE -54.89831672609559

/* UNCOMMENT ONE OF THAT TO USE IN SYSTEM */
// #define AZIMUTE_MODE
#define ZENITE_MODE

/* To print stuffs in the screen */
// #define DEBUG_CONTROL


/* Variáveis de controle */
double KP = 1.000;
double KD = 0.010;
double KI = 0.001;

// Indica o status da conexão wifi
volatile bool wifi_connection_state = false;

// Datetime do sistema
struct datetime_buffer_t {
  uint16_t year;
  uint16_t month;
  uint16_t day;
  uint16_t hour;
  uint16_t minute;
  uint16_t second;
  bool dt_sync;
};
struct datetime_buffer_t spa_datetime = { 0 };

// Modbus
ModbusIP mb;

/* velocidade de saída */
double bdc_motor_out = 0;
double prev_error = 0;
double int_acum = 0;


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

  // Seta direção do LED da placa
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);
  gpio_set_level(LED_BUILTIN, false);

  // Sinaliza que o wifi esta conectado
  gpio_set_level(LED_BUILTIN, true);
  // Aguarda conexão do mestre

  // Inicia o RTC
  init_rtc(spa_datetime);
  // Sinaliza que a hora foi atualizada
  gpio_set_level(LED_BUILTIN, false);

  // Inicializa a struct Motor com as configurações
  bdc_motor_init_device();
  bdc_motor_set_speed((double)0);
  bdc_motor_forward();

  // Inicializa Encoder
  init_analog_encoder();

  // Inicializa Relé
  init_relay_configurations();
  turn_relay_on();
}


/* Compensa o tempo de loop para garantir sincronia de medição */
#define LOOP_TIME_MS 200
uint32_t count = 0;
uint32_t loop_time;
void loop(void) {
  loop_time = micros();
  
  double pot = read_angle();
  double vel = ((pot/180)-1)*8192;
  if ( vel <= 0 ){
    bdc_motor_backward();
    vel *= -1;
  }else{
    bdc_motor_forward();
  }
  bdc_motor_set_speed((uint32_t) vel );
  printf("Motor speed: %f / %f% \n", vel );

  // check_connection();
  delayMicroseconds(LOOP_TIME_MS * 1000 - (micros() - loop_time));
}
