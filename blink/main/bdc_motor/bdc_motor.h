#include <stdint.h>

#define    BDC_TURN_OFF        (0)
#define    BDC_MIN_POWER     (100)
#define    BDC_MAX_POWER    (1024)

#define    BDC_ENABLE          (1)
#define    BDC_DISABLE         (0)

#define    BDC_STOPED          (0)
#define    BDC_BREAK           (1)
#define    BDC_FORWARD         (2)
#define    BDC_BACKWARD        (3)
#define    BDC_ENABLED         (4)
#define    BDC_DISABLED        (5)


typedef struct bdc_motor_t {
    uint8_t gpio_IN1;       /*!< BDC Motor IN A gpio number  */
    uint8_t gpio_IN2;       /*!< BDC Motor IN B gpio number  */
    uint32_t gpio_PWM;      /*!< BDC Motor PWM A gpio number */
    uint32_t pwm_freq_hz;   /*!< PWM frequency in Hz */
    uint8_t direction;      /*!< Spin direction */
    float duty;             /*!< PWM output */
} bdc_motor_t;


// Init 
void bdc_motor_init_device( bdc_motor_t motor );

// Speed 
void bdc_motor_set_speed( bdc_motor_t motor, uint32_t speed);

// Directions
void bdc_motor_forward ( bdc_motor_t motor );
void bdc_motor_backward( bdc_motor_t motor );
void bdc_motor_coast   ( bdc_motor_t motor );
void bdc_motor_brake   ( bdc_motor_t motor );

// Configurations 
void bdc_motor_enable  ( bdc_motor_t motor );
void bdc_motor_disable ( bdc_motor_t motor );
