///*
// * bdc_motor.c
// *
// *  Created on: 24 de dez. de 2023
// *      Author: BrunoSampaio
// */
//
//
//#include "bdc_motor.h"
//
//
//void bdc_motor_init_device( bdc_motor_t motor ){
//    // Define os pinos como direção de saída
//    gpio_set_direction(motor.gpio_IN1, GPIO_MODE_OUTPUT);
//    gpio_set_direction(motor.gpio_IN2, GPIO_MODE_OUTPUT);
//    gpio_set_direction(motor.gpio_PWM, GPIO_MODE_OUTPUT);
//    // Define o modo da saída como PWM
//    gpio_set_pull_mode(motor.gpio_PWM, GPIO_PULLUP_ENABLE );
//    // Configura a frequência do PWM
//    gpio_set_pwm_freq(motor.gpio_PWM, motor.pwm_freq_hz );
//    // Configura o ciclo de trabalho do PWM
//    gpio_set_pwm_duty(motor.gpio_PWM, BDC_TURN_OFF );
//    // Ativa a saída
//    gpio_set_level(motor.gpio_PWM, BDC_ENABLE );
//}
//
//void bdc_motor_set_speed(bdc_motor_t motor, uint32_t speed) {
//    if (speed <= 1000 && speed > 0) {
//        float duty_cycle = ((float)speed / 1000.0);
//        motor.duty = duty_cycle;
//        gpio_set_pwm_duty(motor.gpio_PWM, duty_cycle);
//    }
//}
//
//void bdc_motor_forward(bdc_motor_t motor){
//    gpio_set_level( motor.gpio_IN1, true );
//    gpio_set_level( motor.gpio_IN2, false );
//    motor.direction = BDC_FORWARD;
//}
//
//void bdc_motor_backward(bdc_motor_t motor){
//    gpio_set_level( motor.gpio_IN1, false );
//    gpio_set_level( motor.gpio_IN2, true );
//    motor.direction = BDC_BACKWARD;
//}
//
//void bdc_motor_coast(bdc_motor_t motor){
//    bdc_motor_set_speed( motor, BDC_TURN_OFF );
//    gpio_set_level( motor.gpio_IN1, false );
//    gpio_set_level( motor.gpio_IN2, false );
//    motor.direction = BDC_STOPED;
//}
//
//void bdc_motor_brake(bdc_motor_t motor){
//    bdc_motor_set_speed( motor, BDC_TURN_OFF );
//    gpio_set_level( motor.gpio_IN1, true );
//    gpio_set_level( motor.gpio_IN2, true );
//    motor.direction = BDC_BREAK;
//}
//
//void bdc_motor_enable( bdc_motor_t motor ){
//    gpio_set_level(motor.gpio_PWM, BDC_ENABLE );
//    motor.direction = BDC_ENABLE;
//}
//
//void bdc_motor_disable( bdc_motor_t motor ){
//    gpio_set_level(motor.gpio_PWM, BDC_DISABLE );
//    motor.direction = BDC_DISABLE;
//}
