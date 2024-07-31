/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 22/07/2024
 *  License: MIT
 *
*/

#include "../../interfaces/motor.h"


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
#define BDC_MIN_POWER     ((double)(2000))       //
#define BDC_MAX_POWER     ((double)(8192))       //

#define BDC_ERROR   ((uint8_t)(0)) 
#define BDC_SUCCESS ((uint8_t)(1)) 


class L298N : public Motor {
public:
    /* Variáveis */
    bool direction;
    double speed;
    
    /* Construtor */
    L298N( const char* description );
    L298N( );

    /* Métodos */
    static inline void set_torque(void);
    uint8_t set_direction( bool direction);
    void set_speed( double speed);
    uint8_t stop(void);
    uint8_t brake(void);
}


