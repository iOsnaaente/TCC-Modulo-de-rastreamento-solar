/**
  * @file Motor.h
  * @brief Classe de Controle do Motor para Robô
  * @details Este arquivo contém a definições e protótipos da classe Motor, que é responsável por
  * controlar os movimentos das rodas do robô. A classe fornece métodos para
  * configurar a velocidade e direção dos motores, permitindo ao robô se mover
  * para frente, para trás, girar, parar e ajustar sua velocidade de forma
  * dinâmica.
  *   PARAMETROS:
  *   const char* description;
  *   uint8_t ID;
  *   METODOS:
  *   + Motor(uint8_t motorID, const char* description)
  *   - void setDirection(uint8_t direction
  *   - void setSpeed(uint8_t speed)
  *   - uint8_t getSpeed(void)
  *   - uint8_t getDirection(void)
  *   - bool ping(void)
  * @note A classe Motor é uma interface e deve ter seus métodos sobreescritos pela classe que definira as 
  * os métodos corretos de Motor, únicos para cada tipo de motor usado.
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 6 de abril de 2024
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


// Prototipo de classe do tipo Interface para os motores
// Aqui serão definidas os principais métodos de movimentação dos motores
class Motor {
protected:
    const char* description;
public:
    Motor( const char* description )
        : description(description) {}
    Motor ( void ) {} 

    virtual static inline void set_torque(void) = 0;
    virtual uint8_t set_direction( bool direction) = 0;
    virtual void set_speed( double speed) = 0;
    virtual uint8_t stop(void) = 0;
    virtual uint8_t brake(void) = 0;
};

#endif