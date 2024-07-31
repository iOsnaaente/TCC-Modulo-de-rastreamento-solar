
#ifndef RTC_TIMER_H
#define RTC_TIMER_H

#include "../../interfaces/RTC.h"

class SoftRTC : public RTC {
public:
    uint64_t last_update_time;
    const uint8_t days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    SoftRTC(const char* description, uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second );

    esp_err_t  set_datetime(uint16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second) override;
    esp_err_t  get_datetime(uint16_t &year, uint16_t &month, uint16_t &day, uint16_t &hour, uint16_t &minute, uint16_t &second) override;
    esp_err_t  update(void) override;
};

#endif // RTC_TIMER_H
