from pymodbus.client import ModbusTcpClient
from datetime import datetime

# Definição dos endereços dos registradores
HR_YEAR         = 0x0F
HR_MONTH        = 0x10
HR_DAY          = 0x11
HR_HOUR         = 0x12
HR_MINUTE       = 0x13
HR_SECOND       = 0x14
COIL_DT_SYNC    = 0x02


# IPs dos dispositivos Modbus
devices = [
    "10.0.0.110",       # MOD1 Zenite  
    "10.0.0.111"        # MOD2 Azimute
]


# Obtém a data e hora atuais no formato esperado para enviar ao Modbus 
def get_datetime():
    now = datetime.now()
    return [now.year, now.month, now.day, now.hour, now.minute, now.second]


# Escreve a data e hora nos dispositivos
def write_datetime(client):
    data = get_datetime()
    registers = [HR_YEAR, HR_MONTH, HR_DAY, HR_HOUR, HR_MINUTE, HR_SECOND]
    for reg, value in zip(registers, data):
        client.write_register(reg, value)
    client.write_coil(COIL_DT_SYNC, True)


# Lê os valores escritos para verificação
def read_datetime(client):
    registers = [HR_YEAR, HR_MONTH, HR_DAY, HR_HOUR, HR_MINUTE, HR_SECOND]
    values = client.read_holding_registers(registers[0], len(registers))    
    if values.isError():
        print("Erro ao ler registradores")
    else:
        print("Data e hora lidas do dispositivo:", values.registers)


# Itera sobre os dispositivos e executa a sincronização
for device in devices:
    print(f"Conectando ao dispositivo: {device}")
    client = ModbusTcpClient(device)
    if client.connect():
        write_datetime(client)
        read_datetime(client)
        client.close()
    else:
        print(f"Falha ao conectar ao dispositivo {device}")
