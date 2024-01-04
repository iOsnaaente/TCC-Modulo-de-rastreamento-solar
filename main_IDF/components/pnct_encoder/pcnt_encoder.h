/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#ifndef PCNT_ENCODER_H_H 
#define PCNT_ENCODER_H_H  

#include "driver/gpio.h"

#define  FORWARD   1 
#define  STOPPED   0
#define  BACKWARD -1 


/* ADC com soquete externo - Sensores */
#define ADC_EXT1          GPIO_NUM_36
#define ADC_EXT2          GPIO_NUM_39
#define ADC_EXT3          GPIO_NUM_34

/* Função de interrupção para contagem dos pulsos 
 * Para a contagem de pulsos, há dois encoders com
 * defasagem para detecção do sentido de rotação.
 */
void count_pulses( void* arg );

void init_pulses_counter(void);

/* Para atualizar as variáveis ```pulses``` e ```direction``` 
 * chamar a função compute_pulses
 */
uint32_t compute_pulses(void);

int32_t get_pulses( void );
int32_t get_direction( void );

#endif 