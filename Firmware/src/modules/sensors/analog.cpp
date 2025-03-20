#include "analog.h"

AnalogSensor::AnalogSensor(const char* description, uint8_t num_adc) : Sensor(description) {
  // Define a resolução do ADC para 12 bits
  adc1_config_width(ADC_WIDTH_BIT_12); 
  if ( num_adc == 36 ){
    // Configura o GPIO 36 (canal 0 do ADC1)
    adc1_config_channel_atten(ADC_EXT1_CHANNEL, ADC_ATTEN_DB_12); 
  }else if ( num_adc == 39 ){ 
    // Configura o GPIO 39 (canal 3 do ADC1)
    adc1_config_channel_atten(ADC_EXT2_CHANNEL, ADC_ATTEN_DB_12); 
  }else if ( num_adc == 34 ){ 
    // Configura o GPIO 36 (canal 6 do ADC1)
    adc1_config_channel_atten(ADC_EXT3_CHANNEL, ADC_ATTEN_DB_12); 
  }else{
    // Configura o GPIO 36 como padrão (canal 0 do ADC1)
    adc1_config_channel_atten(ADC_EXT1_CHANNEL, ADC_ATTEN_DB_12); 
  }
}


/*
 * Lê o valor bruto do ADC (0 - 4095) do pino GPIO 36.
 * Retorna o valor bruto lido.
 */
int32_t AnalogSensor::read_raw() {
    this->raw_position = adc1_get_raw(ADC_EXT1_CHANNEL);
    return this->raw_position;
}


/*
 * Lê o valor bruto do ADC e converte para um ângulo escalado (0-360 graus).
 * Retorna o ângulo escalado.
 */
double AnalogSensor::read_scaled() {
    this->scaled_position = (double)(read_raw()/4095.0)*360.0;
    return this->scaled_position;
}


uint8_t AnalogSensor::get_status(){
  return (uint8_t)true;
}
