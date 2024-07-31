/**
 * @file Serial.h
 * @brief Header do Gerenciador de Comunicação Serial 
 * @details Este arquivo contém as definição e prototipos de funções para uso da
 * interface serial.
 * @author Bruno Gabriel Flores Sampaio,
 * @date 22 de julho de 2024
 */


#ifndef SERIAL_CONTROLLER_H_
#define SERIAL_CONTROLLER_H_

#include "../boardConfig.h"

#ifdef DEBUG_SERIAL_COMPLETO
  #define DEBUG_SERIAL( TIPO, MESSAGE ) \
    if (xSemaphoreTake(serialDebuggerMutex, ( TickType_t ) pdMS_TO_TICKS(1) ) == pdTRUE) { \
      USB_BUS.println( "[" + String( TIPO) + "] " + String(__FILE__) + " [" + String(__LINE__) + "]: " + MESSAGE); \
      xSemaphoreGive(serialDebuggerMutex); \
    }
#endif

#ifdef DEBUG_SERIAL_REDUZIDO
  #define DEBUG_SERIAL( TIPO, MESSAGE ) \
    if (xSemaphoreTake(serialDebuggerMutex, ( TickType_t ) pdMS_TO_TICKS(1) ) == pdTRUE) { \
      USB_BUS.println( "[" + String( TIPO) + "] " + MESSAGE ); \
      xSemaphoreGive(serialDebuggerMutex); \
    }
#endif 

#ifdef DEBUG_SERIAL_DESLIGADO
  #define DEBUG_SERIAL( TIPO, MESSAGE ) {};
#endif 


/* Velocidade de comunicação entre Debug e Serial data */
#define UART_BAUDRATE 1000000     // Baudrate utilizado pela Serial do USB 


/* Mutex para proteger o acesso à porta serial */
extern SemaphoreHandle_t serialDebuggerMutex;

/* Variável global para controlar o acesso à porta serial e fila de dados In and Out */ 
extern QueueHandle_t messageOutQueue;
extern QueueHandle_t messageInQueue;


/**
 * @brief Inicializa as interfaces UART para comunicação.
 * @details Esta função configura as interfaces UART usadas para comunicação com dispositivos 
 * externos e para fins de depuração. Ela estabelece as taxas de transmissão (baud rates)
 * e outros parâmetros necessários para as portas seriais USB_BUS, TOP_BUS e PIN_BUS.
 * Além disso, garante que as interfaces seriais estejam prontas para transmissão e recepção
 * de dados.
 * @note Esta função deve ser chamada no início do programa para garantir que as interfaces
 *       seriais estejam devidamente inicializadas antes de qualquer comunicação ocorrer.
 */
void init_serial( void );


/**
 * @brief Tarefa responsável por escutar a porta serial para recebimento de dados.
 * @details Esta função deve ser usada como uma tarefa do FreeRTOS. Ela fica em constante
 * escuta nas portas seriais definidas (USB_BUS, TOP_BUS, PIN_BUS) para o recebimento
 * de dados. Quando dados são recebidos, eles são processados e armazenados em uma
 * fila (messageQueue) para posterior tratamento. A função também gerencia o acesso
 * concorrente às portas seriais usando um semáforo (serialMutex) para garantir a
 * integridade dos dados recebidos.
 * @param pvParameters Parâmetros de usuário passados durante a criação da tarefa.
 *                     No contexto atual, não é utilizado, mas está presente para
 *                     compatibilidade com a assinatura exigida pelo FreeRTOS.
 * @note Esta função não retorna valores e é destinada a ser executada indefinidamente
 *       como uma tarefa do FreeRTOS. A tarefa deve ser criada e gerenciada usando as
 *       funções específicas do FreeRTOS.
 */
void serialListener_Task( void *pvParameters );


/**
 * @brief Lê os valores da porta serial 
 * @details Essa função le os valores recebidos no barramento serial somente se, algum dado
 * tenha sido recebido, dessa forma, ele não bloqueia o código esperando novos dados. 
 * A função utiliza o Message message para armazenar as informações lida do barramento
 * para não ter que instanciar uma nova variavel toda vez que receber algum dado.
 * @param message Buffer para armazenar os valores lidos e salvar na Queue
 */
void _read_serial( serial_message_t &message );


/**
 * @brief Writes data to a serial port.
 * @details Essa função escreve os valores contidos na Queue, dentro do barramento serial. A
 * função usa o Message message como um buffer, para não ter que instanciar a variavel 
 * toda vez que for necessário fazer uma escrita.
 * @param message Buffer para receber os valores da Queue.
 */
void _write_serial( serial_message_t &message );


/**
 * @brief Calcula o checksum de uma mensagem.
 * @details Esta função calcula o checksum de uma mensagem e armazena o resultado no
 * campo `checksum` da mensagem. O checksum é calculado fazendo um xor todos
 * os bytes na mensagem.
 * @example Para usar, deve-se chamar esta função como ```_checksum(msg);``` e `msg` deve ser
 * uma estrutura de Message com msg.data[N], msg.length e msg.checksum, onde o checksum
 * será armazenado.
 * @param message A mensagem para a qual o checksum será calculado.
 */
uint8_t _checksum( serial_message_t& message );


/**
 * @brief Converte um buffer de bytes em uma representação de string hexadecimal.
 * @details Função itera sobre cada byte no buffer, converte-o para o seu equivalente
 * em string hexadecimal, e, anexa ao resultado da string. Para bytes menores que 0x10,
 * adiciona um zero à esquerda para manter um formato consistente de dois caracteres
 * para cada byte.
 * @note Essa função é util para ser utilizada em debugging de buffers: 
 *  ```Serial.print( "O buffer contem: " + buffer2String(buffer, len) )```
 * @param buffer Ponteiro para o buffer de bytes a ser convertido.
 * @param length O número de bytes no buffer a ser convertido.
 * @return Um objeto String contendo a representação hexadecimal dos bytes no buffer.
 */
String buffer2String( const uint8_t* buffer, size_t length);


#endif