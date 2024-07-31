/*  
 *  Description: Definições para uso de um potenciometro como sensor de posição angular analógico.
 *  Author: Bruno G. F. Sampaio
 *  Date: 19/07/2024
 *  License: MIT
 *
*/


// Importe da interface I2C
#include "driver/adc.h"
#include "pinout.h"

/*
 * Inicializa o encoder analógico.
 * Retorna o status esp_err_t da operação:
 *  ESP_OK se bem sucedido
 *
 * Exemplo de inicialização do encoder analógico:
 *  > esp_err_t ret = init_analog_encoder();
 *  > if (ret == ESP_OK) {
 *  >     printf("Encoder analógico inicializado com sucesso!\n");
 *  > }
 */
esp_err_t init_analog_encoder(void) {
  // Configura o GPIO 36 (canal 0 do ADC1)
  adc1_config_width(ADC_WIDTH_BIT_12); // Define a resolução do ADC para 12 bits
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // Configura a atenuação do canal para 11dB
  return ESP_OK;
}


/*
 * Lê o valor bruto do ADC (0 - 4095) do pino GPIO 36.
 * Retorna o valor bruto lido.
 */
int16_t read_raw_angle(void) {
    return adc1_get_raw(ADC1_CHANNEL_0);
}

/*
 * Lê o valor bruto do ADC e converte para um ângulo escalado (0-360 graus).
 * Retorna o ângulo escalado.
 */
double read_angle(void) {
    return (double)(read_raw_angle()/4095.0)*360.0;
}

