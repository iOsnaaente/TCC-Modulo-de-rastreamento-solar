/**
 * @file board_config.h
 * @brief Gerenciador das configurações do código utilizado no projeto
 * de conclusão de curso TCC em Engenharia de controle e automação. 
 * @author Bruno Gabriel Flores Sampaio,
 * @date 22 de julho de 2024
 */


#ifndef BOARDs_CONFIG_H_
#define BOARDs_CONFIG_H_

#include "driver/gpio.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"


/** 
 * @brief Mensagem de debug para serial. 
 * @details É usaod um Mutex para não haver conflitos de uso de memórias compartilhadas e deadlocks  D:
 * Para desativar o debug serial, deve ser definido a Macrodefinição DEBUG_SERIAL_DESLIGADO
 * 
 * @example `#define DEBUG_SERIAL_COMPLETO`  -> Printa o nome do arquivo e linha + mensagem de debug
 * @example `#define DEBUG_SERIAL_REDUZIDO`  -> Printa somente a mensagem de debug
 * @example `#define DEBUG_SERIAL_DESLIGADO` -> Desliga o print de mensagens de debug 
 */

// #define DEBUG_SERIAL_COMPLETO 
#define DEBUG_SERIAL_REDUZIDO 
// #define DEBUG_SERIAL_DESLIGADO 


/**
 * @brief GPIO do LED da placa 
 */ 
#define LED_BUILTIN        GPIO_NUM_2


/* ADC com soquete externo - Sensores */
#define ADC_EXT1          GPIO_NUM_36
#define ADC_EXT2          GPIO_NUM_39
#define ADC_EXT3          GPIO_NUM_34

/* ADCs internos  */
#define ADC4              GPIO_NUM_35
#define ADC5              GPIO_NUM_33
#define ADC6              GPIO_NUM_25

/* GPIO de controle do Relé de potência  */
#define POWER_RELAY       GPIO_NUM_32

/* Interface SPI - Leitor de cartão micro SD  */
#define MOSI              GPIO_NUM_23 
#define MISO              GPIO_NUM_19
#define CS                GPIO_NUM_15
#define SCK               GPIO_NUM_18

/* Interface I2C com soquete externo */
#define SDA               GPIO_NUM_21
#define SCL               GPIO_NUM_22

/* GPIO para ponte H - L298N */
#define IN1               GPIO_NUM_17
#define IN2               GPIO_NUM_16
#define ENB               GPIO_NUM_4

/* ADC de leitura de corrente da ponte H  */
#define I_SENSING         GPIO_NUM_33

/* GPIO com soquete externo - Sensores  */
#define GPIO_EXT1         GPIO_NUM_26
#define GPIO_EXT2         GPIO_NUM_27
#define GPIO_EXT3         GPIO_NUM_14

#define UART_TXD0         GPIO_NUM_1
#define UART_RXD0         GPIO_NUM_3


#endif