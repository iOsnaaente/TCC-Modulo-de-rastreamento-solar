/**
  * @file motorController.h
  * @brief Classe de Controle de posição do Motor utilziando os sensores adequados 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */

#ifndef POSITIONCONTROLLER_H
#define POSITIONCONTROLLER_H

#include "../modules/motors/L298N.h"
#include "../modules/sensors/analog.h" 


class PositionController {
private:
  /* Atuador */
  L298N motor;
  /* Sensor */
  AnalogSensor sensor;
  /* Setpoint */
  double tolerance;
  double setpoint;
  /* velocidade de saída */
  double out_value  = 0;
  /* Erros */
  double error      = 0;
  double prev_error = 0;
  double int_error  = 0;
  double dif_error  = 0;
  

public:
    // Construtor
    PositionController(const char* description, uint8_t sensor_type );
    // Configurar posição alvo
    void set_target_position( double target );
    // Iniciar o controle
    void start_control();
    // Parar o motor
    void stop();
    // Atualizar o controle (chamar periodicamente)
    void update_control();
};

#endif // POSITIONCONTROLLER_H
