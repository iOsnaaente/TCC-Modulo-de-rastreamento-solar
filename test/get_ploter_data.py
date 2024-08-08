import serial
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.animation import FuncAnimation

# Configurações da porta serial
SERIAL_PORT = 'COM5'  # Substitua pelo nome da sua porta serial
BAUD_RATE = 115200
SERIAL_TIMEOUT = 1

# Configurações do arquivo CSV
CSV_FILE = 'dados.csv'

# Inicializa a conexão serial
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=SERIAL_TIMEOUT)

# Listas para armazenar os dados
timestamps = []
setpoints = []
positions = []
actuators = []



# Função para ler e processar os dados da serial
def read_serial_data():
    global setpoint, position, actuator
    try:
        while ser.in_waiting:
            line = ser.readline().decode().strip()
            if line.startswith('[Setpoint]:'):
                setpoint = float(line.split(':')[1].strip())
            elif line.startswith('[Position]:'):
                position = float(line.split(':')[1].strip())
            elif line.startswith('[Atuador]:'):
                actuator = float(line.split(':')[1].strip())
                if setpoint is not None and position is not None:
                    if len(setpoints) < len(positions) or len(setpoints) < len(actuators):
                        # Completa os dados faltantes
                        if len(setpoints) < len(positions):
                            setpoints.append(setpoint)
                        if len(setpoints) < len(actuators):
                            setpoints.append(setpoint)
                        if len(positions) < len(setpoints):
                            positions.append(position)
                        if len(actuators) < len(setpoints):
                            actuators.append(actuator)
                    else:
                        # Adiciona os dados completos
                        setpoints.append(setpoint)
                        positions.append(position)
                        actuators.append(actuator)
                        timestamps.append(pd.Timestamp.now())
                    # Reinicializa as variáveis temporárias
                    setpoint = None
                    position = None
                    actuator = None
    except Exception as e:
        print(f"Erro ao ler dados da serial: {e}")


# Função para atualizar o gráfico
def update(frame):
    read_serial_data()
    if timestamps:
        ax1.clear()
        ax2.clear()
        ax3.clear()
        
        ax1.plot(timestamps, setpoints, label='Setpoint')
        ax2.plot(timestamps, positions, label='Position')
        ax3.plot(timestamps, actuators, label='Actuator')
        
        ax1.set_title('Setpoint')
        ax2.set_title('Position')
        ax3.set_title('Actuator')
        
        for ax in [ax1, ax2, ax3]:
            ax.legend()
            ax.grid(True)
        
        # Salva os dados em CSV
        df = pd.DataFrame({
            'Timestamp': timestamps,
            'Sensor': positions,
            'Atuador': actuators,
            'Setpoint': setpoints
        })
        df.to_csv(CSV_FILE, index=False)

# Configuração do gráfico
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8))
ani = FuncAnimation(fig, update, interval = 1 )  # Atualiza o gráfico a cada 1 segundo

plt.tight_layout()
plt.show()
