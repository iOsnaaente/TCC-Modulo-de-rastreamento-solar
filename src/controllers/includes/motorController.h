/**
  * @file motorController.h
  * @brief Classe de Controle de posição do Motor utilziando os sensores adequados 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */

#ifndef POSITIONCONTROLLER_H
#define POSITIONCONTROLLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_err.h"

/**  Interfaces */ 
#include "../../interfaces/motor.h"
#include "../../interfaces/sensor.h"
#include "../../interfaces/RTC.h"

/**  Módulos */
#include "../../modules/sensors/analog.h" // Sensores 
#include "../../modules/sensors/AS5600.h"
#include "../../modules/sensors/incremental.h"
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
public:
  const char *description;
  uint8_t status;


  // Instancia da classe this
  static MotorController *Instance;
  TaskHandle_t *stepControllTask;
  SemaphoreHandle_t xMotorControllerSemaphore;

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

  // 
  double prev_angle = 0;
  double position = 0;

  /* Erros */
  double prev_error = 0;
  double int_error  = 0;
  double dif_error  = 0;

  /* Ganhos */
  const uint8_t measurement_time = 5;
  uint64_t last_measurement = 0; 
  double Kp = 10.0;
  double Ki = 2.10;
  double Kd = 0.10;

  void step( void * pvParameters );
  
    double error      = 0;
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
};

#endif // POSITIONCONTROLLER_H
