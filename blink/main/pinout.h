/*  
 *  Description: Constants used in TCC board project 
 *  Author: Bruno G. F. Sampaio
 *  Date: 23/12/2023
 *  License: MIT
 *
*/

/* LED da placa para debug visual  */
#define LED_BUILTIN        2

/* ADC com soquete externo - Sensores */
#define ADC_EXT1          36
#define ADC_EXT2          39
#define ADC_EXT3          34

/* ADCs internos  */
#define ADC4              35
#define ADC5              33
#define ADC6              25

/* GPIO de controle do Relé de potência  */
#define POWER_RELAY       32

/* Interface SPI - Leitor de cartão micro SD  */
#define MOSI              23 
#define MISO              19
#define CS                15
#define SCK               18

/* Interface I2C com soquete externo */
#define SDA               21
#define SCL               22

/* GPIO para ponte H - L298N */
#define IN1               17
#define IN2               16
#define ENB                4

/* ADC de leitura de corrente da ponte H  */
#define I_SENSING         33

/* GPIO com soquete externo - Sensores  */
#define GPIO_EXT1         26
#define GPIO_EXT2         27
#define GPIO_EXT3         14


#define UART_BAUDRATE     115200
#define UART_MODE         SERIAL_8N1
#define UART_TXD0         1
#define UART_RXD0         3


