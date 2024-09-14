#include "ds3231.h"

DS3231::DS3231( const char* description, datetime_buffer_t datetime = {0} )
    : RTC(description)  {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = BOARD_SDA;
    conf.scl_io_num = BOARD_SCL;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    esp_err_t ret = i2c_param_config(DS3231_I2C_PORT, &conf);
    i2c_driver_install(DS3231_I2C_PORT, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if ( datetime.year != 0 ){
        this->set_datetime( datetime );
    }
}

esp_err_t DS3231::write_byte( uint8_t reg_addr, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, DS3231_I2C_ADDRESS << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(DS3231_I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t DS3231::read_bytes( uint8_t reg_addr, uint8_t *data, size_t length) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, DS3231_I2C_ADDRESS << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, DS3231_I2C_ADDRESS << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, length - 1, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, data + length - 1, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(DS3231_I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


esp_err_t DS3231::set_datetime( datetime_buffer_t datetime ) {
    uint8_t data[7] = {0};
    data[0] = 0x00;  
    data[1] = (( (int)(datetime.second) / 10) << 4) | ( (int)(datetime.second) % 10);
    data[2] = (( datetime.minute / 10) << 4) | ( datetime.minute % 10);
    data[3] = (( datetime.hour   / 10) << 4) | ( datetime.hour % 10);
    data[4] = (( datetime.day    / 10) << 4) | ( datetime.day % 10);
    data[6] = (( datetime.month  / 10) << 4) | ( datetime.month % 10);
    data[7] = (( datetime.year   / 10) << 4) | ( datetime.year % 10);
    return this->write_byte( DS3231_REG_TIME    , data[0]) ||
           this->write_byte( DS3231_REG_TIME + 1, data[1]) ||
           this->write_byte( DS3231_REG_TIME + 2, data[2]) ||
           this->write_byte( DS3231_REG_TIME + 3, data[3]) ||
           this->write_byte( DS3231_REG_TIME + 4, data[4]) ||
           this->write_byte( DS3231_REG_TIME + 5, data[5]) ||
           this->write_byte( DS3231_REG_TIME + 6, data[6]) ||
           this->write_byte( DS3231_REG_TIME + 7, data[7]);
}

esp_err_t DS3231::get_datetime( datetime_buffer_t &datetime) {
    uint8_t data[7] = {0};
    esp_err_t ret = this->read_bytes( DS3231_REG_TIME, data, 7 );
    if (ret != ESP_OK) return ret;
    datetime.second = ((data[0] >> 4) * 10) + (data[0] & 0x0F);
    datetime.minute = ((data[1] >> 4) * 10) + (data[1] & 0x0F);
    datetime.hour = ((data[2] >> 4) * 10) + (data[2] & 0x0F);
    datetime.day = ((data[3] >> 4) * 10) + (data[3] & 0x0F);
    // date = ((data[4] >> 4) * 10) + (data[4] & 0x0F);
    datetime.month = ((data[5] >> 4) * 10) + (data[5] & 0x0F);
    datetime.year = ((data[6] >> 4) * 10) + (data[6] & 0x0F);
    return ESP_OK;
}

esp_err_t DS3231::get_temperature( float *temperature) {
    uint8_t data[2];
    esp_err_t ret = this->read_bytes( DS3231_REG_TEMP, data, 2);
    if (ret != ESP_OK) return ret;
    int16_t temp_raw = (data[0] << 8) | data[1];
    *temperature = (float)temp_raw / 256.0;
    return ESP_OK;
}
