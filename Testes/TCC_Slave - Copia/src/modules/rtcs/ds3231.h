#ifndef DS3231_RTC_H
#define DS3231_RTC_H

#include "../../interfaces/RTC.h"


// Configuração do I2C
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

// Endereço I2C do DS3231
#define DS3231_I2C_PORT     (i2c_port_t)I2C_NUM_0
#define DS3231_I2C_ADDRESS  0x68

// Endereços dos registradores do DS3231
#define DS3231_REG_TIME     0x00
#define DS3231_REG_DATE     0x04
#define DS3231_REG_CONTROL  0x0E
#define DS3231_REG_TEMP     0x11


class DS3231 : public RTC {
private:
    esp_err_t read_bytes( uint8_t reg_addr, uint8_t *data, size_t length);
    esp_err_t write_byte( uint8_t reg_addr, uint8_t data );
    esp_err_t get_temperature( float *temperature);
public:
    double temperature;
    DS3231(const char* description, uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second);
    esp_err_t  set_datetime(uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second) override;
    esp_err_t  get_datetime(uint16_t &year, uint16_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute, uint16_t &second) override;
    esp_err_t  update(void) override;
};


#endif // DS3231_RTC_H
