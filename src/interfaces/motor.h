/**
  * @file Motor.h
  * @brief Classe de Controle do Motor para Robô
  * @details Este arquivo contém a definições e protótipos da classe Motor, que é responsável por
  * controlar os movimentos do motor DC.
  * @note A classe Motor é uma interface e deve ter seus métodos sobreescritos pela classe que definira as 
  * os métodos corretos de Motor, únicos para cada tipo de motor usado.
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 22 de julho de 2024
  */


#ifndef MOTOR_H_
#define MOTOR_H_

#include "../board_config.h"

#include "stdio.h"
#include "stdlib.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
  
/* Constantes para estados on-off */
#define BDC_ENABLE        ((bool)(true))         //
#define BDC_DISABLE       ((bool)(false))        //

/* Constantes para definir o estado de mocimento do motor */
#define BDC_STOPED        ((uint8_t)(0))         //
#define BDC_BREAK         ((uint8_t)(1))         //
#define BDC_FORWARD       ((uint8_t)(2))         //
#define BDC_BACKWARD      ((uint8_t)(3))         //

enum MotorType_t {
  MOTOR_DC,
  MOTOR_STEPPER,
  MOTOR_SERVO,
  MOTOR_BLDC
};


class Motor {
public:    
    /* Atributos da classe */
    const char* description;
    bool direction;
    double speed;
    /* Construtor da classe */
    Motor( const char* description, uint8_t direction, double speed )
        : description(description), direction(direction), speed(speed) {}
    /* Métodos da classe */
    virtual uint8_t set_torque( bool torque ) = 0;
    virtual uint8_t set_direction( uint8_t direction) = 0;
    virtual uint8_t set_speed( double speed) = 0;
    virtual uint8_t stop(void) = 0;
    virtual uint8_t brake(void) = 0;
};

#endif