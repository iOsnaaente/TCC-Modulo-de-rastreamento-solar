/*  
 *  Description: Definições para controle de encoder do tipo incremental A-B.
 *  Author: Bruno G. F. Sampaio
 *  Date: 22/07/2024
 *  License: MIT
 *
*/

#include <stdint.h> 

#define  FORWARD   1 
#define  STOPPED   0
#define  BACKWARD -1 

volatile int8_t   direction = STOPPED;
volatile uint32_t forward_count = 0;
volatile uint32_t backward_count = 0;
volatile uint32_t pulses = 0;

// Armazena o tempo da ultima medição
volatile uint32_t pnct_time_last_measure = 0;
volatile uint32_t pnct_dt_last_measure = 0;


/* Função de interrupção para contagem dos pulsos 
 * Para a contagem de pulsos, há dois encoders com
 * defasagem para detecção do sentido de rotação.
 */
void IRAM_ATTR count_pulses( void* arg );

void init_pulses_counter(void);


/* Para atualizar as variáveis ```pulses``` e ```direction``` 
 * chamar a função compute_pulses
 */
uint32_t compute_pulses(void);

int32_t get_pulses( void );

int32_t get_direction( void );