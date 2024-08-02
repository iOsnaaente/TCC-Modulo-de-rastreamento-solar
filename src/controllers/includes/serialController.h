/**
 * @file Serial.h
 * @brief Header do Gerenciador de Comunicação Serial 
 * @details Este arquivo contém as definição e prototipos de funções para uso da
 * interface serial. Dentre as responsabilidades da interface estão:
 * @author Bruno Gabriel Flores Sampaio,
 * @date 22 de julho de 2024
 */


#ifndef SERIAL_CONTROLLER_H_
#define SERIAL_CONTROLLER_H_

#include "../../../board_config.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include <WString.h>


/* Mutex para proteger o acesso à porta serial */
extern SemaphoreHandle_t serialDebuggerMutex;


#ifdef DEBUG_SERIAL_COMPLETO
  #define DEBUG_SERIAL( TIPO, MESSAGE ) \
    if (xSemaphoreTake(serialDebuggerMutex, ( TickType_t ) pdMS_TO_TICKS(1) ) == pdTRUE) { \
      String logMessage = "[" + String( TIPO) + "] " + String(__FILE__) + " [" + String(__LINE__) + "]: " + MESSAGE + "\n"; \
      uart_write_bytes(UART_NUM_0, logMessage.c_str(), strlen(logMessage.c_str())); \
      xSemaphoreGive(serialDebuggerMutex); \
    }
#endif

#ifdef DEBUG_SERIAL_REDUZIDO
  #define DEBUG_SERIAL( TIPO, MESSAGE ) \
    if (xSemaphoreTake(serialDebuggerMutex, ( TickType_t ) pdMS_TO_TICKS(1) ) == pdTRUE) { \
      String logMessage = "[" + String(TIPO) + "]: " + MESSAGE + "\n"; \
      uart_write_bytes(UART_NUM_0, logMessage.c_str(), strlen(logMessage.c_str())); \
      xSemaphoreGive(serialDebuggerMutex); \
    }
#endif 

#ifdef DEBUG_SERIAL_DESLIGADO
  #define DEBUG_SERIAL( TIPO, MESSAGE ) {};
#endif 


/* Velocidade de comunicação entre Debug e Serial data */
#define UART_BAUDRATE 115200     // Baudrate utilizado pela Serial do USB 

void serial_begin();
String buffer2String(const uint8_t* buffer, size_t length);

#endif