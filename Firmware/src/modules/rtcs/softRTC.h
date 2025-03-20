#ifndef RTC_TIMER_H
#define RTC_TIMER_H

#include "../../controllers/includes/serialController.h"

#include "../../interfaces/RTC.h"

#include "driver/rtc_io.h"

#include "esp_system.h"
#include "esp_sntp.h"
#include "esp_log.h"

#include "soc/rtc.h"

#include "ctime"


#define SECONDS_IN_A_DAY 86400 // 24*60*60 


class SoftRTC : public RTC {
public:
    uint64_t last_update_time;
    const uint8_t days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    SoftRTC(const char* description, datetime_buffer_t datetime ) :
    RTC(description), last_update_time(0) {
        set_datetime( datetime );
    };
    esp_err_t  set_datetime( datetime_buffer_t datetime ) override ;
    esp_err_t  get_datetime( datetime_buffer_t &datetime ) override ;
    esp_err_t  update(void);
};

#endif // RTC_TIMER_H
