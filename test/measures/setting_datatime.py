# MODBUS INPUTS REGISTER ADDRESSES
INPUT_SYSTEM_MODE   = 0x00

INPUT_SENSOR_POS    = 0x01
INPUT_SENSOR_STATUS = 0x02

INPUT_SUN_TARGET    = 0x03
INPUT_ZENITH        = 0x04
INPUT_AZIMUTH       = 0x05

INPUT_POWER_GEN     = 0x06   

INPUT_TEMPERATURE   = 0x07
INPUT_PRESURE       = 0x08

INPUT_YEAR          = 0x09
INPUT_MONTH         = 0x0A
INPUT_DAY           = 0x0B
INPUT_HOUR          = 0x0C
INPUT_MINUTE        = 0x0D
INPUT_SECOND        = 0x0E

# MODBUS HOLDING REGISTER ADDRESSES
HR_STATE            = 0x00
HR_MOTOR_PV         = 0x01
HR_MOTOR_KP         = 0x02
HR_MOTOR_KI         = 0x03
HR_MOTOR_KD         = 0x04
HR_ALTITUDE         = 0x05
HR_LATITUDE         = 0x06
HR_LONGITUDE        = 0x08
HR_YEAR             = 0x0A
HR_MONTH            = 0x0B
HR_DAY              = 0x0C
HR_HOUR             = 0x0D
HR_MINUTE           = 0x0E
HR_SECOND           = 0x0F
# MODBUS DISCRETES REGISTER ADDRESSES
DISCRETE_ADC1_0     = 0x00
DISCRETE_ADC1_3     = 0x01
DISCRETE_ADC1_6     = 0x03
DISCRETE_FAIL       = 0x04
DISCRETE_SYNC       = 0x05
# MODBUS COILS REGISTER ADDRESSES
COIL_POWER          = 0x00   
COIL_LED            = 0x01
COIL_DT_SYNC        = 0x02

from pymodbus.client import ModbusTcpClient
import datetime
import struct
import time 

# Configurações do Modbus
# MODBUS_IP = '192.168.137.223'   # Endereço IP do dispositivo Modbus
# MODBUS_IP = '192.168.18.20'        # Endereço IP do dispositivo Modbus

MODBUS_IP1 = '10.0.0.110'          # Endereço IP do dispositivo Modbus
MODBUS_IP2 = '10.0.0.108'          # Endereço IP do dispositivo Modbus

# MODBUS_IP = '192.168.0.35'        # Endereço IP do dispositivo Modbus
# MODBUS_IP = '192.168.0.36'        # Endereço IP do dispositivo Modbus

MODBUS_PORT = 502               # Porta Modbus TCP


datetimes = {
    "equ_out" : datetime.datetime( 2024,  3, 21, 5, 0, 0),
    "sol_inv" : datetime.datetime( 2024,  6, 20, 5, 0, 0),
    "equ_pri" : datetime.datetime( 2024,  9, 22, 5, 0, 0),
    "sol_ver" : datetime.datetime( 2024, 12, 21, 5, 0, 0),
}

CLIENTS_IPS = [ MODBUS_IP1 ] #, MODBUS_IP2 ]

import os 

if __name__ == '__main__':
    clients = [ ModbusTcpClient( ip , port = MODBUS_PORT ) for ip in CLIENTS_IPS ]
    for file_name, date in datetimes.items():
        print(f"Processando arquivo {file_name} com data: {date}")
        with open( os.path.join( os.path.dirname(__file__), file_name  + ".csv"), 'w', newline='\n' ) as file:
            now = date 
            last_hour = now.hour
            while True:
                now += datetime.timedelta(seconds = 120 )
                try:
                    if clients[0].connect():
                        # Ajuste do datetime atual 
                        year = now.year - 2000  
                        month = now.month
                        day = now.day
                        hour = now.hour
                        minute = now.minute
                        second = now.second
                        if last_hour > now.hour:
                            break
                        last_hour = now.hour 
                        print(f"Enviando data e hora: {now}")
                        file.write( now.strftime("%Y-%m-%d %H:%M:%S") + ";" ) 
                        # Escrever os valores nos registros Modbus
                        for client in clients:
                            client.write_registers( HR_YEAR, [ year, month, day, hour, minute, second ], 0x12     ) 
                            client.write_coil( COIL_DT_SYNC, True ) 
                            # Ler os valores do registrador de leitura
                            res = client.read_input_registers( INPUT_ZENITH, 2 )
                            if res.isError():
                                print(f"Erro ao ler registrador Modbus: {res}")
                            else:
                                data = [ struct.unpack("h", struct.pack("H", i))[0]/100 for i in res.registers]
                                print(f"ID da Transação: {res.transaction_id}, Registros: {data}")
                            file.write( str(data[0]) + ";" + str(data[1]) + ";" )
                        file.write( "\n" )
                    else:
                        print("Falha ao conectar ao dispositivo Modbus")
                except KeyboardInterrupt as err :
                    break


