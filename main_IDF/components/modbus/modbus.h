/*  
 *  Description: 
 *  Author: Bruno G. F. Sampaio
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#include <WiFi.h>
#include <ModbusIP_ESP8266.h>

#include "registradores.h"

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


IPAddress remote(192, 168, 125,  5 );  // Address of Modbus Slave device
ModbusIP mb;                           // ModbusIP object

void init_modbus_registers( void ){

}

void init_modbus_communication(void) {
  #ifdef HIDDEN_NET
  WiFi.begin( SSID, PASSWORD, 0, nullptr, true );
  Wifi.beginHidden( HIDDEN_SSID )
  #else 
  WiFi.begin( SSID, PASSWORD  );
  #endif 

  while (WiFi.status() != WL_CONNECTED){};
  mb.client();
}


void modbus_task( void ){
  while (true) {
    if (mb.isConnected(remote)) {      // Check if connection to Modbus Slave is established
      mb.readHreg(remote, REG, &res);  // Initiate Read Coil from Modbus Slave
    } else {
      mb.connect(remote);  // Try to connect if no connection
    }
    mb.task();  // Common local Modbus task
  }
}





