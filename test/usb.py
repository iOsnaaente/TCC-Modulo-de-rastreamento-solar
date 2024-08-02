import serial

# Configuração da porta serial (modifique de acordo com sua porta e configuração)
port = 'COM5'  # Substitua por sua porta serial, como 'COM3' no Windows ou '/dev/ttyUSB0' no Linux
baudrate = 9600  # Ajuste a taxa de transmissão conforme necessário

# Cria uma conexão serial
ser = serial.Serial(port, baudrate)

import time 
while True:
    # Dados a serem enviados (0xCAFE)
    data = bytes( "CU".encode() )
    # Envia os dados
    ser.write(data)
    time.sleep(0.001)

# Fecha a conexão
ser.close()
