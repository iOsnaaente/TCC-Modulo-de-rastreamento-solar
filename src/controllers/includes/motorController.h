/**
  * @file motorController.h
  * @brief Classe de Controle de posição do Motor utilziando os sensores adequados 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */

#ifndef POSITIONCONTROLLER_H
#define POSITIONCONTROLLER_H

#include "esp_err.h"


/**  Interfaces */ 
#include "../../interfaces/motor.h"
#include "../../interfaces/sensor.h"
#include "../../interfaces/RTC.h"

/**  Módulos */
#include "../../modules/sensors/analog.h" // Sensores 
#include "../../modules/sensors/AS5600.h"
#include "../../modules/motors/L298N.h"   // Motores 
#include "../../modules/rtcs/softRTC.h"   // RTCs 
#include "../../modules/rtcs/ds3231.h"
#include "../../modules/spa/spa.h"        // SPA 

/**  Controllers */ 
#include "serialController.h"


#define CONTROLLER_OFF  0
#define CONTROLLER_ON   1 


class MotorController {
private:
  const char *description;
  uint8_t status;

  /* Atuador */
  MotorType_t motor_type;
  Motor *motor;

  /* Sensor */
  SensorType_t sensor_type;
  Sensor *sensor;

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

  /* Ganhos */
  uint64_t last_measurement = 0; 
  double Kp = 0;
  double Ki = 0;
  double Kd = 0;
  

public:
    // Construtor
    MotorController(const char* description, SensorType_t sensor_type, MotorType_t motor_type  );
    // seta os valores de PID
    esp_err_t set_pid_gains( double Kp, double Ki, double Kd );
    // Configurar posição alvo
    esp_err_t set_target( double target );
    // Iniciar o controle
    esp_err_t start();
    // Parar o motor
    esp_err_t stop();
    // Atualizar os sensores e saida do atuador
    esp_err_t update();
    // Aplica um passo de calculo das variavesid e controle 
    esp_err_t step();
};

#endif // POSITIONCONTROLLER_H