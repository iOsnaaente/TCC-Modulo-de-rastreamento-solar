from pymodbus.client import ModbusTcpClient
from datetime import datetime

import time 

# Configurações do Modbus
MODBUS_IP = '192.168.137.223'   # Endereço IP do dispositivo Modbus
MODBUS_IP = '10.0.0.106'        # Endereço IP do dispositivo Modbus
MODBUS_PORT = 502               # Porta Modbus TCP

# Endereços Modbus aleatórios para data e hora
HR_YEAR      = 0x0A
HR_MONTH     = 0x0B
HR_DAY       = 0x0C
HR_HOUR      = 0x0D
HR_MINUTE    = 0x0E
HR_SECOND    = 0x0F
COIL_DT_SYNC = 0x02


if __name__ == '__main__':
    client = ModbusTcpClient( MODBUS_IP, port = MODBUS_PORT )
    if client.connect():
        print("Conectado ao dispositivo Modbus")
        now = datetime.now()
        year = now.year - 2000  # Ajuste do ano para dois dígitos
        month = now.month
        day = now.day
        hour = now.hour
        minute = now.minute
        second = now.second
        print(f"Enviando data e hora: {now}")

        # Escrever os valores nos registros Modbus
        client.write_registers( HR_YEAR, [ year, month, day, hour, minute, second ], 0x12     ) 
        time.sleep(0.1)
        client.write_coil( COIL_DT_SYNC, True ) 

        print("Data e hora enviadas com sucesso")
        client.close()
    else:
        print("Falha ao conectar ao dispositivo Modbus")
