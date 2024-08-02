/**
  * @file motorController.h
  * @brief Classe de Controle de posição do Motor utilziando os sensores adequados 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */


#include "includes/motorController.h"

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
      // return ESP_ERR_NOT_FOUND;
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

}

esp_err_t MotorController::set_pid_gains( double Kp, double Ki, double Kd ){
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;
  return ESP_OK;
}

// Configurar posição alvo
esp_err_t MotorController::set_target( double target ){
  this->setpoint = target;
  return ESP_OK;
}

// Aplica um passo de cálculo das variáveis de controle 
esp_err_t MotorController::step() {
  if ( this->status == CONTROLLER_ON ){
    uint64_t current_time = esp_timer_get_time()/1000;
    double delta_time = (current_time - this->last_measurement) / 1000.0; // Tempo em segundos
    this->last_measurement = current_time;

    // Ler a posição atual do sensor 
    sensor->read_scaled();
    
    // Calcular o erro 
    error = this->setpoint - this->sensor->scaled_position;
    // Acumulador do Integrativo 
    int_error += error * delta_time;
    // Diferencial 
    dif_error = (error - prev_error) / delta_time;
    
    // Calculo da malha de controle 
    double control_signal = (Kp * error) + (Ki * int_error) + (Kd * dif_error);

    // Define a direção de giro 
    if (control_signal > 0) {
        this->motor->set_direction(BDC_FORWARD);
    } else {
        this->motor->set_direction(BDC_BACKWARD);
        control_signal = -control_signal;
    }
    // Aplica a velocidade adequada ao atuador 
    this->motor->set_speed(control_signal);
    // Armazena o erro atual para a próxima iteração   
    prev_error = error;
    // Debug
    DEBUG_SERIAL( "PID", "Current position: " + String(this->sensor->scaled_position) ); 
    DEBUG_SERIAL( "PID", "Control signal: " + String(control_signal) ); 
    return ESP_OK;
  }else{
    return ESP_ERR_INVALID_STATE;
  }
}


// Iniciar o controle
esp_err_t MotorController::start(){
  this->status = CONTROLLER_ON;
  return ESP_OK;
}

// Parar o motor
esp_err_t MotorController::stop(){ 
  this->status = CONTROLLER_OFF;
  return ESP_OK;
}