#include "softRTC.h"

// Define a data e hora
esp_err_t SoftRTC::set_datetime( datetime_buffer_t datetime ) {
    // Validação básica dos parâmetros
    if (datetime.month < 1 || datetime.month > 12 || datetime.day < 1 || datetime.day > 31 || 
        datetime.hour > 23 || datetime.minute > 59 || datetime.second > 59) {
        return ESP_ERR_INVALID_ARG;
    }
    // Calcula o tempo em segundos desde 1970-01-01
    struct tm timeinfo = {0};
    timeinfo.tm_year = datetime.year - 1900;
    timeinfo.tm_mon = datetime.month - 1;
    timeinfo.tm_mday = datetime.day;
    timeinfo.tm_hour = datetime.hour;
    timeinfo.tm_min = datetime.minute;
    timeinfo.tm_sec = datetime.second;
    // Faz a transformação 
    time_t raw_time = mktime(&timeinfo);
    if (raw_time == (time_t)-1) {
        return ESP_ERR_INVALID_ARG;
    }
    // Armazenar o tempo em segundos
    last_update_time = static_cast<uint64_t>(raw_time);
    return ESP_OK;
}


// Obtém a data e hora atual
esp_err_t SoftRTC::get_datetime(datetime_buffer_t &datetime) {
    if (last_update_time == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    time_t raw_time = static_cast<time_t>(last_update_time);
    struct tm *timeinfo = localtime(&raw_time);
    datetime.year = timeinfo->tm_year + 1900;
    datetime.month = timeinfo->tm_mon + 1;
    datetime.day = timeinfo->tm_mday;
    datetime.hour = timeinfo->tm_hour;
    datetime.minute = timeinfo->tm_min;
    datetime.second = timeinfo->tm_sec;
    return ESP_OK;
}


// Atualiza o relógio - Não é precisso 
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