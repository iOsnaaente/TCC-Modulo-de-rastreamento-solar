/**
  * @file RTC.h
  * @brief Classe de interface para os relógios de tempo real
  * @details Este arquivo contém a definições e protótipos da classe Interface de Real Time Clocks.
  * @note A classe RTC é uma interface e deve ter seus métodos sobreescritos pela classe que definira as 
  * os métodos corretos de RTC, únicos para cada tipo de sensor utilizado.
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 22 de julho de 2024
  */

#ifndef RTC_H_
#define RTC_H_

#include "../board_config.h"

#include "stdio.h"
#include "stdlib.h"

#include "esp_timer.h"
#include "time.h"

#include "driver/i2c.h"

enum RTC_type_t {
  RTC_DS3231,
  RTC_SOFT,
};

// Datetime do sistema
struct datetime_buffer_t {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  double second;
  bool dt_sync;
};

class RTC {
protected:
    const char* description;
    datetime_buffer_t datetime;
public:
    RTC( const char* description ) : description(description) {}
    virtual esp_err_t  set_datetime( datetime_buffer_t datetime ) = 0;
    virtual esp_err_t  get_datetime( datetime_buffer_t &datetime ) = 0;
};

#endif