/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 22/07/2024
 *  License: MIT
 *
*/

#include "driver/gpio.h"
#include "driver/ledc.h"

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


/**  FUNÇÕES PARA MOTOR CONTROL */
void bdc_motor_init_device(void);

/* 13 bits de resolução = 8192 de duty máximo */
void bdc_motor_set_speed( double speed);

uint8_t bdc_motor_forward(void);

uint8_t bdc_motor_backward(void);

uint8_t bdc_motor_stop(void);

uint8_t bdc_motor_brake(void);

static inline void bdc_motor_enable(void);

static inline void bdc_motor_disable(void);