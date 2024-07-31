/*  
 *  Description: Definições para uso do sensor de posição angular através de encoder magnético AS5600.
 *  Author: Bruno G. F. Sampaio
 *  Date: 08/01/2024
 *  License: MIT
 *  DATASHEET available in https://ams.com/documents/20143/36005/AS5600_DS000365_5-00.pdf
 *
*/


// Importe da interface I2C
#include "driver/i2c.h"
#include "pinout.h"



/*  I2C Macro Definitions */
#define I2C_BUS_TIMEOUT_MS 100
#define I2C_TX_BUFFER 0
#define I2C_RX_BUFFER 0

/* AS5600 Macro Definitions */
#define AS5600_ADDRESS 0x36
#define AS5600_RESOLUTION ((double)(4096))

#define AS5600_MD_STATUS -5
#define AS5600_ML_STATUS -4
#define AS5600_MH_STATUS -3
#define AS5600_OK_STATUS 1


// Configuration registers
#define AS5600_ADDR_ZMCO 0x00  // 1 byte  [1:0] number of burn cmd
#define AS5600_ADDR_ZPOS 0x01  // 2 bytes Initial position (OTP memory)
#define AS5600_ADDR_MPOS 0x03  // 2 bytes Stop position (OTP memory)
#define AS5600_ADDR_MANG 0x05  // 2 bytes Maximum angle (OTP memory)

/*
 *  Name      Bit Position      Description
 *  PM(1:0)       1:0           Power Mode
 *                                00 = NOM 
 *                                01 = LPM1 
 *                                10 = LPM2 
 *                                11 = LPM3
 *  HYST(1:0)     3:2           Hysteresis
 *                                00 = OFF
 *                                01 = 1 LSB 
 *                                10 = 2 LSBs
 *                                11 = 3 LSBs
 *  OUTS(1:0)     5:4           Output Stage
 *                                00 = analog (full range from 0% to 100% between GND and VDD, 
 *                                01 = analog (reduced range from 10% to 90% between GND and VDD, 
 *                                10 = digital PWM
 *  PWMF(1:0)     7:6           PWM Frequency
 *                                00 = 115 Hz; 
 *                                01 = 230 Hz; 
 *                                10 = 460 Hz; 
 *                                11 = 920 Hz
 *  SF(1:0)       9:8           Slow Filter
 *                                00 = 16x   2.200 ms with 0.015º of noise
 *                                01 = 8x    1.100 ms with 0.021º of noise
 *                                10 = 4x    0.550 ms with 0.030º of noise
 *                                11 = 2x    0.286 ms with 0.043º of noise
 *  FTH(2:0)      12:10         Fast Filter Threshold
 *                                000 = slow filter only, 
 *                                001 = 6 LSBs, 
 *                                010 = 7 LSBs, 
 *                                011 = 9 LSBs,
 *                                100 = 18 LSBs, 
 *                                101 = 21 LSBs, 
 *                                110 = 24 LSBs, 
 *                                111 = 10 LSBs
 *  WD            13            Watchdog
 *                                0 = OFF
 *                                1 = ON 
 */
#define AS5600_ADDR_CONF 0x07  // 2 bytes of configuration

// Output Registers
#define AS5600_ADDR_RAW_ANGLE 0x0C  // 2 bytes of position
#define AS5600_ADDR_ANGLE 0x0E      // 2 bytes of position

// Status Registers
/*
 * [bit 5] MH AGC minimum gain overflow, magnet too strong
 * [bit 4] ML AGC maximum gain overflow, magnet too weak
 * [bit 3] MD Magnet was detected
 */
#define AS5600_ADDR_STATUS 0x0B  // 1 byte  [5:3]

#define AS5600_ADDR_AGC 0x1A        // 1 byte AGC[7:0]
#define AS5600_ADDR_MAGNITUDE 0x1B  // 2 bytes [11:0]

// Burn Commands
/*
 * Burn angle = 0x80
 * Burn setting = 0x40
 */
#define AS5600_ADDR_BURN 0xFF  // Burn command


/*
 * Inicializa o encoder magnético.
 * Retorna o status esp_err_t da operação:
 *  ESP_OK se bem sucedido
 *
 * Exemplo de inicialização do encoder magnético:
 *  > esp_err_t ret = init_magnetic_encoder();
 *  > if (ret == ESP_OK) {
 *  >     printf("Encoder magnético inicializado com sucesso!\n");
 *  > }
 */
esp_err_t init_magnetic_encoder(void) {
  // Configurações básicas do I2C
  i2c_config_t i2c_channel_config = {};
  i2c_channel_config.mode = I2C_MODE_MASTER;
  i2c_channel_config.sda_io_num = SDA;
  i2c_channel_config.scl_io_num = SCL;
  i2c_channel_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_channel_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_channel_config.master.clk_speed = 100000;
  esp_err_t ret = i2c_param_config(I2C_NUM_0, &i2c_channel_config);
  if (ret != ESP_OK) {
    return ret;
  }
  ret = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, I2C_RX_BUFFER, I2C_TX_BUFFER, 0);
  if (ret != ESP_OK) {
    return ret;
  }
  uint8_t data_config[8] = { 0 };
  ret = write_register_data(AS5600_ADDR_ZPOS, sizeof(data_config), data_config);
  if (ret != ESP_OK) {
    return ret;
  }

  return ESP_OK;
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
esp_err_t read_register_data(uint8_t init_addr, size_t data_len, uint8_t *data_buffer) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AS5600_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, init_addr, true);
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AS5600_ADDRESS << 1) | I2C_MASTER_READ, true);
  for (size_t i = 0; i < data_len - 1; i++) {
    i2c_master_read_byte(cmd, &data_buffer[i], I2C_MASTER_ACK);
  }
  i2c_master_read_byte(cmd, &data_buffer[data_len - 1], I2C_MASTER_NACK);
  i2c_master_stop(cmd);

  esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_BUS_TIMEOUT_MS );
  if (ret != ESP_OK) {
    return ret;
  }
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
esp_err_t write_register_data(uint8_t register_addr, size_t data_len, const uint8_t *data_buffer) {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  i2c_master_write_byte(cmd, (AS5600_ADDRESS << 1) | I2C_MASTER_WRITE, true);
  i2c_master_write_byte(cmd, register_addr, true);
  for (size_t i = 0; i < data_len; i++) {
    i2c_master_write_byte(cmd, data_buffer[i], true);
  }
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_BUS_TIMEOUT_MS );
  if (ret != ESP_OK) {
    return ret;
  }
  i2c_cmd_link_delete(cmd);
  return ret;
}


// read the unscaled angle and unmodified angle
int16_t read_raw_angle(void) {
  uint8_t raw_angle_data[2];
  esp_err_t ret = read_register_data(AS5600_ADDR_RAW_ANGLE, sizeof(raw_angle_data), raw_angle_data);
  if (ret == ESP_OK) {
    uint16_t raw_angle = (raw_angle_data[0] << 8) | raw_angle_data[1];
    return raw_angle;
  }
  return -1;
}

// Read the scaled angle
double read_angle(void) {
  return (((double)read_raw_angle() / AS5600_RESOLUTION) * 360.0);
}


/* Verify the status of the magnetic range
* The status be in the 0x0B address in the 5:3 bits
*
* MH [3] AGC minimum gain overflow, magnet too strong
* ML [4] AGC maximum gain overflow, magnet too weak
* MD [5] Magnet was detected
*
* to operate properly, the MD have to be set and the MH and ML have to be 0
*/
int8_t get_magnetic_status(void) {
  uint8_t magnetic_status;
  esp_err_t ret = read_register_data(AS5600_ADDR_STATUS, 1, &magnetic_status);
  if (ret == ESP_OK) {
    if ((magnetic_status >> 5) & 0x01) return       AS5600_MD_STATUS;
    else if ((magnetic_status >> 4) & 0x01) return  AS5600_ML_STATUS;
    else if ((magnetic_status >> 3) & 0x01) return  AS5600_MH_STATUS;
    else return                                     AS5600_OK_STATUS;
  }else{
    return ret;
  }
}
