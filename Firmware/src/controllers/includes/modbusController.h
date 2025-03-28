/**
  * @file modbus.h
  * @brief Classe de uso modbus. 
  * @author Bruno Gabriel Flores Sampaio, 
  * @date 23 de julho de 2024
  */

#ifndef _MODBUS_H_ 
#define _MODBUS_H_ 

/* Importar a biblioteca externa */
#include "ModbusIP_ESP8266.h"


#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_netif.h>

#include "esp_chip_info.h"
#include "esp_mac.h"

#include <nvs_flash.h>

/* Configurações de placa */
#include "../../../board_config.h"

/* Módulos */ 
#include "../../modules/spa/spa.h"
#include "../../modules/rtcs/softRTC.h"
#include "../../modules/rtcs/ds3231.h"

/* Interfaces */ 
#include "../../interfaces/RTC.h"


#include "serialController.h"

/*
 * +-----------------------+-----------------+---------------------------------------------+
 * | Coil/Register Numbers |  Data Addresses |       Type	Table Name                       |
 * +-----------------------+-----------------+---------------------------------------------+
 * |        1-9999         |  0000 to 270E   | Read-Write	Discrete Output Coils            |   
 * |      10001-19999      |  0000 to 270E   | Read-Only	Discrete Input Contacts          |     
 * |      30001-39999      |  0000 to 270E	 | Read-Only	Analog Input Registers           |     
 * |      40001-49999      |  0000 to 270E	 | Read-Write	Analog Output Holding Registers  |             
 * +------------------------+----------------+---------------------------------------------+
 */
#define COIL_REG_OFFSET           1      // Modbus COIL     reg Offset
#define DISCRETE_REG_OFFSET   10000      // Modbus DISCRETE reg Offset
#define ANALOG_REG_OFFSET     20000      // Modbus ANALOG   reg Offset
#define HOLDING_REG_OFFSET    30000      // Modbus HOLDING  reg Offset


// MODBUS INPUTS REGISTER ADDRESSES
#define INPUT_SYSTEM_MODE    0x00

#define INPUT_SENSOR_STATUS  0x01
#define INPUT_SENSOR_POS     0x02

#define INPUT_SUN_TARGET     0x04
#define INPUT_ZENITH         0x06
#define INPUT_AZIMUTH        0x08

#define INPUT_POWER_GEN  	   0x0A   
#define INPUT_TEMPERATURE    0x0C
#define INPUT_PRESURE        0x0E

#define INPUT_YEAR           0x10
#define INPUT_MONTH          0x11
#define INPUT_DAY            0x12
#define INPUT_HOUR           0x13
#define INPUT_MINUTE         0x14
#define INPUT_SECOND         0x15



// MODBUS HOLDING REGISTER ADDRESSES
#define HR_STATE             0x00

#define HR_MOTOR_PV          0x01
#define HR_MOTOR_KP          0x03
#define HR_MOTOR_KI          0x05
#define HR_MOTOR_KD          0x07

#define HR_ALTITUDE          0x09
#define HR_LATITUDE          0x0B
#define HR_LONGITUDE         0x0D

#define HR_YEAR              0x0F
#define HR_MONTH             0x10
#define HR_DAY               0x11
#define HR_HOUR              0x12
#define HR_MINUTE            0x13
#define HR_SECOND            0x14


// MODBUS DISCRETES REGISTER ADDRESSES
#define DISCRETE_ADC1_0      0x00
#define DISCRETE_ADC1_3      0x01
#define DISCRETE_ADC1_6      0x03

#define DISCRETE_FAIL        0x04
#define DISCRETE_SYNC        0x05


// MODBUS COILS REGISTER ADDRESSES
#define COIL_POWER           0x00   
#define COIL_LED             0x01
#define COIL_DT_SYNC         0x02
#define COIL_PUSH_BUTTON_1   0x03
#define COIL_PUSH_BUTTON_2   0x04
#define COIL_PUSH_BUTTON_3   0x05



/* *
 * +----------------+-------------+-------------------------------------------------+
 * | Function Code	|    Action	  |          Table Name                             |
 * +----------------+-------------+-------------------------------------------------+
 * |      01        |   (01 hex)	|  Read	Discrete Output Coils                     |
 * |      02        |   (02 hex)	|  Read	Discrete Input Contacts                   |
 * |      03        |   (03 hex)	|  Read	Analog Output Holding Registers           |
 * |      04        |   (04 hex)	|  Read	Analog Input Registers                    |
 * |      05        |   (05 hex)	|  Write single	Discrete Output Coil              |
 * |      06        |   (06 hex)	|  Write single	Analog Output Holding Register    |
 * |      15        |   (0F hex)	|  Write multiple	Discrete Output Coils           |
 * |      16        |   (10 hex)	|  Write multiple	Analog Output Holding Registers |
 * +----------------+-------------+-------------------------------------------------+
 * */
#define READ_COIL                 0x01
#define READ_DISCRETE             0x02
#define READ_HOLDING              0x03
#define READ_ANALOG               0x04
#define WRITE_SINGLE_COIL         0x05
#define WRITE_SINGLE_HOLDING      0x06
#define WRITE_MULTIPLE_COILS      0x0F
#define WRITE_MULTIPLE_HOLDINGS   0x10

/* *
 * AE41 5652  ->  high byte first / high word first  = "big endian"
 * 5652 AE41  ->  high byte first / low word first  
 * 41AE 5256  ->  low byte first  / high word first 
 * 5256 41AE  ->  low byte first  / low word first   = "little endian"
 * */


/* * 
 * Supported Register Function codes:
 * +----------------+--------+--------------------------------------------+
 * | Function Code	| Action |         Table Name                         |
 * +----------------+--------+--------------------------------------------+
 * |  04 (04 hex)	  |  Read  |   Analog Input Registers                   |
 * |  03 (03 hex)	  |  Read  |   Analog Output Holding Registers          |
 * |  06 (06 hex)	  |  Write |   single	Analog Output Holding Register    |
 * |  16 (10 hex)	  |  Write |   multiple	Analog Output Holding Registers |
 * +----------------+--------+--------------------------------------------+
 * */

/* * 
 * Supported Coil Function codes:
 * +----------------+--------+----------------------------------+
 * | Function Code	| Action |        Table Name                |
 * +----------------+--------+----------------------------------+
 * | 02 (02 hex)	  |  Read  |   Discrete Input Contacts        |
 * | 01 (01 hex)	  |  Read  |   Discrete Output Coils          |
 * | 05 (05 hex)	  |  Write |   single	Discrete Output Coil    |
 * | 15 (0F hex)	  |  Write |   multiple	Discrete Output Coils |
 * +----------------+--------+----------------------------------+
 * */


// Estados de funcionamento do sistema 
#define HR_STATE_AUTO          0

#define HR_STATE_QUADRANT_1   01
#define HR_STATE_QUADRANT_2   02
#define HR_STATE_QUADRANT_3   03
#define HR_STATE_QUADRANT_4   04
#define HR_STATE_QUADRANT_12  12
#define HR_STATE_QUADRANT_23  23
#define HR_STATE_QUADRANT_34  34
#define HR_STATE_QUADRANT_41  41

#define HR_STATE_GO_HOME      253
#define HR_STATE_WAITING      254
#define HR_STATE_OFF          255 


#ifdef ZENITE_MODE 
  // Angulos de posição em quadrante do sistema em Graus 
  #define HR_QUADRANT_1_ANGLE   ((float)(270.000))
  #define HR_QUADRANT_2_ANGLE   ((float)(180.00))
  #define HR_QUADRANT_3_ANGLE   ((float)(90.0))
  #define HR_QUADRANT_4_ANGLE   ((float)(0.0))

  #define HR_QUADRANT_41_ANGLE  ((float)(45.00))
  #define HR_QUADRANT_34_ANGLE  ((float)(135.0))
  #define HR_QUADRANT_23_ANGLE  ((float)(225.0))
  #define HR_QUADRANT_12_ANGLE  ((float)(315.0))
  #define POS_HOME_MIN 90.0
  #define POS_HOME_MAX 91.0
#endif 

#ifdef AZIMUTE_MODE
  // Angulos de posição em quadrante do sistema em Graus 
  #define HR_QUADRANT_1_ANGLE   ((float)(0.000))
  #define HR_QUADRANT_2_ANGLE   ((float)(90.00))
  #define HR_QUADRANT_3_ANGLE   ((float)(180.0))
  #define HR_QUADRANT_4_ANGLE   ((float)(270.0))
  #define HR_QUADRANT_12_ANGLE  ((float)(45.00))
  #define HR_QUADRANT_23_ANGLE  ((float)(135.0))
  #define HR_QUADRANT_34_ANGLE  ((float)(225.0))
  #define HR_QUADRANT_41_ANGLE  ((float)(315.0))
  #define POS_HOME_MIN 100.0
  #define POS_HOME_MAX 101.0
#endif 



static void event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);


class ModbusController {
private:
  void modbusDatetimeTask( void * pvParameters );
  void modbusScanTask( void * pvParameters );

public:
  const char *description; 
  
  // Cria a instancia para usar as Tasks como método.
  static ModbusController *Instance;
  TaskHandle_t *modbusDatetimeTaskHandler;
  TaskHandle_t *modbusScanTaskHandler;
  TaskHandle_t *modbus_TaskHandler;
  SemaphoreHandle_t xModbusSemaphore;

  // Objetos 
  ModbusIP *mb;
  RTC *rtc;

  // Periodo de atualização da rede modbus 
  uint32_t measurement_time = 1000;

  ModbusController( const char *description, RTC_type_t RTC_TYPE );
  void begin_connection( void ); 
  void update_datetime( struct datetime_buffer_t dt);
  void scan();
};



#endif // _MODBUS_H_