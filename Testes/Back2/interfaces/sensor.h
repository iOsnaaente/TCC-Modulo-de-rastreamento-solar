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

#include "../board_config.h"

#include "stdio.h"
#include "stdlib.h"

#include "driver/gpio.h"
#include "driver/adc.h"


class Sensor {
protected:
    const char* description;
public:
    Sensor( const char* description )
        : description(description) {};
    Sensor( void ){};

    virtual int32_t read_raw() = 0;
    
    virtual double read_scaled() = 0;
};


#endif 