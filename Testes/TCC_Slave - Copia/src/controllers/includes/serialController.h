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
#define UART_BAUDRATE 1000000     // Baudrate utilizado pela Serial do USB 

/* Mutex para proteger o acesso à porta serial */
SemaphoreHandle_t serialDebuggerMutex;

void serial_begin() {
  // Iniciar o Mutex serial 
  serialDebuggerMutex = xSemaphoreCreateMutex();
  // Inicializa o Serial
  const uart_config_t uart_config = {
    .baud_rate = UART_BAUDRATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, UART_TXD0, UART_RXD0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
  DEBUG_SERIAL("UART COMM", "Iniciado a comunicação UART através de Serial0 [Pinos TXD0/RXD0].");
}

String buffer2String(const uint8_t* buffer, size_t length) {
  String result;
  for (size_t i = 0; i < length; ++i) {
    if (buffer[i] < 0x10)
      result += "0";
    result += String(buffer[i], 16);
  }
  return result;
}

#endif