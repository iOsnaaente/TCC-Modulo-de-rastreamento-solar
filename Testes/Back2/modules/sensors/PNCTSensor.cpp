/*  
 *  Description: Definições para controle de encoder do tipo incremental A-B.
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#include "includes/PNCTSensor.h"


/* Função de interrupção para contagem dos pulsos 
 * Para a contagem de pulsos, há dois encoders com
 * defasagem para detecção do sentido de rotação.
 */
void IRAM_ATTR count_pulses( void* arg ) {
  bool vel_state = gpio_get_level(ADC_EXT1);
  bool dir_state = gpio_get_level(ADC_EXT2);
  /* Sentido horário */
  if (vel_state && !dir_state) {
    forward_count++;
    /* Sentido anti-horário */
  } else if (!vel_state && dir_state) {
    backward_count++;
    /* Próximo pulso sem importancia */
  } else {
  }
}

void init_pulses_counter(void) {
  gpio_config_t io_conf = {
    .pin_bit_mask = ((1ULL << ADC_EXT1) | (1ULL << ADC_EXT2)),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_POSEDGE,
  };
  gpio_config(&io_conf);

  // Instala a rotina de tratamento de interrupção
  gpio_install_isr_service(0);
  gpio_isr_handler_add(ADC_EXT1, count_pulses, NULL);
  gpio_isr_handler_add(ADC_EXT2, count_pulses, NULL);
}

/* Para atualizar as variáveis ```pulses``` e ```direction``` 
 * chamar a função compute_pulses
 */
uint32_t compute_pulses(void) {
  pulses = (backward_count + forward_count);
  if ( backward_count < forward_count )  direction = FORWARD;
  else if (pulses == 0 )                 direction = STOPPED;
  else                                   direction = BACKWARD;
  // Reinicia as contagens para a próxima iteração
  backward_count = 0;
  forward_count = 0;

  // Calcula a diferença de tempo entre as medições
  pnct_dt_last_measure = (micros() - pnct_time_last_measure);
  pnct_time_last_measure = micros();

  return pulses;
}

int32_t get_pulses( void ){
  return pulses;
}

int32_t get_direction( void ){
  return direction;
}
