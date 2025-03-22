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

      float zenith  = spa_get_zenith()  +  90; // OFFSET para não explodir uint
      float azimuth = spa_get_azimuth() + 200; // OFFSET para não explodir uint

      // DEBUG_SERIAL( "ZENITE",  zenith  ); 
      // DEBUG_SERIAL( "AZIMUTE", azimuth ); 

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



// Manipulador de eventos WiFi (Arduino Framework)
void event_handler( WiFiEvent_t event) {
  switch (event) {
    // STA pronto para conectar
    case ARDUINO_EVENT_WIFI_STA_START:
      WiFi.begin(SSID_MASTER, PSD_MASTER);
      Serial.println("[WIFI HANDLER] WiFi Start!");
      break;

    // STA conectado
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      wifi_connection_state = true;
      wifi_retry_conn = 0;
      Serial.println("[WIFI HANDLER] WiFi STA Connected!");
      break;

    // STA recebeu um IP
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("[WIFI HANDLER] Got IP!");
      Serial.print("[WIFI HANDLER] IP Address: ");
      Serial.println(WiFi.localIP());
      break;

    // STA desconectado
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      wifi_connection_state = false;
      if (wifi_retry_conn++ < 10) {
        Serial.printf("[WIFI HANDLER] Try to reconnect %d times...\n", wifi_retry_conn);
        WiFi.reconnect();
      } else {
        Serial.println("[WIFI HANDLER] Rebooting...");
        ESP.restart();
      }
      break;

    default:
        break;
  }
}


ModbusController::ModbusController( const char *description, RTC_type_t RTC_TYPE ) 
  : description(description) {
  
  // Inicia conexão Wi-Fi
  WiFi.mode(WIFI_STA);
  
  #ifdef AZIMUTE_MODE
    // Configura o hostname que aparecerá no gateway
    WiFi.setHostname( "Tracker-Azi_1");
  #endif 
  #ifdef ZENITE_MODE
    // Configura o hostname que aparecerá no gateway
    WiFi.setHostname( "Tracker-Zen_1" );
  #endif 

  WiFi.onEvent( event_handler );
  WiFi.begin(SSID_MASTER, PSD_MASTER);

  // Aguarde a conexão WiFi
  while (!wifi_connection_state) {
    vTaskDelay( pdMS_TO_TICKS(100) );
  }
  DEBUG_SERIAL("WI-FI SERVICE", "wifi STA connection started" );
  vTaskDelay( pdMS_TO_TICKS(10) );

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
  xTaskCreate( modbus_Task, "modbusControllTask", 1024*16, this, 1, this->modbus_TaskHandler );
  DEBUG_SERIAL( "MODBUS TASK", "Modbus task inicilizated!" );
}



void ModbusController::scan( void ){
  this->mb->task();
}

void ModbusController::begin_connection( void ){
  // MODBUS INPUTS REGISTER ADDRESSES
  this->mb->addIreg( INPUT_SYSTEM_MODE,     0x00, 1 ); // (uint16_t) 
  this->mb->addIreg( INPUT_SENSOR_STATUS,   0x00, 1 ); // (uint16_t) 
  
  this->mb->addIreg( INPUT_SENSOR_POS,      0x00, 2 ); // (float) 
  this->mb->addIreg( INPUT_SUN_TARGET,      0x00, 2 ); // (float) 
  this->mb->addIreg( INPUT_ZENITH,          0x00, 2 ); // (float) 
  this->mb->addIreg( INPUT_AZIMUTH,         0x00, 2 ); // (float) 
  this->mb->addIreg( INPUT_POWER_GEN,       0x00, 2 ); // (float) 
  this->mb->addIreg( INPUT_TEMPERATURE,     0x00, 2 ); // (float) 
  this->mb->addIreg( INPUT_PRESURE,         0x00, 2 ); // (float) 

  this->mb->addIreg( INPUT_YEAR,             25 , 1 ); // (uint16_t) 
  this->mb->addIreg( INPUT_MONTH,            3  , 1 ); // (uint16_t) 
  this->mb->addIreg( INPUT_DAY,              20 , 1 ); // (uint16_t) 
  this->mb->addIreg( INPUT_HOUR,             8  , 1 ); // (uint16_t) 
  this->mb->addIreg( INPUT_MINUTE,           0  , 1 ); // (uint16_t) 
  this->mb->addIreg( INPUT_SECOND,           0  , 1 ); // (uint16_t) 
  
  // MODBUS HOLDING REGISTER ADDRESSES
  this->mb->addHreg( HR_STATE,              0xFF, 1 ); // (uint16_t) 
  this->mb->addHreg( HR_MOTOR_PV,           0x00, 2 ); // (float) 
  this->mb->addHreg( HR_MOTOR_KP,           0x00, 2 ); // (float) 
  this->mb->addHreg( HR_MOTOR_KI,           0x00, 2 ); // (float) 
  this->mb->addHreg( HR_MOTOR_KD,           0x00, 2 ); // (float) 
  this->mb->addHreg( HR_ALTITUDE,           0x00, 2 ); // (float) 
  this->mb->addHreg( HR_LATITUDE,           0x00, 2 ); // (float) 
  this->mb->addHreg( HR_LONGITUDE,          0x00, 2 ); // (float) 
  this->mb->addHreg( HR_YEAR,               0x00, 1 ); // (uint16_t) 
  this->mb->addHreg( HR_MONTH,              0x00, 1 ); // (uint16_t) 
  this->mb->addHreg( HR_DAY,                0x00, 1 ); // (uint16_t) 
  this->mb->addHreg( HR_HOUR,               0x00, 1 ); // (uint16_t) 
  this->mb->addHreg( HR_MINUTE,             0x00, 1 ); // (uint16_t) 
  this->mb->addHreg( HR_SECOND,             0x00, 1 ); // (uint16_t) 
  
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

