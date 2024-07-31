/*  
 *  Description: Definições para controle de relé 5V (RAS-0510).
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *  DATASHEET available in https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf
 *
*/

#include "../board_config.h"
#include "esp_err.h"

#ifndef __RELAY_H_ 
#define __RELAY_H_ 

class Relay {
private:
    const char *description;
public:
    bool state;
    /* Construtor: Inicializar as configurações do relé, configurando o pino GPIO associado ao relé */
    Relay( const char *description );
    /* Ligar o relé, configurando o pino para nível alto */
    esp_err_t turn_on(void);
    /* Desligar o relé, configurando o pino para nível baixo */
    esp_err_t turn_off(void);
    /* Definir o estado do relé (ligado/desligado) */
    esp_err_t set_state(bool state);
    /* Obter o estado atual do relé (0 para desligado, 1 para ligado) */
    bool get_state(void);
};

#endif /* __RELAY_H_ */