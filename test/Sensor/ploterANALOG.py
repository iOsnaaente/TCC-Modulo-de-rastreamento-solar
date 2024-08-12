import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
from datetime import datetime, timedelta
import os 

FILENAME = os.path.join( os.path.dirname(__file__ ), 'puttyANALOG.log' ) 

# Função para ler o arquivo de log e extrair dados
def read_log_data(filename):
    timestamps = []
    pulses = []
    sensor = []
    actuator = []
    with open(filename, 'r') as file:
        for line in file:
            if line.strip() == "":
                continue
            line = line.split(",")
            
            # Sensor:0,Atuador:0.000000,Pulses:0.000000,dt:93
            for i in line:
                if 'Sensor:' in i:
                    s = float( i.split(':')[1].strip())
                    sensor.append(s)
                elif 'Atuador:' in i:
                    a = float(i.split(':')[1].strip())
                    actuator.append(a)
                elif 'Pulses:' in i:
                    p = float( i.split(':')[1].strip())
                    pulses.append(p)
                elif 'dt:' in i:
                    dt = float(i.split(':')[1].strip())
                    timestamps.append(dt)
    return timestamps, sensor, actuator, pulses 


# Ler os dados do arquivo de log
timestamps, sensor, pulses, actuator = read_log_data( FILENAME )

# Ajustar o comprimento das listas
min_length  = min(len(pulses), len(actuator), len(timestamps), len(sensor))
sensor      = sensor[:min_length] 
pulses      = pulses[:min_length]
actuator    = actuator[:min_length]  
timestamps  = timestamps[:min_length]  


sensor = [ i/(2**13) for i in sensor ]
actuator = [ i/max(actuator) for i in actuator ]

# Calcular o tempo relativo a partir do primeiro timestamp
start_time = timestamps[0]
relative_times = [(i - start_time)/1000 for i in timestamps ]

# Defina o tamanho da janela e o grau do polinômio (por exemplo, 5 e 2)
window_length = 25  # Deve ser ímpar
polyorder = 5
from scipy.signal import savgol_filter
# Aplicar o filtro de Savitzky-Golay
# pulses = savgol_filter(pulses, window_length, polyorder)

# Criar a figura e os eixos
fig, (ax1) = plt.subplots(1, 1, figsize=(10, 8), sharex=True)

# Plotar os dados de Atuador no segundo subplot
# ax1.plot( relative_times, pulsesDEG, label='Posição', color='green', linestyle='-')
ax1.plot( relative_times, actuator, label='Pulsos', color='blue', linestyle='-')
ax1.plot( relative_times, pulses, label='Sensor', color='red', linestyle='-')
ax1.set_title('Sensor Analogico')
ax1.set_xlabel('Tempo (s)')
ax1.set_ylabel('Sensor (13 bits)')
ax1.legend()
ax1.grid(True)

# Definir os ticks no eixo x para intervalos de 2 segundos
x_ticks = np.arange(min(relative_times), max(relative_times) + 2, 2)
for ax in [ ax1 ]:
    ax.set_xticks(x_ticks)

# Ajustar o layout
plt.tight_layout()

# Salvar o gráfico em um arquivo
plt.savefig('grafico_dados.png', dpi=600, bbox_inches='tight')

# Mostrar o gráfico
plt.show()
