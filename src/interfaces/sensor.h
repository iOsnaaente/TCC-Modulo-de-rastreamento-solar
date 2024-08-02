/**
  * @file sensor.h
  * @brief Classe de interface para sensores de posição
  * @details Este arquivo contém a definições e protótipos da classe Interface de sensores.
  * @note A classe Sensor é uma interface e deve ter seus métodos sobreescritos pela classe que definira as 
  * os métodos corretos de Sensor, únicos para cada tipo de sensor utilizado.
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 22 de julho de 2024
  */

#ifndef SENSORS_H_
#define SENSORS_H_

#include "../../board_config.h"

#include "stdio.h"
#include "stdlib.h"

#include "esp_err.h" 

#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/i2c.h"

/* Enumeradores de módulo*/
enum SensorType_t {
  SENSOR_AS5600_I2C,
  SENSOR_AS5600_ANALOG,
  SENSOR_AS5600_PWM,
  SENSOR_ANALOG,
  SENSOR_TACOMETER,
};


class Sensor {
protected:
    const char* description;
public:
    /* Atributos da classe */
    int32_t raw_position;
    double scaled_position; 
    /* Construtor da classe */
    Sensor( const char* description )
        : description(description) {};
    Sensor( void ){};
    /* Métodos da classe */
    virtual int32_t read_raw( void ) = 0;
    virtual double read_scaled( void ) = 0;
    virtual uint8_t get_status( void ) = 0;
};


#endif 