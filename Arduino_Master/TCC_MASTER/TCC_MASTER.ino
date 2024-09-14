#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include <esp_log.h>
#include <stdio.h>

#include "registradores.h"

#include <ModbusIP_ESP8266.h>
#include <DS3231.h>
#include <Wire.h>

#include <WiFi.h>


#define UNKNOW 0   // Operation mode
#define AZIMUTH 1  // Operation mode
#define ZENITH 2   // Operation mode

DS3231 rtc;
ModbusIP mb;

// Defina as credenciais do ponto de acesso
IPAddress AP_IP(192, 168, 12, 1);
const char *SSID_MASTER = "ESP32_TCC_MASTER_MODBUS";
const char *PSD_MASTER = "12051999";


wifi_sta_list_t wifi_sta_list;
tcpip_adapter_sta_list_t adapter_sta_list;

// Defina o número máximo de clientes conforme necessário
const int MAX_CLIENTS = 32;

// Gerenciamento de novos clientes
struct client_params_t {
  uint8_t sta_aid;
  uint8_t sta_mac[6];
  esp_ip4_addr_t sta_ip;
  bool sta_connected;
  uint8_t sta_mode;
  bool sta_datetime_sync;
};
client_params_t clients[MAX_CLIENTS];

bool flag_clients_to_sync = false;

// Modbus array data
uint16_t modbus_data[6];

uint32_t count = 0;
bool century = false;
bool h12Flag;
bool pmFlag;


// Handler das conexões entre dispositivos
static void wifi_event_handler(void *args, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  // Quando uma nova STA é conectada ela é adicionada à lista de clientes
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *sta_connected = (wifi_event_ap_staconnected_t *)event_data;
    for (uint8_t i = 0; i < MAX_CLIENTS; i++) {
      if (!clients[i].sta_connected) {
        clients[i].sta_aid = sta_connected->aid;
        memcpy(clients[i].sta_mac, sta_connected->mac, sizeof(clients[i].sta_mac));
        clients[i].sta_ip = { 0 };
        clients[i].sta_connected = true;
        clients[i].sta_mode = UNKNOW;
        clients[i].sta_datetime_sync = true;
        printf("New client connected!\nAID: %d\n", clients[i].sta_aid);
        break;
      }
    }

    // Quando uma STA é desconectada ela é removida da lista de clientes
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t *sta_disconnected = (wifi_event_ap_stadisconnected_t *)event_data;
    for (uint8_t i = 0; i < MAX_CLIENTS; i++)
      if (!clients[i].sta_aid == sta_disconnected->aid)
        clients[i] = { 0 };
    printf("Client with AID:%d disconnected!\n", sta_disconnected->aid);

    // Quando um cliente recebe um IP, ele é associado dentro da lista de clientes relacionado a ele
  } else if (event_base == IP_EVENT && event_id == IP_EVENT_AP_STAIPASSIGNED) {
    printf("AP STA IP ASSIGNED...\n");
    // Prepara uma lista para salvar os STA conectados
    memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
    memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
    esp_wifi_ap_get_sta_list(&wifi_sta_list);
    tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
    // Compara com os STAs já conectados
    for (uint8_t i = 0; i < adapter_sta_list.num; i++) {
      tcpip_adapter_sta_info_t sta = adapter_sta_list.sta[i];
      for (uint8_t j = 0; j < MAX_CLIENTS; j++) {
        if (clients[j].sta_aid && !memcmp(clients[j].sta_mac, sta.mac, sizeof(clients[j].sta_mac))) {
          clients[i].sta_ip = sta.ip;
          printf("Cliente AID:%d encontrado pelo MAC.\nIP associado: %s \n", clients[i].sta_aid, ip4addr_ntoa((const ip4_addr_t *)&(sta.ip)));
          break;
        }
      }
    }
    flag_clients_to_sync = true;
  }
}

void setup() {
  // Inicializa o Serial
  Serial.begin(115200);
  printf("esp idf version:%s \n\r", esp_get_idf_version());
  memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
  memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));


  // Inicializa o barramento I2C
  Wire.begin();
  printf("Wire began!\n");
  // Atualiza a hora se necessário
  // set_datetime();


  // Inicializa o AP
  WiFi.softAPConfig(AP_IP, AP_IP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_MASTER, PSD_MASTER);


  // Registrar os callbacks para eventos WiFi
  esp_event_handler_instance_register( WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
  esp_event_handler_instance_register( IP_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
  printf("Server AP inicilizated!\nIP do Access Point: %s\n", WiFi.softAPIP());

  // Inicia o Modbus
  mb.client();
  printf("Modbus cliente (Master) inicilizated!\n");
}

void loop() {

  // Execute as tarefas do Modbus
  mb.task();

  // Verifica se os clientes estão sincronizados ou não
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].sta_datetime_sync) {
      // printf("Client[AID:%d] have to sync!\n", clients[i].sta_aid);
      IPAddress clientIP(clients[i].sta_ip.addr);  // clients[i].sta_ip é do tipo esp_ip4_addr e precisa ser IPAddress
      if (!mb.isConnected(clientIP)) {
        mb.connect(clientIP);
      }
      mb.writeHreg(clientIP, HR_YEAR, modbus_data, 6);
      mb.writeCoil(clientIP, COIL_DT_SYNC, true);
      bool ret_mb[1] = { 0 };
      mb.readCoil(clientIP, COIL_DT_SYNC, ret_mb, 1);
      if (ret_mb[0]) {
        Serial.println("Flag_clients_to_sync atualizada com base no COIL_DT_SYNC!");
        clients[i].sta_datetime_sync = false;
      }
    }
  }
  // Atualiza a aproximadamente 1s
  if ((count++) % 10 == 0)
    update_datetime();
  vTaskDelay(pdMS_TO_TICKS(100));
}


void set_datetime(void) {
  rtc.setClockMode(false);  // set to 24h
  rtc.setYear(24);
  rtc.setMonth(1);
  rtc.setDate(10);
  rtc.setDoW(3);
  rtc.setHour(18);
  rtc.setMinute(12);
  rtc.setSecond(0);
  printf("datetime updated!\n");
}

void update_datetime(void) {
  // Atualiza a array de data
  modbus_data[0] = rtc.getYear();
  modbus_data[1] = rtc.getMonth(century);
  modbus_data[2] = rtc.getDate();
  modbus_data[3] = rtc.getHour(h12Flag, pmFlag);
  modbus_data[4] = rtc.getMinute();
  modbus_data[5] = rtc.getSecond();
  // Impressão formatada usando printf
  printf("%d-%d-%d %d:%d:%d\n", modbus_data[0], modbus_data[1], modbus_data[2], modbus_data[3], modbus_data[4], modbus_data[5]);
}
