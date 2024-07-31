
/*  
 *  Description: Atualização do Real Time Clock do ESP para sincronia da hora com o sistema.
 *  Author: Bruno G. F. Sampaio
 *  Date: 09/01/2024
 *  License: MIT
 *
*/

time_t convert_datetime_to_tv_sec(struct datetime_buffer_t datetime) {
  struct tm timeinfo;
  timeinfo.tm_year = datetime.year - 1900;
  timeinfo.tm_mon = datetime.month - 1;
  timeinfo.tm_mday = datetime.day;
  timeinfo.tm_hour = datetime.hour;
  timeinfo.tm_min = datetime.minute;
  timeinfo.tm_sec = datetime.second;
  return mktime(&timeinfo);
}

esp_err_t init_rtc(struct datetime_buffer_t datetime) {
  struct timeval initial_time = {
    .tv_sec = 0,
    .tv_usec = 0
  };
  struct tm timeinfo;
  timeinfo.tm_year = datetime.year + 100;
  timeinfo.tm_mon = datetime.month - 1;
  timeinfo.tm_mday = datetime.day;
  timeinfo.tm_hour = datetime.hour;
  timeinfo.tm_min = datetime.minute;
  timeinfo.tm_sec = datetime.second;
  initial_time.tv_sec = mktime(&timeinfo);
  esp_err_t ret = settimeofday(&initial_time, NULL);
  return ret;
}

void datetime_update_from_rtc(struct datetime_buffer_t *datetime) {
  struct timeval now;
  gettimeofday(&now, NULL);
  struct tm timeinfo;
  localtime_r(&now.tv_sec, &timeinfo);
  datetime->year = timeinfo.tm_year + 1900;
  datetime->month = timeinfo.tm_mon + 1;
  datetime->day = timeinfo.tm_mday;
  datetime->hour = timeinfo.tm_hour;
  datetime->minute = timeinfo.tm_min;
  datetime->second = timeinfo.tm_sec;
}