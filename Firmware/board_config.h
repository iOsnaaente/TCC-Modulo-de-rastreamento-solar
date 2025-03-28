/**
 * @file board_config.h
 * @brief Gerenciador das configurações do código utilizado no projeto
 * de conclusão de curso TCC em Engenharia de controle e automação. 
 * @author Bruno Gabriel Flores Sampaio,
 * @date 22 de julho de 2024
 */


#ifndef BOARDs_CONFIG_H_
#define BOARDs_CONFIG_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "driver/gpio.h"


/** Posição geográfica */
#define LATITUDE    -29.16530765942215
#define LONGITUDE   -54.89831672609559

/* Velocidade de comunicação entre Debug */
#define UART_BAUDRATE 115200     


/** UNCOMMENT ONE OF THAT TO USE IN SYSTEM */
// #define AZIMUTE_MODE
#define ZENITE_MODE



/**
 * @brief Configurações do Wi-Fi
 * @details É necessário se definir as credenciais da rede WiFi
 * TODO: Implementar um WiFiManager para configurar a rede WiFi 
 *       sem a necessidade de recompilar o código.
 *       Ou então, implementar um armazenamento na memória não volátil
 */
#define SSID_MASTER         ((const char *)"Tracker-TCC")
#define PSD_MASTER          ((const char *)"Bruno123456")

#define SERVER_IP           ((const char *)"10.0.0.120")
#define SERVER_PORT         502


#define I2C_RX_BUFFER       1024
#define I2C_TX_BUFFER       1024 
#define I2C_BUS_TIMEOUT_MS  50 


/** 
 * @brief Mensagem de debug para serial. 
 * @details É usado um Mutex para não haver conflitos de uso de memórias compartilhadas e deadlocks
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
#define LED_BOARD         ((gpio_num_t)GPIO_NUM_2)

/* ADC com soquete externo - Sensores */
#define ADC_EXT1          ((gpio_num_t)GPIO_NUM_36)
#define ADC_EXT2          ((gpio_num_t)GPIO_NUM_39)
#define ADC_EXT3          ((gpio_num_t)GPIO_NUM_34)

#define ADC_EXT1_CHANNEL  ((adc1_channel_t)ADC1_CHANNEL_0)
#define ADC_EXT2_CHANNEL  ((adc1_channel_t)ADC1_CHANNEL_3)
#define ADC_EXT3_CHANNEL  ((adc1_channel_t)ADC1_CHANNEL_6)

/* ADCs internos  */
#define ADC4              ((gpio_num_t)GPIO_NUM_35)
#define ADC5              ((gpio_num_t)GPIO_NUM_33)
#define ADC6              ((gpio_num_t)GPIO_NUM_25)

#define ADC4_CHANNEL      ((adc1_channel_t)ADC1_CHANNEL_7)
#define ADC5_CHANNEL      ((adc1_channel_t)ADC1_CHANNEL_5)
#define ADC6_CHANNEL      ((adc2_channel_t)ADC2_CHANNEL_8)

/* GPIO de controle do Relé de potência  */
#define POWER_RELAY       ((gpio_num_t)GPIO_NUM_32)

/* Interface SPI - Leitor de cartão micro SD  */
#define BOARD_MOSI        ((gpio_num_t)GPIO_NUM_23) 
#define BOARD_MISO        ((gpio_num_t)GPIO_NUM_19)
#define BOARD_CS          ((gpio_num_t)GPIO_NUM_15)
#define BOARD_SCK         ((gpio_num_t)GPIO_NUM_18)

/* Interface I2C com soquete externo */
#define BOARD_SDA         ((gpio_num_t)GPIO_NUM_21)
#define BOARD_SCL         ((gpio_num_t)GPIO_NUM_22)


/* GPIO para ponte H - L298N */
#define IN1               ((gpio_num_t)GPIO_NUM_17)
#define IN2               ((gpio_num_t)GPIO_NUM_16)
#define ENB               ((gpio_num_t)GPIO_NUM_4)

/* ADC de leitura de corrente da ponte H  */
#define I_SENSING         ((gpio_num_t)GPIO_NUM_33)

/* GPIO com soquete externo - Sensores  */
#define GPIO_EXT1         ((gpio_num_t)GPIO_NUM_26)
#define GPIO_EXT2         ((gpio_num_t)GPIO_NUM_27)
#define GPIO_EXT3         ((gpio_num_t)GPIO_NUM_14)

#define UART_TXD0         ((gpio_num_t)GPIO_NUM_1)
#define UART_RXD0         ((gpio_num_t)GPIO_NUM_3)


/** Protótipo de funções auxiliares */
void blink_Task(void *pvParameters);

#endif