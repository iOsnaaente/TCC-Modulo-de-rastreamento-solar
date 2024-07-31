#include "softRTC.h"

SoftRTC::SoftRTC(const char* description, uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second) : RTC(description), last_update_time(0) {
    this->set_datetime( year, month, day, hour, minute, second );
}

esp_err_t  SoftRTC::set_datetime(uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second) {
    this->datetime.year = year;
    this->datetime.month = month;
    this->datetime.day = day;
    this->datetime.hour = hour;
    this->datetime.minute = minute;
    this->datetime.second = second;
    this->last_update_time = esp_timer_get_time()/1000;
    return ESP_OK;
}

esp_err_t  SoftRTC::get_datetime(uint16_t &year, uint16_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute, uint16_t &second) {
    update(); 
    year = this->datetime.year;
    month = this->datetime.month;
    day = this->datetime.day;
    hour = this->datetime.hour;
    minute = this->datetime.minute;
    second = this->datetime.second;
    return ESP_OK;
}

esp_err_t  SoftRTC::update(void) {
    unsigned long current_time = esp_timer_get_time()/1000;
    unsigned long elapsed_time = current_time - last_update_time;
    if (elapsed_time >= 1000) { // Atualiza a cada segundo
        uint16_t increment = elapsed_time / 1000;
        this->datetime.second+=increment;
        if (this->datetime.second >= 60) {
            this->datetime.second = 0;
            this->datetime.minute++;
            if (this->datetime.minute >= 60) {
                this->datetime.minute = 0;
                this->datetime.hour++;
                if (this->datetime.hour >= 24) {
                    this->datetime.hour = 0;
                    this->datetime.day++;
                    if (this->datetime.day > this->days_in_month[this->datetime.month - 1]) {
                        this->datetime.day = 1;
                        this->datetime.month++;
                        if (this->datetime.month > 12) {
                            this->datetime.month = 1;
                            this->datetime.year++;
                        }
                    }
                }
            }
        }
        last_update_time += increment * 1000;
    }
    return ESP_OK;
}