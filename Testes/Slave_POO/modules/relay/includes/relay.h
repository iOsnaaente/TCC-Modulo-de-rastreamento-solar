/*  
 *  Description: Definições para controle de relé 5V (RAS-0510).
 *  Author: Bruno G. F. Sampaio
 *  Date: 22/07/2024
 *  License: MIT
 *  DATASHEET available in https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf
 *
*/

#include "stdint.h"
#include "driver/gpio.h"

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