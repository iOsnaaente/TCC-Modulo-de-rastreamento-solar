#ifndef INCREMENTAL_SENSOR_H
#define INCREMENTAL_SENSOR_H

#include "../../interfaces/sensor.h"
#include "../../controllers/includes/SerialController.h"

#include "driver/gpio.h"
#include "driver/timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define  FORWARD   1 
#define  STOPPED   0
#define  BACKWARD -1 

/* 
 * Função de interrupção para contagem dos pulsos. Para a contagem de pulsos, há dois encoders com
 *     defasagem para detecção do sentido de rotação.
 *
 * O sinal de leitura será do tipo: 
 * 
 * Sinal A  __|¯¯¯¯|____|¯¯¯¯|____|¯¯¯¯|____|¯
 * Sinal B  ____|¯¯¯¯|____|¯¯¯¯|____|¯¯¯¯|____
 * LeituraA __|_________|_________|_________|_ 
 * LeituraB ____|_________|_________|_________ 
 * Leitura  __|_|_______|_|_______|_|_______|_
 * 
 * Para saber o sentido de rotação, basta ler um dos dois sinais A ou B:
 * Lendo A: 
 *      Sentido horário     -> (A=1,B=0)
 *      Sentido antihorário -> (A=1,B=1) 
 * Lendo B: 
 *      Sentido horário     -> (A=0,B=1)
 *      Sentido antihorário -> (A=1,B=1)
 *
 */
// void IRAM_ATTR count_pulsesISR(void* pvParameters);

/* 
 * Task para contagem periódica de pulsos. O valor da velocidade será salvo dentro da
 *     da variável global ```r_vel``` e ```l_vel```. 
 * O periodo de contagem é dado por ```PULSES_COUNT_PERIOD_MS``` e deve ser ajustado de 
 *     acordo com a resolução do sensor.
 */
// void count_pulsesTask( void * pvParameters );


class IncrementalSensor : public Sensor {
protected:
    // Handlers de interrupções e tasks 
    gpio_isr_handle_t *encoder_a_handler;
    gpio_isr_handle_t *encoder_b_handler;
    TaskHandle_t      *encoder_task_Handler;
    
    // Semáforo para proteger o acesso às variáveis compartilhadas
    SemaphoreHandle_t xSensorSemaphore;
    
    // Instancia da classe this
    static IncrementalSensor *Instance;

    // Armazena o tempo da ultima medição
    volatile uint32_t time_last_measure = 0;
    volatile uint32_t dt_last_measure = 0;

    // Contadores de passos para cada sentido
    volatile int32_t forward_count;
    volatile int32_t backward_count;

    // Pinos de interrupção
    volatile gpio_num_t enc_a;
    volatile gpio_num_t enc_b;    

    // Métodos de interrupção e tasks 
    void IRAM_ATTR count_pulsesISR( void * pvParameters );
    void count_pulsesTask( void * pvParameters );

    // Variaveis de controle 
    uint8_t MEASURE_PERIOD = 15;
    double SCALE = 1.0;

public:

    // Pulsos totais e sentido de giro 
    volatile int32_t pulse_count;
    volatile uint8_t direction;
    uint8_t status;
    
    // Construtor
    IncrementalSensor(const char* description, gpio_num_t inc_a, gpio_num_t inc_b );

    int32_t read_raw( void ) override;
    double read_scaled( void ) override;
    uint8_t get_status( void ) override;
    uint8_t get_direction( void );  

    esp_err_t set_measure_time( uint8_t value );
    esp_err_t set_scale( double value );
};

#endif // INCREMENTAL_SENSOR_H