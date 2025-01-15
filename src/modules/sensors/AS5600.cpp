/**
  * @file As5600.cpp
  * @brief Classe de uso do sensor AS5600 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */

#include "AS5600.h"

AS5600::AS5600( const char* description, uint8_t mode ) 
  : Sensor( description ), mode( mode ) {
    
  // Configurações básicas do I2C
  if ( this->mode == AS5600_MODE_I2C ){
    i2c_config_t i2c_channel_config = {};
    i2c_channel_config.mode = I2C_MODE_MASTER;
    i2c_channel_config.sda_io_num = BOARD_SDA;
    i2c_channel_config.scl_io_num = BOARD_SCL;
    i2c_channel_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_channel_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_channel_config.master.clk_speed = 1000000; // 1000 kHz 
    i2c_param_config(I2C_NUM_0, &i2c_channel_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, I2C_RX_BUFFER, I2C_TX_BUFFER, 0);
    uint8_t data_config[8] = { 0 };
    write_register_data(AS5600_ADDR_ZPOS, sizeof(data_config), data_config);

  } else if ( this->mode == AS5600_MODE_ANALOG ) { 
  } else if ( this->mode == AS5600_MODE_PWM ) { 
  } else {
  }
}

/*
 * Lê os registradores do escravo começando pelo uint8_t init_addr com tamanho size_t data_len.
 * Salva os valores em uint8_t *data_buffer. 
 * Retorna o status esp_err_t da operação:
 *  ESP_OK se bem sucedido
 *
 * Exemplo de leitura do registro de ângulo (endereço 0x0E) do AS5600
 *  > uint8_t angle_data[2]; // O ângulo é representado por 2 bytes
 *  > esp_err_t ret = read_register_data( ADDR_ANGLE, sizeof(angle_data), angle_data );
 *  > if (ret == ESP_OK) {
 *  >     uint16_t angle = (angle_data[0] << 8) | angle_data[1];
 *  >     Serial.println("Ângulo lido: %d\n", angle);
 *  > }
 */
esp_err_t AS5600::read_register_data(uint8_t init_addr, size_t data_len, uint8_t *data_buffer) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AS5600_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, init_addr, true);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AS5600_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
  for (size_t i = 0; i < data_len - 1; i++)
    i2c_master_read_byte(cmd, &data_buffer[i], I2C_MASTER_ACK);
  i2c_master_read_byte(cmd, &data_buffer[data_len - 1], I2C_MASTER_NACK);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_BUS_TIMEOUT_MS );
  if (ret != ESP_OK)
    return ret;
  i2c_cmd_link_delete(cmd);
  return ret;
}

/*
 * Escreve nos registradores do escravo começando pelo uint8_t init_addr com tamanho size_t data_len.
 * Os valores para escrita estão dentro do uint8_t *data_buffer. 
 * Retorna o status esp_err_t da operação:
 *  ESP_OK se bem sucedido
 *
 * Exemplo de escrita de dados (0xAA e 0xBB) no registrador 0x0F do AS5600:
 *  > uint8_t data_to_write[2] = {0xAA, 0xBB};
 *  > esp_err_t ret = write_register_data(0x0F, sizeof(data_to_write), data_to_write);
 *  > if (ret == ESP_OK) {
 *  >     Serial.println("Dados escritos com sucesso no registrador!\n");
 *  > }
 */
esp_err_t AS5600::write_register_data(uint8_t register_addr, size_t data_len, const uint8_t *data_buffer) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AS5600_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, register_addr, true);
  for (size_t i = 0; i < data_len; i++) 
    i2c_master_write_byte(cmd, data_buffer[i], true);
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_BUS_TIMEOUT_MS );
  if (ret != ESP_OK)
    return ret;
  i2c_cmd_link_delete(cmd);
  return ret;
}


// read the unscaled angle and unmodified angle
int32_t AS5600::read_raw(void) {
  if (this->mode == AS5600_MODE_I2C ){
    uint8_t raw_angle_data[2];
    esp_err_t ret = this->read_register_data(AS5600_ADDR_RAW_ANGLE, sizeof(raw_angle_data), raw_angle_data);
    if (ret == ESP_OK) {
      this->raw_position = (raw_angle_data[0] << 8) | raw_angle_data[1];
      return this->raw_position;
    }
    return -1;
  } else if ( this->mode == AS5600_MODE_ANALOG ){
    return -1;
  } else if ( this->mode == AS5600_MODE_PWM ){
    return -1;
  }else {
    return -1;
  }
}

// Read the scaled angle
double AS5600::read_scaled(void) {
  this->scaled_position = (((double)read_raw() / AS5600_RESOLUTION) * 360.0);
  return this->scaled_position;
}


/** Verify the status of the magnetic range
  * The status be in the 0x0B address in the 5:3 bits
  * MH [3] AGC minimum gain overflow, magnet too strong
  * ML [4] AGC maximum gain overflow, magnet too weak
  * MD [5] Magnet was detected
  * to operate properly, the MD have to be set and the MH and ML have to be 0
  */
uint8_t AS5600::get_status(void) {
  uint8_t magnetic_status;
  esp_err_t ret = this->read_register_data(AS5600_ADDR_STATUS, 1, &magnetic_status);
  if (ret == ESP_OK) {
    if ((magnetic_status >> 5) & 0x01)      return  AS5600_MD_STATUS;
    else if ((magnetic_status >> 4) & 0x01) return  AS5600_ML_STATUS;
    else if ((magnetic_status >> 3) & 0x01) return  AS5600_MH_STATUS;
    else return AS5600_ERROR_STATUS;
  }else{
    return AS5600_ERROR_STATUS;
  }
}
