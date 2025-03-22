/**
 * @file Serial.h
 * @brief Header do Gerenciador de Comunicação Serial 
 * @details Este arquivo contém as definição e prototipos de funções para uso da
 * interface serial. Dentre as responsabilidades da interface estão:
 * @author Bruno Gabriel Flores Sampaio,
 * @date 23 de julho de 2024
 */

#include "includes/serialController.h"

SemaphoreHandle_t serialDebuggerMutex;


// Inicializa o Serial baseado nos parametros definidos dentro de ../BoardConfig.h
void serial_begin( void ) {

  Serial.begin( UART_BAUDRATE );
  
  // Iniciar o Mutex serial 
  serialDebuggerMutex = xSemaphoreCreateMutex();
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
