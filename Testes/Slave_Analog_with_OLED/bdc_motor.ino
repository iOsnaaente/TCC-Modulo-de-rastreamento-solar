/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

/* Constantes para configuração do LEDC */
#define LEDC_GPIO_NUM     ((uint8_t)(4))         //        
#define LEDC_GPIO         GPIO_NUM_4             //      
#define LEDC_MODE         LEDC_HIGH_SPEED_MODE   //      
#define LEDC_CHANNEL      LEDC_CHANNEL_0         //            
#define LEDC_TIMER        LEDC_TIMER_0           //          
#define LEDC_RESOLUTION   LEDC_TIMER_13_BIT      // 
#define LEDC_FREQ         ((uint32_t)1000)       //  
              
/* Constantes para definir valores mínimos e máximo de potência */
#define BDC_TURN_OFF      ((double)(0))          //        
#define BDC_MIN_POWER     ((double)(2000))        //          
#define BDC_MAX_POWER     ((double)(8192))       //            

/* Constantes para estados on-off */
#define BDC_ENABLE        ((bool)(true))         // 
#define BDC_DISABLE       ((bool)(false))        // 

/* Constantes para definir o estado de mocimento do motor */
#define BDC_STOPED        ((uint8_t)(0))         //   
#define BDC_BREAK         ((uint8_t)(1))         //   
#define BDC_FORWARD       ((uint8_t)(2))         //   
#define BDC_BACKWARD      ((uint8_t)(3))         //   

/*  
 *  FUNÇÕES PARA MOTOR CONTROL 
 */
void bdc_motor_init_device(void) {
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
    .gpio_num = LEDC_GPIO_NUM,          /*!< the LEDC output gpio_num, if you want to use gpio16, gpio_num = 16 */
    .speed_mode = LEDC_MODE,            /*!< LEDC speed speed_mode, high-speed mode [0] or low-speed mode [] */
    .channel = LEDC_CHANNEL,            /*!< LEDC channel (0 - 7) */
    .intr_type = LEDC_INTR_DISABLE,     /*!< configure interrupt, Fade interrupt enable  or Fade interrupt disable */    
    .timer_sel = LEDC_TIMER,            /*!< Select the timer source of channel (0 - 3) */
    .duty = 0,                          /*!< LEDC channel duty, the range of duty setting is [0, (2**duty_resolution)] */
  };
  ledc_channel_config( &pwm_config );
}


/* 13 bits de resolução = 8192 de duty máximo */
void bdc_motor_set_speed( double speed) {
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
}

uint8_t bdc_motor_forward(void) {
  bdc_motor_enable();
  gpio_set_level(IN1, true);
  gpio_set_level(IN2, false);
  return BDC_FORWARD;
}

uint8_t bdc_motor_backward(void) {
  bdc_motor_enable();
  gpio_set_level(IN1, false);
  gpio_set_level(IN2, true);
  return BDC_BACKWARD;
}

uint8_t bdc_motor_stop(void) {
  bdc_motor_disable();
  gpio_set_level(IN1, false);
  gpio_set_level(IN2, false);
  return BDC_STOPED;
}

uint8_t bdc_motor_brake(void) {
  bdc_motor_disable();
  gpio_set_level(IN1, true);
  gpio_set_level(IN2, true);
  return BDC_BREAK;
}

static inline void bdc_motor_enable(void) {
  gpio_set_level(ENB, BDC_ENABLE);
}

static inline void bdc_motor_disable(void) {
  gpio_set_level(ENB, BDC_DISABLE);
}