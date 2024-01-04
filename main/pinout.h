/*  
 *  Description: Constants used in TCC board project 
 *  Author: Bruno G. F. Sampaio
 *  Date: 23/12/2023
 *  License: MIT
 *
*/

#ifndef PINOUT_H_
#define PINOUT_H_ 

#include "driver/gpio.h"

/* LED da placa para debug visual  */
#define LED_BUILTIN        GPIO_NUM_2

/* ADC com soquete externo - Sensores */
#define ADC_EXT1          GPIO_NUM_36
#define ADC_EXT2          GPIO_NUM_39
#define ADC_EXT3          GPIO_NUM_34

/* ADCs internos  */
#define ADC4              GPIO_NUM_35
#define ADC5              GPIO_NUM_33
#define ADC6              GPIO_NUM_25


/* Interface SPI - Leitor de cartão micro SD  */
#define MOSI              GPIO_NUM_23 
#define MISO              GPIO_NUM_19
#define CS                GPIO_NUM_15
#define SCK               GPIO_NUM_18

/* Interface I2C com soquete externo */
#define SDA               GPIO_NUM_21
#define SCL               GPIO_NUM_22

/* ADC de leitura de corrente da ponte H  */
#define I_SENSING         GPIO_NUM_33

/* GPIO com soquete externo - Sensores  */
#define GPIO_EXT1         GPIO_NUM_26
#define GPIO_EXT2         GPIO_NUM_27
#define GPIO_EXT3         GPIO_NUM_14


#define UART_BAUDRATE     115200
#define UART_MODE         SERIAL_8N1
#define UART_TXD0         GPIO_NUM_1
#define UART_RXD0         GPIO_NUM_3


#endif