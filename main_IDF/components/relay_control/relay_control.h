/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#ifndef RELAY_CONTROL_H_
#define RELAY_CONTROL_H_ 

#include "driver/gpio.h"

/* GPIO de controle do Relé de potência  */
#define POWER_RELAY     GPIO_NUM_32

/* Inicializar as configurações do relé, configurando o pino GPIO associado ao relé */
void init_relay_configurations(void);

/* Ligar o relé, configurando o pino para nível alto */
void turn_relay_on(void);

/* Desligar o relé, configurando o pino para nível baixo */
void turn_relay_off(void);

/* Definir o estado do relé (ligado/desligado) */
void set_relay_state(bool state);

/* Obter o estado atual do relé (0 para desligado, 1 para ligado) */
bool get_relay_state(void);

#endif 