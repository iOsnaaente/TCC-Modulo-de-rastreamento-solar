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
