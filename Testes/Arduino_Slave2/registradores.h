/*  
 *  Description: Constants used in Modbus registers 
 *  Author: Bruno G. F. Sampaio 
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#ifndef REGISTRADORES_H_
#define REGISTRADORES_H_ 


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

#define INPUT_SENSOR_POS     0x01
#define INPUT_SENSOR_STATUS  0x02

#define INPUT_SUN_TARGET     0x03

#define INPUT_POWER_GEN  	   0x04   
#define INPUT_TEMPERATURE    0x05
#define INPUT_PRESURE        0x06

#define INPUT_YEAR           0x07
#define INPUT_MONTH          0x08
#define INPUT_DAY            0x09
#define INPUT_HOUR           0x0A
#define INPUT_MINUTE         0x0B
#define INPUT_SECOND         0x0C


// MODBUS HOLDING REGISTER ADDRESSES
#define HR_STATE             0x00

#define HR_MOTOR_PV          0x01
#define HR_MOTOR_KP          0x02
#define HR_MOTOR_KI          0x03
#define HR_MOTOR_KD          0x04

#define HR_ALTITUDE          0x05
#define HR_LATITUDE          0x06
#define HR_LONGITUDE         0x08

#define HR_YEAR              0x0A
#define HR_MONTH             0x0B
#define HR_DAY               0x0C
#define HR_HOUR              0x0D
#define HR_MINUTE            0x0E
#define HR_SECOND            0x0F


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


#endif