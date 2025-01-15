#include "includes/modbusController.h"

// Indica o status da conexão wifi
volatile bool wifi_connection_state = false;
volatile uint8_t wifi_retry_conn = 0;

ModbusController* ModbusController::Instance = nullptr;

void modbus_Task( void * pvParameters ){
  ModbusController *controller = (ModbusController *)pvParameters;
  if (controller == nullptr) {
    DEBUG_SERIAL("ERROR", "ModbusController pointer is null!");
    return; // Para a task se o ponteiro for inválido
  }
  // Aramazena a hora do sistema 
  datetime_buffer_t datetime = {0}; 
  // Conta o tempo para atualizar a hora do sistema 
  uint32_t count = 0; 

  while (true) {

    // Verifica se é para atualizar o registrador de date e hora
    if ( count++ >= 100 ){
      count = 0;
      // Calcula a posição de referencia
      // this->rtc->get_datetime( datetime );
      datetime.year   = controller->mb->Ireg( INPUT_YEAR   );
      datetime.month  = controller->mb->Ireg( INPUT_MONTH  );
      datetime.day    = controller->mb->Ireg( INPUT_DAY    );
      datetime.hour   = controller->mb->Ireg( INPUT_HOUR   );
      datetime.minute = controller->mb->Ireg( INPUT_MINUTE );
      datetime.second = controller->mb->Ireg( INPUT_SECOND );
      // DEBUG_SERIAL( "UPDATE DATETIME", String(datetime.year) + "/" + String(datetime.month) + "/"+ String(datetime.day) + " "+ String(datetime.hour) + ":"+ String(datetime.minute) + ":"+ String(datetime.second) );

      // Calcula o SPA para pegar as localizaçoes aparentes de Azimute e Zenite 
      spa_att_location( LATITUDE, LONGITUDE );
      spa_att_datetime( datetime );
      spa_att_position();

      float zenith  = spa_get_zenith();
      float azimuth = spa_get_azimuth();

      // DEBUG_SERIAL( "ZENITE",  (int16_t) zenith ); 
      // DEBUG_SERIAL( "AZIMUTE", (int16_t) azimuth ); 

      controller->mb->Ireg( INPUT_ZENITH,  zenith  );
      controller->mb->Ireg( INPUT_AZIMUTH, azimuth );

      #ifdef ZENITE_MODE
        controller->mb->Ireg( INPUT_SUN_TARGET, zenith  );
      #endif

      #ifdef AZIMUTE_MODE
        controller->mb->Ireg( INPUT_SUN_TARGET, azimuth );
      #endif
      // DEBUG_SERIAL( "SUN POS", controller->mb->Ireg( INPUT_SUN_TARGET) );
    }

    // Scaneia mudanças de registradores 
    controller->mb->task();
    vTaskDelay( pdMS_TO_TICKS(10) );

  }
}



// Eventos WiFi
static void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  // STA pronto para conectar 
  if (event_id == SYSTEM_EVENT_STA_START ){
    esp_wifi_connect();
    DEBUG_SERIAL( "WIFI HANDLER", "Wifi Start!" );
  
  // STA conectado 
  } else if (event_id == SYSTEM_EVENT_STA_CONNECTED ){
      wifi_connection_state = true;
      wifi_retry_conn = 0;
      DEBUG_SERIAL( "WIFI HANDLER", "Wifi SPA Connected!" );
  
  // Recebimento de um IP para o STA 
  } else if ( event_id == IP_EVENT_STA_GOT_IP ) {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    DEBUG_SERIAL("WIFI HANDLER", "Got IP!");
    DEBUG_SERIAL("WIFI HANDLER", String( ip4addr_ntoa((const ip4_addr_t*)&event->ip_info.ip)) );
  
  // STA desconectado 
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


ModbusController::ModbusController( const char *description, RTC_type_t RTC_TYPE ) 
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
    vTaskDelay( pdMS_TO_TICKS(100) );
  }
  DEBUG_SERIAL("WI-FI SERVICE", "wifi STA connection started" );
  vTaskDelay( pdMS_TO_TICKS(1000) );

  // // Init datetime counter 
  // datetime_buffer_t dt = {0};
  // if ( RTC_TYPE == RTC_DS3231 ){
  //   this->rtc = new DS3231( "RTC DS3231", dt );
  // }else if ( RTC_TYPE == RTC_SOFT ){
  //   this->rtc = new SoftRTC( "RTC Software", dt );
  // }
  // DEBUG_SERIAL( "RTC INIT", "RTC inicializated!" );

  this->mb = new ModbusIP();

  if (this->mb != nullptr) {
      this->mb->server( 502 );
      // continue com o restante do código
  } else {
      DEBUG_SERIAL("ERROR", "Failed to initialize ModbusIP object!");
  }

  // Inicializa os registradores modbus
  this->begin_connection();

  // Inicializa a task de atualização da hora e Scan modbus 
  xTaskCreate( modbus_Task, "modbusControllTask", 1024*8, this, 1, this->modbus_TaskHandler );
  DEBUG_SERIAL( "MODBUS TASK", "Modbus task inicilizated!" );
}



void ModbusController::scan( void ){
  this->mb->task();
}

void ModbusController::begin_connection( void ){
  // MODBUS INPUTS REGISTER ADDRESSES
  this->mb->addIreg( INPUT_SYSTEM_MODE,     0x00, (uint16_t) 1 );
  this->mb->addIreg( INPUT_SENSOR_STATUS,   0x00, (uint16_t) 1 );
  this->mb->addIreg( INPUT_SENSOR_POS,      0x00, (float)    2 );
  this->mb->addIreg( INPUT_SUN_TARGET,      0x00, (float)    2 );
  this->mb->addIreg( INPUT_ZENITH,          0x00, (float)    2 );
  this->mb->addIreg( INPUT_AZIMUTH,         0x00, (float)    2 );
  this->mb->addIreg( INPUT_POWER_GEN,       0x00, (float)    2 );
  this->mb->addIreg( INPUT_TEMPERATURE,     0x00, (float)    2 );
  this->mb->addIreg( INPUT_PRESURE,         0x00, (float)    2 );
  this->mb->addIreg( INPUT_YEAR,            0x00, (uint16_t) 1 );
  this->mb->addIreg( INPUT_MONTH,           0x00, (uint16_t) 1 );
  this->mb->addIreg( INPUT_DAY,             0x00, (uint16_t) 1 );
  this->mb->addIreg( INPUT_HOUR,            0x00, (uint16_t) 1 );
  this->mb->addIreg( INPUT_MINUTE,          0x00, (uint16_t) 1 );
  this->mb->addIreg( INPUT_SECOND,          0x00, (uint16_t) 1 );
  
  // MODBUS HOLDING REGISTER ADDRESSES
  this->mb->addHreg( HR_STATE,              0x00, (uint16_t) 1 );
  this->mb->addHreg( HR_MOTOR_PV,           0x00, (float)    2 );
  this->mb->addHreg( HR_MOTOR_KP,           0x00, (float)    2 );
  this->mb->addHreg( HR_MOTOR_KI,           0x00, (float)    2 );
  this->mb->addHreg( HR_MOTOR_KD,           0x00, (float)    2 );
  this->mb->addHreg( HR_ALTITUDE,           0x00, (float)    2 );
  this->mb->addHreg( HR_LATITUDE,           0x00, (float)    2 );
  this->mb->addHreg( HR_LONGITUDE,          0x00, (float)    2 );
  this->mb->addHreg( HR_YEAR,               0x00, (uint16_t) 1 );
  this->mb->addHreg( HR_MONTH,              0x00, (uint16_t) 1 );
  this->mb->addHreg( HR_DAY,                0x00, (uint16_t) 1 );
  this->mb->addHreg( HR_HOUR,               0x00, (uint16_t) 1 );
  this->mb->addHreg( HR_MINUTE,             0x00, (uint16_t) 1 );
  this->mb->addHreg( HR_SECOND,             0x00, (uint16_t) 1 );
  
  // MODBUS DISCRETES REGISTER ADDRESSES
  this->mb->addIsts( DISCRETE_ADC1_0,       false );
  this->mb->addIsts( DISCRETE_ADC1_3,       false );
  this->mb->addIsts( DISCRETE_ADC1_6,       false );
  this->mb->addIsts( DISCRETE_FAIL,         false );
  this->mb->addIsts( DISCRETE_SYNC,         false );

  // MODBUS COILS REGISTER ADDRESSES
  this->mb->addCoil( COIL_POWER,            false );
  this->mb->addCoil( COIL_LED,              false );
  this->mb->addCoil( COIL_DT_SYNC,          false );
  DEBUG_SERIAL("MODBUS SERVICE", "Modbus registers updated!" );
}


void ModbusController::update_datetime( struct datetime_buffer_t dt) {  
  // Atualize os registradores Modbus com os valores da estrutura dt_buffer_t
  this->mb->Ireg(INPUT_YEAR, dt.year);
  this->mb->Ireg(INPUT_MONTH, dt.month);
  this->mb->Ireg(INPUT_DAY, dt.day);
  this->mb->Ireg(INPUT_HOUR, dt.hour);
  this->mb->Ireg(INPUT_MINUTE, dt.minute);
  this->mb->Ireg(INPUT_SECOND, dt.second);
  
  /** 
   * @note Não usarei no momento 
  
  // // Atualiza os registradores locais para armazenarem o novo DT 
  // datetime_buffer_t datetime = {0};
  // datetime.year = dt.year;
  // datetime.month = dt.month;
  // datetime.day = dt.day;
  // datetime.hour = dt.hour;
  // datetime.minute = dt.minute;
  // datetime.second = dt.second;
  // // Atualiza o novo DT no RTC 
  // this->rtc->set_datetime( datetime );
  
  */
}

