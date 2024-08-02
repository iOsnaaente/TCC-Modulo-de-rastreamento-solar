/*  
 *  Description: Definições para controle de relé 5V (RAS-0510).
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *  DATASHEET available in https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf
 *
*/

#include "relay.h"

/* Construtor: Inicializar as configurações do relé, configurando o pino GPIO associado ao relé */
Relay::Relay( const char *description ) : description(description){
  gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << POWER_RELAY), /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
    .mode = GPIO_MODE_OUTPUT,              /*!< GPIO mode: set input/output mode                     */
    .pull_up_en = GPIO_PULLUP_DISABLE,     /*!< GPIO pull-up                                         */
    .pull_down_en = GPIO_PULLDOWN_DISABLE, /*!< GPIO pull-down                                       */
    .intr_type = GPIO_INTR_DISABLE,        /*!< GPIO interrupt type                                  */
  };
  gpio_config(&io_conf);
  gpio_set_level(POWER_RELAY, false);
}

/* Ligar o relé, configurando o pino para nível alto */
esp_err_t Relay::turn_on(void){
  return gpio_set_level(POWER_RELAY, true);
}

/* Desligar o relé, configurando o pino para nível baixo */
esp_err_t Relay::turn_off(void){
  return gpio_set_level(POWER_RELAY, false);
}

/* Definir o estado do relé (ligado/desligado) */
esp_err_t Relay::set_state(bool state){
  return gpio_set_level(POWER_RELAY, state);
}

/* Obter o estado atual do relé (0 para desligado, 1 para ligado) */
bool Relay::get_state(void){
  return gpio_get_level(POWER_RELAY);
}