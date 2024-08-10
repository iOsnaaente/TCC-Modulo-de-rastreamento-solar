import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
from datetime import datetime, timedelta
import os 

FILENAME = os.path.join( os.path.dirname(__file__ ), 'puttyDEG.log' ) 

# Função para ler o arquivo de log e extrair dados
def read_log_data(filename):
    timestamps = []
    pulses = []
    actuator = []
    with open(filename, 'r') as file:
        for line in file:
            if line.strip() == "":
                continue
            line = line.split(",")
            for i in line:
                if 'Pulses:' in i:
                    p = float( i.split(':')[1].strip())
                    pulses.append(p)
                elif 'Atuador:' in i:
                    a = float(i.split(':')[1].strip())
                    actuator.append(a)
                elif 'dt:' in i:
                    dt = float(i.split(':')[1].strip())
                    timestamps.append(dt)
    return timestamps, pulses, actuator 


# Ler os dados do arquivo de log
timestamps, pulses, actuator = read_log_data( FILENAME )

# Ajustar o comprimento das listas
min_length  = min(len(pulses), len(actuator), len(timestamps))
pulsesDEG      = pulses[:8259]
actuatorDEG    = actuator[:8259]  
timestampsDEG  = timestamps[:8259]  


FILENAME = os.path.join( os.path.dirname(__file__ ), 'puttySMO.log' ) 
timestamps, pulses, actuator = read_log_data( FILENAME )
min_length  = min(len(pulses), len(actuator), len(timestamps))
pulsesSMO      = pulses[:min_length]
actuatorSMO    = actuator[:min_length]  
timestampsSMO  = timestamps[:min_length]  

print( len(timestampsDEG), len(timestampsSMO))

# Calcular o tempo relativo a partir do primeiro timestamp
start_time = timestamps[0]
relative_times = [(i - start_time)/1000 for i in timestampsDEG ]

# actuators = [ i/100 for i in actuators ]
# actuator = [ i/8192 for i in actuator ] 

# Defina o tamanho da janela e o grau do polinômio (por exemplo, 5 e 2)
window_length = 25  # Deve ser ímpar
polyorder = 5
from scipy.signal import savgol_filter
# Aplicar o filtro de Savitzky-Golay
pulsesSMO = savgol_filter(pulsesSMO, window_length, polyorder)
pulsesDEG = savgol_filter(pulsesDEG, window_length, polyorder)


# Criar a figura e os eixos
fig, axs = plt.subplots(2, 2, figsize=(10, 8), sharex=True)

# Descompactar os eixos
ax21, ax11, ax22, ax12 = axs.flatten()

# Plotar os dados de Atuador no segundo subplot
# ax1.plot( relative_times, pulsesDEG, label='Posição', color='green', linestyle='-')
ax11.plot( relative_times, pulsesSMO, label='Posição', color='green', linestyle='-')
ax11.set_title('Pulsos')
ax11.set_ylabel('Pulsos')
ax11.legend()
ax11.grid(True)

# Plotar os dados de Atuador no segundo subplot
# ax2.plot( relative_times, actuatorDEG, label='Atuador', color='red', linestyle='-')
ax12.plot( relative_times, actuatorSMO, label='Atuador', color='red', linestyle='-')
ax12.set_title('Variável de atuação')
ax12.set_xlabel('Tempo (s)')
ax12.set_ylabel('Duty Cycle (%)')
ax12.legend()
ax12.grid(True)


# Plotar os dados de Atuador no segundo subplot
ax21.plot( relative_times, pulsesDEG, label='Posição', color='green', linestyle='-')
# ax21.plot( relative_times, pulsesSMO, label='Posição', color='green', linestyle='-')
ax21.set_title('Pulsos')
ax21.set_ylabel('Pulsos')
ax21.legend()
ax21.grid(True)

# Plotar os dados de Atuador no segundo subplot
ax22.plot( relative_times, actuatorDEG, label='Atuador', color='red', linestyle='-')
# ax22.plot( relative_times, actuatorSMO, label='Atuador', color='red', linestyle='-')
ax22.set_title('Variável de atuação')
ax22.set_xlabel('Tempo (s)')
ax22.set_ylabel('Duty Cycle (%)')
ax22.legend()
ax22.grid(True)


# Definir os ticks no eixo x para intervalos de 2 segundos
x_ticks = np.arange(min(relative_times), max(relative_times) + 2, 2)
for ax in [ax11, ax12, ax21, ax22]:
    ax.set_xticks(x_ticks)

# Ajustar o layout
plt.tight_layout()

# Salvar o gráfico em um arquivo
plt.savefig('grafico_dados.png', dpi=600, bbox_inches='tight')

# Mostrar o gráfico
plt.show()
