/*  
 *  Description: Constants used in Modbus registers 
 *  Author: Bruno G. F. Sampaio 
 *  Date: 04/01/2024
 *  License: MIT
 *
*/

#ifndef REGISTRADORES_H_
#define REGISTRADORES_H_ 

// MODBUS INPUTS REGISTER ADDRESSES
#define INPUT_POS_GIR        0x00
#define INPUT_AZIMUTE        0x04    
#define INPUT_GENERATION 	   0x08     

#define INPUT_TEMP           0x0A
#define INPUT_PRESURE        0x0C

#define INPUT_SENS_CONF_GIR  0x0E

#define INPUT_YEAR           0x12
#define INPUT_MONTH          0x13
#define INPUT_DAY            0x14
#define INPUT_HOUR           0x15
#define INPUT_MINUTE         0x16
#define INPUT_SECOND         0x17



// MODBUS HOLDING REGISTER ADDRESSES
#define HR_PV_GIR            0x00
#define HR_KP_GIR            0x02
#define HR_KI_GIR            0x04
#define HR_KD_GIR            0x06
#define HR_AZIMUTE           0x08
#define HR_PV_ELE            0x0A
#define HR_KP_ELE            0x0C
#define HR_KI_ELE            0x0E
#define HR_KD_ELE            0x10
#define HR_ALTITUDE          0x12
#define HR_LATITUDE          0x14
#define HR_LONGITUDE         0x16
#define HR_STATE             0x18
#define HR_YEAR              0x19
#define HR_MONTH             0x1A
#define HR_DAY               0x1B
#define HR_HOUR              0x1C
#define HR_MINUTE            0x1D
#define HR_SECOND            0x1E


// MODBUS DISCRETES REGISTER ADDRESSES
#define DISCRETE_FAIL        0x00
#define DISCRETE_POWER       0x01
#define DISCRETE_TIME        0x02
#define DISCRETE_GPS         0x03


// MODBUS COILS REGISTER ADDRESSES
#define COIL_POWER           0x00   
#define COIL_LED             0x01
#define COIL_M_GIR           0x02 
#define COIL_M_ELE           0x03
#define COIL_LEDR            0x04  
#define COIL_LEDG            0x05  
#define COIL_LEDB            0x06
#define COIL_SYNC_DATE       0x07


//endif