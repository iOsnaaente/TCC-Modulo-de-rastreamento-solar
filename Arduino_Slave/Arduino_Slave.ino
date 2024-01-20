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
double KP = 1.00;
double KD = 0.10;
double KI = 0.00;

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

  // Aguarda conexão do mestre
  init_wifi_config();
  // Sinaliza que o wifi esta conectado
  gpio_set_level(LED_BUILTIN, true);

  // Aguarda atualização da hora
  printf("Waiting for datetime update by master connection...\n");
  while (!mb.Coil(COIL_DT_SYNC)) {
    mb.task();
  };
  spa_datetime.year = mb.Hreg(HR_YEAR);
  spa_datetime.month = mb.Hreg(HR_MONTH);
  spa_datetime.day = mb.Hreg(HR_DAY);
  spa_datetime.hour = mb.Hreg(HR_HOUR);
  spa_datetime.minute = mb.Hreg(HR_MINUTE);
  spa_datetime.second = mb.Hreg(HR_SECOND);
  gpio_set_level(LED_BUILTIN, false );

#ifdef DEBUG_CONTROL
  printf("%d/%d/%d - %d:%d:%d [Sync=%d]\n",
         mb.Hreg(HR_YEAR),
         mb.Hreg(HR_MONTH),
         mb.Hreg(HR_DAY),
         mb.Hreg(HR_HOUR),
         mb.Hreg(HR_MINUTE),
         mb.Hreg(HR_SECOND),
         mb.Coil(COIL_DT_SYNC));
  printf("%d/%d/%d - %d:%d:%d\n", spa_datetime.year, spa_datetime.month, spa_datetime.day, spa_datetime.hour, spa_datetime.minute, spa_datetime.second);
#endif

  // Inicia o RTC
  init_rtc(spa_datetime);
  // Atualiza o SPA com o datetime
  spa_att_datetime(spa_datetime);
  // Atualiza a posição geográfica
  spa_att_location(LATITUDE, LONGITUDE);
  // Computa os valores de posição solar
  spa_att_position();

  // Inicializa a struct Motor com as configurações
  bdc_motor_init_device();
  bdc_motor_set_speed((double)0);
  bdc_motor_forward();

  // Inicializa Encoder
  init_magnetic_encoder();

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

  // Lidar com as solicitações Modbus
  mb.task();

  // Atualiza o endereço relacionado ao LED_BUILTIN da placa
  gpio_set_level(LED_BUILTIN, (uint32_t)mb.Coil(COIL_LED));

  // Atualiza o valor de datetime através do RTC
  datetime_update_from_rtc(&spa_datetime);
  modbus_update_datetime(spa_datetime);

  // Atualiza a estrutura do SPA
  spa_att_datetime(spa_datetime);
  // Computa os valores do SPA
  spa_att_position();

  // Malha de controle
  mb.Ireg(INPUT_SENSOR_POS, read_raw_angle());
  double sensor_pos = (((double)mb.Ireg(INPUT_SENSOR_POS) / ((double)(4096))) * 360.0);
  double sun_pos = 0.0;
#ifdef AZIMUTE_MODE
  sun_pos = spa_get_azimuth();
#endif
#ifdef ZENITE_MODE
  sun_pos = spa_get_zenith();
#endif
  double error = (sun_pos - sensor_pos);
  int_acum += error;
  double derivative = (error - prev_error);
  prev_error = error;
  bdc_motor_out += (double)((KP * error) + (KD * derivative) + (KI * int_acum));
  if (bdc_motor_out > 0) {
    bdc_motor_forward();
  } else {
    bdc_motor_out *= -1;
    bdc_motor_backward();
  }
  if (bdc_motor_out > 8192) bdc_motor_out = 8192;
  bdc_motor_set_speed((uint32_t)bdc_motor_out);

  if ((count++) % 10 == 0) {
    printf("%d/%d/%d - %d:%d:%d\n", spa_datetime.year, spa_datetime.month, spa_datetime.day, spa_datetime.hour, spa_datetime.minute, spa_datetime.second);
    printf("Encoder read: %3.2f\n", read_angle());
    printf("Azimute: %3.4fº \tZenite: %3.4fº\n", spa_get_azimuth(), spa_get_zenith());
    printf("Error: %f\n", error);
    printf("Motor speed: %f / %f% \n", bdc_motor_out, (double)(bdc_motor_out/8192)*100 );
  }

  // check_connection();
  delayMicroseconds(LOOP_TIME_MS * 1000 - (micros() - loop_time));
}
