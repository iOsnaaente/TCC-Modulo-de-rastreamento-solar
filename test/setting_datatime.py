from pymodbus.client import ModbusTcpClient
import datetime

import time 

# Configurações do Modbus
# MODBUS_IP = '192.168.137.223'   # Endereço IP do dispositivo Modbus
MODBUS_IP = '192.168.18.20'        # Endereço IP do dispositivo Modbus
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
    now = datetime.datetime.now()
    while True:
        now += datetime.timedelta(seconds=600)
        try:
            if client.connect():
                year = now.year - 2000  # Ajuste do ano para dois dígitos
                month = now.month
                day = now.day
                hour = now.hour
                minute = now.minute
                second = now.second
                print(f"Enviando data e hora: {now}")

                # Escrever os valores nos registros Modbus
                client.write_registers( HR_YEAR, [ year, month, day, hour, minute, second ], 0x12     ) 
                client.write_coil( COIL_DT_SYNC, True ) 
                print("Data e hora enviadas com sucesso")

                # client.close()
                time.sleep(1)
            else:
                print("Falha ao conectar ao dispositivo Modbus")
                time.sleep(1)
        except KeyboardInterrupt as err :
            break


