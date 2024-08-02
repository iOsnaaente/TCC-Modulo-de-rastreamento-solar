#include "includes/modbusController.h"

// Indica o status da conexão wifi
volatile bool wifi_connection_state = false;
volatile uint8_t wifi_retry_conn = 0;


// Eventos WiFi
static void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_id == SYSTEM_EVENT_STA_START ){
    esp_wifi_connect();
    DEBUG_SERIAL( "WIFI HANDLER", "Wifi Start!" );
  } else if (event_id == SYSTEM_EVENT_STA_CONNECTED ){
      wifi_connection_state = true;
      wifi_retry_conn = 0;
      DEBUG_SERIAL( "WIFI HANDLER", "Wifi SPA Connected!" );
  } else if ( event_id == IP_EVENT_STA_GOT_IP ) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    DEBUG_SERIAL("WIFI HANDLER", "Got IP!");
    DEBUG_SERIAL("WIFI HANDLER", String( ip4addr_ntoa((const ip4_addr_t*)&event->ip_info.ip)) );
  } else if ( event_id == SYSTEM_EVENT_STA_DISCONNECTED ){
    wifi_connection_state = false;
    if ((wifi_retry_conn++) != 10) {
      esp_wifi_connect();
      DEBUG_SERIAL( "WIFI HANDLER", "Try to reconnect " + String(wifi_retry_conn) + " times..." );
    } else {
      DEBUG_SERIAL( "WIFI HANDLER", "Rebooting!" );
      esp_restart();
    }
  }
}

ModbusController::ModbusController( const char *description ) 
  : description(description) {
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
  DEBUG_SERIAL("MODBUS SERVICE", "wifi STA connection started" );
}

void ModbusController::scan( void ){
  this->mb.task();
}

void ModbusController::begin_connection( void ){
  // Inicia o servidor modbus 
  this->mb.server();
  // MODBUS INPUTS REGISTER ADDRESSES
  this->mb.addIreg(INPUT_SYSTEM_MODE, 0x00);
  this->mb.addIreg(INPUT_SENSOR_POS, 0x00);
  this->mb.addIreg(INPUT_SENSOR_STATUS, 0x00);
  this->mb.addIreg(INPUT_SUN_TARGET, 0x00);
  this->mb.addIreg(INPUT_POWER_GEN, 0x00);
  this->mb.addIreg(INPUT_TEMPERATURE, 0x00);
  this->mb.addIreg(INPUT_PRESURE, 0x00);
  this->mb.addIreg(INPUT_YEAR, 0x00);
  this->mb.addIreg(INPUT_MONTH, 0x00);
  this->mb.addIreg(INPUT_DAY, 0x00);
  this->mb.addIreg(INPUT_HOUR, 0x00);
  this->mb.addIreg(INPUT_MINUTE, 0x00);
  this->mb.addIreg(INPUT_SECOND, 0x00);
  // MODBUS HOLDING REGISTER ADDRESSES
  this->mb.addHreg(HR_STATE, 0x00);
  this->mb.addHreg(HR_MOTOR_PV, 0x00);
  this->mb.addHreg(HR_MOTOR_KP, 0x00);
  this->mb.addHreg(HR_MOTOR_KI, 0x00);
  this->mb.addHreg(HR_MOTOR_KD, 0x00);
  this->mb.addHreg(HR_ALTITUDE, 0x00);
  this->mb.addHreg(HR_LATITUDE, 0x00);
  this->mb.addHreg(HR_LONGITUDE, 0x00);
  this->mb.addHreg(HR_YEAR, 0x00);
  this->mb.addHreg(HR_MONTH, 0x00);
  this->mb.addHreg(HR_DAY, 0x00);
  this->mb.addHreg(HR_HOUR, 0x00);
  this->mb.addHreg(HR_MINUTE, 0x00);
  this->mb.addHreg(HR_SECOND, 0x00);
  // MODBUS DISCRETES REGISTER ADDRESSES
  this->mb.addIsts(DISCRETE_ADC1_0, false);
  this->mb.addIsts(DISCRETE_ADC1_3, false);
  this->mb.addIsts(DISCRETE_ADC1_6, false);
  this->mb.addIsts(DISCRETE_FAIL, false);
  this->mb.addIsts(DISCRETE_SYNC, false);
  // MODBUS COILS REGISTER ADDRESSES
  this->mb.addCoil(COIL_POWER, false);
  this->mb.addCoil(COIL_LED, false);
  this->mb.addCoil(COIL_DT_SYNC, false);
  DEBUG_SERIAL("MODBUS SERVICE", "Modbus registers updated!" );

}


void ModbusController::update_datetime( struct datetime_buffer_t dt) {
  // Atualize os registradores Modbus com os valores da estrutura datetime_buffer_t
  this->mb.Ireg(INPUT_YEAR, dt.year);
  this->mb.Ireg(INPUT_MONTH, dt.month);
  this->mb.Ireg(INPUT_DAY, dt.day);
  this->mb.Ireg(INPUT_HOUR, dt.hour);
  this->mb.Ireg(INPUT_MINUTE, dt.minute);
  this->mb.Ireg(INPUT_SECOND, dt.second);
}

