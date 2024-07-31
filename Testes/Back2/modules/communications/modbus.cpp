/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_netif.h>

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "esp_chip_info.h"
#include "esp_mac.h"

#include <nvs_flash.h>

#include "registradores.h"

#ifndef DEBUG
#define DEBUG
#endif

// Defina as credenciais da sua rede WiFi
const char *SSID_MASTER = "ESP32_TCC_MASTER_MODBUS";
const char *PSD_MASTER = "12051999";

const char *SERVER_IP = "192.168.12.1";
const uint16_t SERVER_PORT = 502;

volatile uint8_t wifi_retry_conn = 0;

// Eventos WiFi
static void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  switch (event_id) {
    case SYSTEM_EVENT_STA_START:
      esp_wifi_connect();
      printf("Wifi Start. \n");
      break;
    case SYSTEM_EVENT_STA_CONNECTED:
      wifi_connection_state = true;
      wifi_retry_conn = 0;
      break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
      wifi_connection_state = false;
      if ((wifi_retry_conn++) != 10) {
        esp_wifi_connect();
        printf("Try reconnect %d times... \n", wifi_retry_conn);
      } else {
        printf("Rebooting\n");
        esp_restart();
      }
      break;
    default:
      break;
  }
}


void init_wifi_config(void) {
  // Inicia o non-volatile storage para armazenamento das variáveis do wifi
  nvs_flash_init();
  // Inicia as configurações padrão para inicialização de uma rede TCP/IP
  esp_netif_init();
  esp_event_loop_create_default();
  // Cria e configura a interface WiFi no modo AP_STA
  esp_netif_create_default_wifi_sta();
  // esp_netif_create_default_wifi_ap();
  wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_initiation);
  // Cria os handlers de interrupções wifi
  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id);
  // Seta as configurações da conexão
  wifi_config_t wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char *)wifi_config.sta.ssid, SSID_MASTER);
  strcpy((char *)wifi_config.sta.password, PSD_MASTER);
  esp_wifi_set_config((wifi_interface_t)ESP_IF_WIFI_STA, &wifi_config);
  esp_wifi_start();
  // Seleciona o modo STA
  esp_wifi_set_mode(WIFI_MODE_STA);
  // Aguarde a conexão WiFi
  esp_wifi_connect();
  while (!wifi_connection_state) {
  };
  printf("Wifi connected!!\n");
  vTaskDelay( 1 / portTICK_PERIOD_MS);
  // Inicia o servidor modbus 
  mb.server();
  init_modbus_registers();
}


void modbus_update_datetime(struct datetime_buffer_t dt) {
  // Atualize os registradores Modbus com os valores da estrutura datetime_buffer_t
  mb.Ireg(INPUT_YEAR, dt.year);
  mb.Ireg(INPUT_MONTH, dt.month);
  mb.Ireg(INPUT_DAY, dt.day);
  mb.Ireg(INPUT_HOUR, dt.hour);
  mb.Ireg(INPUT_MINUTE, dt.minute);
  mb.Ireg(INPUT_SECOND, dt.second);
}

void init_modbus_registers(void) {
  // MODBUS INPUTS REGISTER ADDRESSES
  mb.addIreg(INPUT_SYSTEM_MODE, 0x00);
  mb.addIreg(INPUT_SENSOR_POS, 0x00);
  mb.addIreg(INPUT_SENSOR_STATUS, 0x00);
  mb.addIreg(INPUT_SUN_TARGET, 0x00);
  mb.addIreg(INPUT_POWER_GEN, 0x00);
  mb.addIreg(INPUT_TEMPERATURE, 0x00);
  mb.addIreg(INPUT_PRESURE, 0x00);
  mb.addIreg(INPUT_YEAR, 0x00);
  mb.addIreg(INPUT_MONTH, 0x00);
  mb.addIreg(INPUT_DAY, 0x00);
  mb.addIreg(INPUT_HOUR, 0x00);
  mb.addIreg(INPUT_MINUTE, 0x00);
  mb.addIreg(INPUT_SECOND, 0x00);

  // MODBUS HOLDING REGISTER ADDRESSES
  mb.addHreg(HR_STATE, 0x00);
  mb.addHreg(HR_MOTOR_PV, 0x00);
  mb.addHreg(HR_MOTOR_KP, 0x00);
  mb.addHreg(HR_MOTOR_KI, 0x00);
  mb.addHreg(HR_MOTOR_KD, 0x00);
  mb.addHreg(HR_ALTITUDE, 0x00);
  mb.addHreg(HR_LATITUDE, 0x00);
  mb.addHreg(HR_LONGITUDE, 0x00);
  mb.addHreg(HR_YEAR, 0x00);
  mb.addHreg(HR_MONTH, 0x00);
  mb.addHreg(HR_DAY, 0x00);
  mb.addHreg(HR_HOUR, 0x00);
  mb.addHreg(HR_MINUTE, 0x00);
  mb.addHreg(HR_SECOND, 0x00);

  // MODBUS DISCRETES REGISTER ADDRESSES
  mb.addIsts(DISCRETE_ADC1_0, false);
  mb.addIsts(DISCRETE_ADC1_3, false);
  mb.addIsts(DISCRETE_ADC1_6, false);
  mb.addIsts(DISCRETE_FAIL, false);
  mb.addIsts(DISCRETE_SYNC, false);

  // MODBUS COILS REGISTER ADDRESSES
  mb.addCoil(COIL_POWER, false);
  mb.addCoil(COIL_LED, false);
  mb.addCoil(COIL_DT_SYNC, false);
}