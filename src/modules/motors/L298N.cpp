/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#include "L298N.h"

/*  
 *  FUNÇÕES PARA MOTOR CONTROL 
 */

L298N::L298N( const char* description = "L298N Driver", uint8_t direction = BDC_FORWARD, double speed = 0.0 )
  : Motor( description, direction, speed ) {
  // Define os pinos como direção de saída
    gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << IN1) | (1ULL << IN2) | (1ULL << ENB), /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
    .mode = GPIO_MODE_OUTPUT,                                      /*!< GPIO mode: set input/output mode                     */
    .pull_up_en = GPIO_PULLUP_DISABLE,                             /*!< GPIO pull-up                                         */
    .pull_down_en = GPIO_PULLDOWN_DISABLE,                         /*!< GPIO pull-down                                       */
    .intr_type = GPIO_INTR_DISABLE,                                /*!< GPIO interrupt type                                  */
  };
  gpio_config(&io_conf);
  // Configura o timmer para PWM 
  ledc_timer_config_t ledc_timer = (ledc_timer_config_t){
    .speed_mode = LEDC_MODE,               // timer mode
    .duty_resolution = LEDC_RESOLUTION,    // resolution of PWM duty
    .timer_num = LEDC_TIMER,               // timer index
    .freq_hz = LEDC_FREQ,                  // frequency of PWM signal
    .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
  };
  ledc_timer_config( &ledc_timer );
  // Configura o CHANNEL usado pelo PWM 
  ledc_channel_config_t pwm_config = (ledc_channel_config_t){
    .gpio_num = ENB,                    /*!< the LEDC output gpio_num, if you want to use gpio16, gpio_num = 16 */
    .speed_mode = LEDC_MODE,            /*!< LEDC speed speed_mode, high-speed mode [0] or low-speed mode [] */
    .channel = LEDC_CHANNEL,            /*!< LEDC channel (0 - 7) */
    .intr_type = LEDC_INTR_DISABLE,     /*!< configure interrupt, Fade interrupt enable  or Fade interrupt disable */    
    .timer_sel = LEDC_TIMER,            /*!< Select the timer source of channel (0 - 3) */
    .duty = 0,                          /*!< LEDC channel duty, the range of duty setting is [0, (2**duty_resolution)] */
  };
  ledc_channel_config( &pwm_config );
}


/* 13 bits de resolução = 8192 de duty máximo */
uint8_t L298N::set_speed( double speed ) {
  // Limita os valores entre MIN e MAX 
  if (speed < BDC_MIN_POWER) {
    speed = BDC_MIN_POWER;
  } else if ( speed > BDC_MAX_POWER ) {
    speed = BDC_MAX_POWER;
  }
  // Calcula a normalização linear
  uint32_t duty = (uint32_t)(((speed - BDC_MIN_POWER) / (BDC_MAX_POWER - BDC_MIN_POWER))*BDC_MAX_POWER);
  // Define um valor para o duty 
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
  return BDC_SUCCESS;
}

uint8_t L298N::set_direction( uint8_t direction ) {
  if ( direction == BDC_FORWARD ){
    this->set_torque( BDC_ENABLE );
    gpio_set_level(IN1, true);
    gpio_set_level(IN2, false);
    return BDC_FORWARD;
  } else if ( direction == BDC_BACKWARD) {   
    this->set_torque( BDC_ENABLE );
    gpio_set_level(IN1, false);
    gpio_set_level(IN2, true);
    return BDC_BACKWARD;
  } else {
    return BDC_ERROR;
  }
}

uint8_t L298N::stop(void) {
  this->set_torque( BDC_DISABLE );
  gpio_set_level(IN1, false);
  gpio_set_level(IN2, false);
  return BDC_STOPED;
}

uint8_t L298N::brake(void) {
  this->set_torque( BDC_DISABLE );
  gpio_set_level(IN1, true);
  gpio_set_level(IN2, true);
  return BDC_BREAK;
}

uint8_t L298N::set_torque( bool torque ){
  if ( torque == BDC_DISABLE ) {
    gpio_set_level(ENB, BDC_DISABLE);
    return BDC_DISABLE;
  }else if ( torque == BDC_ENABLE ) {
    gpio_set_level(ENB, BDC_ENABLE);
    return BDC_ENABLE;
  }else{
    return BDC_ERROR;
  }
}
