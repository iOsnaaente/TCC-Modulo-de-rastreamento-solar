/**
  * @file motorController.h
  * @brief Classe de Controle de posição do Motor utilziando os sensores adequados 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */


#include "includes/motorController.h"


// Inicializa o ponteiro estático
MotorController* MotorController::Instance = nullptr;

double torque_feedforward = 0.5;


MotorController::MotorController(const char* description, SensorType_t sensor_type, MotorType_t motor_type ) 
  : description( description ), sensor_type( sensor_type), motor_type( motor_type ) {
    // Inicializar os componentes do motor e sensor conforme o tipo escolhido
    if ( sensor_type == SENSOR_ANALOG ) {
      sensor = new AnalogSensor( "Sensor Analogico", ADC_EXT1_CHANNEL );
    }else if ( sensor_type == SENSOR_AS5600_I2C ) {
      sensor = new AS5600( "Sensor I2C Digital", AS5600_MODE_I2C );
    }else if ( sensor_type == SENSOR_AS5600_ANALOG ) {
      sensor = new AS5600( "Sensor I2C Analogico", AS5600_MODE_ANALOG );
    }else if ( sensor_type == SENSOR_AS5600_PWM ) {
      sensor = new AS5600( "Sensor I2C PWM", AS5600_MODE_PWM );
    }else if ( sensor_type == SENSOR_TACOMETER ) {
      sensor = new IncrementalSensor( "TACOMETER", BOARD_SCL, BOARD_SDA );
    }else {
      // return ESP_ERR_NOT_FOUND;
    }

    if ( motor_type == MOTOR_DC ){
      motor = new L298N( "BDC Motor Driver L298N", BDC_FORWARD, 0.0 );

    }else if ( motor_type == MOTOR_BLDC ){
      // return ESP_ERR_NOT_FOUND;
    } else if ( motor_type == MOTOR_SERVO ){
      // return ESP_ERR_NOT_FOUND;
    } else if ( motor_type == MOTOR_STEPPER ){
      // return ESP_ERR_NOT_FOUND;
    }

    // Cria a sua instancia para uso das funções da classe 
    Instance = this; 

    // Inicializa a task de medição dos pulsos 
    xTaskCreate( [](void *arg){Instance->step(nullptr);}, "stepControllTask", 1024*8, NULL, 1, stepControllTask );

}



// Aplica um passo de cálculo das variáveis de controle 
void MotorController::step( void *pvParametes ) {  
  while (true) {
    // Ler a posição do motor 
    sensor->read_scaled();

    // Se o motor estiver ligado, continua o controle 
    if ( this->status == CONTROLLER_ON ){

      // Calculo do ângulo acumulado
      double delta_angle = sensor->scaled_position - prev_angle;
      // Corrige o salto de ângulo se necessário (exemplo: -180 a 180 graus)
      if (delta_angle > 180) {
          delta_angle -= 360;
      } else if (delta_angle < -180) {
          delta_angle += 360;
      }
      position += delta_angle;
      prev_angle = sensor->scaled_position;

      // Tempo atual
      unsigned long current_time = esp_timer_get_time();
      double elapsed_time = (current_time - last_measurement) / 1000000.0; 
      last_measurement = current_time;
      
      // Calculo da malha de controle externa (Posição)
      double position_error = setpoint - position;
      int_position_error += position_error * elapsed_time;
      double derivative = (position_error - prev_position_error) / elapsed_time;
      prev_position_error = position_error;
      
      
      if( fabs(position_error) < 0.5 ){
        int_position_error = 0;
      }
      
      // Calcula o sinal de controle PID
      double velocity_setpoint =          \
        ( Kp_pos * position_error     ) + \
        ( Ki_pos * int_position_error ) + \
        ( Kd_pos * derivative         );

      // // Calculo da malha de controle interna (Velocidade)
      // DEBUG_SERIAL( "VEL", String(sensor->scaled_velocity )); 
      // double velocity_error = velocity_setpoint - sensor->scaled_velocity;
      // int_velocity_error += velocity_error * elapsed_time;
      // double velocity_derivative = (velocity_error - prev_velocity_error) / elapsed_time;
      // prev_velocity_error = velocity_error;

      // Calcula o sinal de controle de saida para o motor 
      double control_signal =  velocity_setpoint;
      // double control_signal =             \
        // ( Kp_vel * velocity_error     ) + \
        // ( Ki_vel * int_velocity_error ) + \
        // ( Kd_vel * velocity_derivative );

      // Adiciona o torque feedforward para suprir o atrito do motor 
      if (fabs(control_signal) < 0.1) {
          control_signal = 0.0;
      } else {
          control_signal += torque_feedforward;
      }

      // Limita o sinal de controle 
      if ( control_signal > BDC_MAX_POWER ) {
        control_signal = BDC_MAX_POWER;
      } else if ( control_signal < -BDC_MAX_POWER ) {
        control_signal = -BDC_MAX_POWER;
      }

      // Corrige orientação 
      if ( control_signal > 0 ){
        motor->set_direction( BDC_FORWARD ); 
      } else {
        motor->set_direction( BDC_BACKWARD );  
        control_signal = -control_signal;
      }
      motor->set_speed( fabs(control_signal) );



      // Debug
      // DEBUG_SERIAL( "PID", "Current position: " + String(this->sensor->scaled_position) ); 
      // DEBUG_SERIAL( "PID", "Control signal: " + String(control_signal) ); 
    
    // Desliga o motor
    } else {
      motor->set_speed( 0 );
    }
    vTaskDelay( pdMS_TO_TICKS( this->measurement_time ) );
  }
}


esp_err_t MotorController::set_pos_pid_gains( double Kp, double Ki, double Kd ){
  this->Kp_pos = Kp;
  this->Ki_pos = Ki;
  this->Kd_pos = Kd;
  return ESP_OK;
}

esp_err_t MotorController::set_vel_pid_gains( double Kp, double Ki, double Kd ){
  this->Kp_vel = Kp;
  this->Ki_vel = Ki;
  this->Kd_vel = Kd;
  return ESP_OK;
}

// Configurar posição alvo
esp_err_t MotorController::set_target( double target ){
  this->setpoint = target;
  return ESP_OK;
}

// Iniciar o controle
esp_err_t MotorController::start(){
  this->status = CONTROLLER_ON;
  return ESP_OK;
}

// Parar o motor
esp_err_t MotorController::stop(){ 
  this->status = CONTROLLER_OFF;
  this->int_position_error = 0.0;
  this->int_velocity_error = 0.0;
  this->prev_position_error = 0.0;
  this->prev_velocity_error = 0.0;
  return ESP_OK;
}