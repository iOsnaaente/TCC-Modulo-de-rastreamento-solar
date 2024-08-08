import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
from datetime import datetime, timedelta
import os 

FILENAME = os.path.join( os.path.dirname(__file__ ), 'putty2.log' ) 

import numpy as np

def moving_average(data, window_size):
    """Aplica um filtro de média móvel aos dados."""
    return np.convolve(data, np.ones(window_size)/window_size, mode='valid')


# Função para ler o arquivo de log e extrair dados
def read_log_data(filename):
    timestamps = []
    pos = []
    vel = []
    acc = []
    setpoints = []
    actuators = []
    with open(filename, 'r') as file:
        timestamp = datetime.now()  # Inicializa o timestamp
        for line in file:
            if '[Atuador]:' in line:
                actuator = float(line.split(':')[1].strip())
                actuators.append(actuator)
            elif '[Setpoint]:' in line:
                setpoint = float(line.split(':')[1].strip())
                setpoints.append(setpoint)
            elif '[Pos]:' in line:
                p = float(line.split(':')[1].strip())
                pos.append(p)
            elif '[Vel]:' in line:
                v = float(line.split(':')[1].strip())
                vel.append(v)
            elif '[Acel]:' in line:
                a = float(line.split(':')[1].strip())
                acc.append(a)
            elif '[dt]:' in line:
                dt = float(line.split(':')[1].strip())
                timestamps.append(dt)
            else: 
                continue
    return timestamps, pos, vel, acc, setpoints, actuators

# Ler os dados do arquivo de log
timestamps, pos, vel, acc, setpoints, actuators = read_log_data( FILENAME )

# Ajustar o comprimento das listas
min_length  = min(len(timestamps), len(pos), len(vel), len(acc), len(setpoints), len(actuators) )
timestamps = timestamps[:min_length]
pos = pos[: min_length]
vel = vel[: min_length]
acc = acc[: min_length]
setpoints = setpoints[: min_length]
actuators = actuators[: min_length]



def smooth_data(data, window_size=10):
    # Inicializa uma cópia dos dados para evitar alterar o original
    smoothed_data = data.copy()
    # Itera sobre os dados, aplicando a operação em cada janela
    for i in range(window_size - 1, len(data) - window_size + 1):
        # Extrai a janela de dados
        window = data[i - window_size + 1 : i + 1]
        # Calcula a média da janela
        window_mean = np.mean(window)
        # Verifica se o segundo valor está abaixo da média
        if window[1] < window_mean:
            # Calcula a média entre o primeiro e o terceiro valor
            smoothed_value = (window[0] + window[2]) / 2
            # Substitui o segundo valor pelo valor calculado
            smoothed_data[i - window_size + 2] = smoothed_value
    return smoothed_data


# Defina o tamanho da janela e o grau do polinômio (por exemplo, 5 e 2)
# window_length = 150  # Deve ser ímpar
# polyorder = 3
from scipy.signal import savgol_filter
# # Aplicar o filtro de Savitzky-Golay
# pos = savgol_filter(pos, window_length, polyorder)


vel[0] = pos[1]-pos[0]
mean = 0
for i in range(len(pos[:-1])):
    a = ((pos[i+1]-pos[i]))/(timestamps[i+1]-timestamps[i])
    mean = a*(0.1) + mean*(1-0.1)
    vel[i+1] = mean

vel = smooth_data( vel )
# vel = savgol_filter(vel, window_length, polyorder)


for i in range(len(vel[:-1])):
    acc[i+1] = ((vel[i+1]-vel[i]))/(timestamps[i+1]-timestamps[i])
acc[0] = vel[1]-vel[0]



# Calcular o tempo relativo a partir do primeiro timestamp
start_time = timestamps[0]
relative_times = [(timestamp - start_time)/1000 for timestamp in timestamps]

actuators = [ 0 if i <=0 else 1 for i in actuators ]
setpoints = [ i/8192 for i in setpoints ] 

# Criar a figura e os eixos
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

# Plotar os dados de Setpoint e Sensor no primeiro subplot
ax1.plot(relative_times, pos, label='Posição', color='blue', linestyle='-')
ax1.set_title('Posição')
ax1.set_ylabel('Ângulo (°)')
ax1.legend()
ax1.grid(True)

# Plotar os dados de Atuador no segundo subplot
ax2.plot( relative_times, setpoints, label='Atuador', color='green', linestyle='-')
ax2.plot( relative_times, actuators, label='Direção', color='red', linestyle='-')
ax2.set_title('Variável de atuação')
ax2.set_xlabel('Tempo (s)')
ax2.set_ylabel('Duty Cycle (%)')
ax2.legend()
ax2.grid(True)

# Plotar os dados de Setpoint e Sensor no primeiro subplot
ax3.plot(relative_times, vel, label='Velocidade', color='blue', linestyle='-')
ax3.plot(relative_times, acc, label='Aceleração', color='green', linestyle='-')
# ax3.plot( range( len( timestamps)), timestamps, label='-', color='blue', linestyle='-')
ax3.set_title('°/ms')
ax3.set_ylabel('Ângulo (°)')
ax3.legend()
ax3.grid(True)



# Ajustar o layout
plt.tight_layout()

# Salvar o gráfico em um arquivo
plt.savefig('grafico_dados.png', dpi=600, bbox_inches='tight')

# Mostrar o gráfico
plt.show()
