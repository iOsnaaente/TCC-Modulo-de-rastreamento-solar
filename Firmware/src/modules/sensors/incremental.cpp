#include "incremental.h"


// Inicializa o ponteiro estático
IncrementalSensor* IncrementalSensor::Instance = nullptr;

IncrementalSensor::IncrementalSensor(const char* description, gpio_num_t pin_enc_a, gpio_num_t pin_enc_b )  
    : Sensor(description), enc_a(pin_enc_a), enc_b(pin_enc_b) {
    // Configuração dos pinos de interrupção
    gpio_config_t io_conf = {
      .pin_bit_mask = (1ULL << enc_a) | (1ULL << enc_b),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_ENABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_POSEDGE, // POS EDGE = POSITIVE EDGE 
    };
    gpio_config(&io_conf);

    // Inicializa o semáforo
    xSensorSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSensorSemaphore);

    // Cria a sua instancia para uso das funções da classe 
    Instance = this; 

    // Instala a rotina de tratamento de interrupção
    gpio_install_isr_service(0);
    gpio_isr_handler_add( enc_a, [](void *arg){Instance->count_pulsesISR(nullptr);}, encoder_a_handler );
    gpio_isr_handler_add( enc_b, [](void *arg){Instance->count_pulsesISR(nullptr);}, encoder_b_handler );

    // Inicializa a task de medição dos pulsos 
    xTaskCreate( [](void *arg){Instance->count_pulsesTask(nullptr);}, "sensor_incrementalTask", 2048, NULL, 1, encoder_task_Handler );
}


void IRAM_ATTR IncrementalSensor::count_pulsesISR(void* pvParameters) {
    uint8_t sensor = gpio_get_level(enc_a) | gpio_get_level(enc_b);
    // Entra na região de memória compartilhada
    if (xSemaphoreTake(xSensorSemaphore, pdMS_TO_TICKS(1) ) == pdTRUE) {
      if (sensor == 0b0010 ) {          // 2 = 0b0010
        forward_count++;
      } else if (sensor == 0b0011 ) {   // 3 = 0b0011
        backward_count--;
      }else{
        forward_count++;
      }
      xSemaphoreGive(xSensorSemaphore);
    }
}


// Função da tarefa FreeRTOS
void IncrementalSensor::count_pulsesTask( void *pvParameters ) {
  const TickType_t xDelay = pdMS_TO_TICKS(MEASURE_PERIOD); 
  TickType_t xLastWakeTime = xTaskGetTickCount(); 
  while (true) {

    vTaskDelayUntil(&xLastWakeTime, xDelay);
    if (xSemaphoreTake(xSensorSemaphore, pdMS_TO_TICKS(1) ) == pdTRUE ) { 
        // Computa o numero de pulsos
        pulse_count = (backward_count + forward_count);

        // Pega a direção de giro
        if (backward_count < forward_count) {
            direction = FORWARD;
        } else if (pulse_count == 0) {
            direction = STOPPED;
        } else {
            direction = BACKWARD;
        }

        // Reinicia as contagens para a próxima iteração
        backward_count = 0;
        forward_count = 0;

        // Calcula a diferença de tempo entre as medições
        // int64_t esp_timer_microseconds = esp_timer_get_time();
        // dt_last_measure = (esp_timer_microseconds - time_last_measure);
        // time_last_measure = esp_timer_microseconds;

        // Libera o semáforo
        xSemaphoreGive(xSensorSemaphore); 
    }
  }
}


uint8_t IncrementalSensor::get_direction( void ) {
  return this->direction;
}

int32_t IncrementalSensor::read_raw(){
  return pulse_count;
}

double IncrementalSensor::read_scaled(){
  this->scaled_position = (double)(this->read_raw()*SCALE); 
  return this->scaled_position;
}

uint8_t IncrementalSensor::get_status(){
  return true;
}


esp_err_t IncrementalSensor::set_measure_time( uint8_t value ){
  MEASURE_PERIOD = value;
  return ESP_OK;
}

esp_err_t IncrementalSensor::set_scale( double value ){
  SCALE = value;
  return ESP_OK;
}

double IncrementalSensor::read_velocity(void){
  return 0.0;
}