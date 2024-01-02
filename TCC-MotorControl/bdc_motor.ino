/*  
 *  DEFINES PARA MOTOR CONTROL 
 */

/*   */
#define LEDC_TIMER        LEDC_TIMER_0           //          
#define LEDC_FREQ         1000                   //  
#define LEDC_GPIO         GPIO_NUM_4             //        
#define LEDC_CHANNEL      LEDC_CHANNEL_0         //            
#define LEDC_MODE         LEDC_HIGH_SPEED_MODE   //      
#define LEDC_RESOLUTION   LEDC_TIMER_13_BIT      // 
              
/*   */
#define BDC_TURN_OFF      ((double)(0))          //        
#define BDC_MIN_POWER     ((double)(100))        //          
#define BDC_MAX_POWER     ((double)(8192))       //            

/*   */
#define BDC_ENABLE        ((uint8_t)(1))         // 
#define BDC_DISABLE       ((uint8_t)(0))         // 

/*   */
#define BDC_STOPED        ((uint8_t)(0))         //   
#define BDC_BREAK         ((uint8_t)(1))         //   
#define BDC_FORWARD       ((uint8_t)(2))         //   
#define BDC_BACKWARD      ((uint8_t)(3))         //   
#define BDC_ENABLED       ((uint8_t)(4))         //   
#define BDC_DISABLED      ((uint8_t)(5))         //   


void bdc_motor_init_device(void) {
  // Define os pinos como direção de saída
  gpio_set_direction(IN1, GPIO_MODE_OUTPUT);
  gpio_set_direction(IN2, GPIO_MODE_OUTPUT);
  gpio_set_direction(ENB, GPIO_MODE_OUTPUT);
  
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
    .gpio_num = LEDC_GPIO,
    .speed_mode = LEDC_MODE,
    .channel = LEDC_CHANNEL,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER,
    .duty = 0,
    .hpoint = 0,
  };
  ledc_channel_config( &pwm_config );
  
}

/* 13 bits de resolução = 8192 de duty máximo */
void bdc_motor_set_speed( double speed) {
  double duty_cycle; 
  // Limita os valores entre MIN e MAX 
  if (speed < BDC_MIN_POWER) {
    duty_cycle = BDC_MIN_POWER;
  } else if ( speed > BDC_MAX_POWER ) {
    duty_cycle = BDC_MAX_POWER;
  }
  // Calcula a normalização linear
  duty_cycle = (speed - BDC_MIN_POWER) / (BDC_MAX_POWER - BDC_MIN_POWER);
  // Define um valor para o duty 
  ledcWrite(LEDC_CHANNEL, duty_cycle );
}

void bdc_motor_forward(void) {
  bdc_motor_enable();
  gpio_set_level(IN1, true);
  gpio_set_level(IN2, false);
}

void bdc_motor_backward(void) {
  bdc_motor_enable();
  gpio_set_level(IN1, false);
  gpio_set_level(IN2, true);
}

void bdc_motor_coast(void) {
  bdc_motor_disable();
  gpio_set_level(IN1, false);
  gpio_set_level(IN2, false);
}

void bdc_motor_brake(void) {
  bdc_motor_disable();
  gpio_set_level(IN1, true);
  gpio_set_level(IN2, true);
}

static inline void bdc_motor_enable(void) {
  gpio_set_level(ENB, BDC_ENABLE);
}

static inline void bdc_motor_disable(void) {
  gpio_set_level(ENB, BDC_DISABLE);
}