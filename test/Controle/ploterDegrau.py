import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
from datetime import datetime, timedelta
import os 

FILENAME = os.path.join( os.path.dirname(__file__ ).removesuffix("\\Controle"), 'putty.log' ) 

# Função para ler o arquivo de log e extrair dados
def read_log_data(filename):
    timestamps = []
    setpoints = []
    sensors = []
    actuators = []
    errors = []
    
    with open(filename, 'r') as file:
        try:
            timestamp = datetime.now()  # Inicializa o timestamp
            for line in file:
                if line.strip() == "":
                    continue
                if '[Atuador]:' in line:
                    actuator = float(line.split(':')[1].strip())
                    actuators.append(actuator)
                elif '[Setpoint]:' in line:
                    setpoint = float(line.split(':')[1].strip())
                    setpoints.append(setpoint)
                elif '[Position]:' in line:
                    sensor = float(line.split(':')[1].strip())
                    sensors.append(sensor)
                elif '[dt]:' in line:
                    dt = float(line.split(':')[1].strip())
                    timestamps.append(dt)
        except:
            pass  

    return timestamps, setpoints, sensors, actuators 

# Ler os dados do arquivo de log
timestamps, setpoints, sensors, actuators = read_log_data( FILENAME )

# Ajustar o comprimento das listas
min_length = min(len(setpoints), len(sensors), len(actuators), len(timestamps))
setpoints  = setpoints[:min_length]
sensors    = sensors[:min_length]
actuators  = actuators[:min_length]
timestamps = timestamps[:min_length]  # Ajuste timestamps para a menor lista

# Calcular o tempo relativo a partir do primeiro timestamp
start_time = timestamps[0]
relative_times = [(timestamp - start_time)/1000 for timestamp in timestamps]

actuators = [ i/100 for i in actuators ]
# setpoints = [ i/8192 for i in setpoints ] 

# Criar a figura e os eixos
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

# Plotar os dados de Setpoint e Sensor no primeiro subplot
ax1.plot( relative_times, setpoints, label='Setpoint', color='green', linestyle='-')
ax1.plot(relative_times, sensors, label='Sensor', color='blue', linestyle='-')
ax1.set_title('Malha de controle')
ax1.set_ylabel('Ângulo (º)')
ax1.legend()
ax1.grid(True)

# Plotar os dados de Atuador no segundo subplot
ax2.plot( relative_times, actuators, label='Atuador', color='red', linestyle='-')
ax2.set_title('Variável de atuação')
ax2.set_xlabel('Tempo (s)')
ax2.set_ylabel('Duty Cycle (%)')
ax2.legend()
ax2.grid(True)

# Ajustar o layout
plt.tight_layout()

# Salvar o gráfico em um arquivo
plt.savefig('grafico_dados.png', dpi=600, bbox_inches='tight')

# Mostrar o gráfico
plt.show()
