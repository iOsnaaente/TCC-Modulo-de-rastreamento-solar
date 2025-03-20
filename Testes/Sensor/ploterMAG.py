import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
from datetime import datetime, timedelta
import os 

# FILENAME = os.path.join( os.path.dirname(__file__ ), 'dadosI2C.log' ) 
FILENAME = os.path.join( os.path.dirname(__file__ ), 'log_putty.log' ) 

# Função para ler o arquivo de log e extrair dados
def read_log_data(filename):
    timestamps = []
    actuator = []
    acum = []
    pos = []
    raw = []
    
    with open(filename, 'r') as file:
        timestamp = datetime.now()  # Inicializa o timestamp
        for line in file:
            values = line.split( "," ) 
            for value in values:
                if line.strip() == "":
                    continue
                if 'Pos:' in value:
                    p = float(value.split(':')[1].strip())
                    pos.append(p)
                elif 'Raw:' in value:
                    r = float(value.split(':')[1].strip())
                    raw.append(r)
                elif 'Acum:' in value:
                    ac = float(value.split(':')[1].strip())
                    acum.append(ac)
                elif 'Atuador:' in value:
                    at = float(value.split(':')[1].strip())
                    actuator.append(at)
                elif 'dt:' in value:
                    dt = float(value.split(':')[1].strip())
                    timestamps.append(dt)
    return timestamps, pos, raw, acum, actuator 

# Ler os dados do arquivo de log
timestamps, pos, raw, acum, actuator = read_log_data( FILENAME )

# Ajustar o comprimento das listas
min_length  = min(len(pos), len(raw), len(acum), len(actuator), len(timestamps))//2
pos         = pos[:min_length]
raw         = raw[:min_length]

acum        = acum[:min_length]
acum_off = acum[0]
acum_max = max(acum) - acum_off 
acum = [ ((a-acum_off)/acum_max)*360 for a in acum ]

actuator    = actuator[:min_length]  
timestamps  = timestamps[:min_length]  

# Calcular o tempo relativo a partir do primeiro timestamp
start_time = timestamps[0]
relative_times = [(i - start_time)/1000 for i in timestamps ]

# actuators = [ i/100 for i in actuators ]
actuator = [ i/8192 for i in actuator ] 

# Criar a figura e os eixos
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8), sharex=True)


# Plotar os dados de Atuador no segundo subplot
ax1.plot( relative_times, acum, label='Posição', color='green', linestyle='-')
ax1.set_title('Posição')
ax1.set_ylabel('Ângulo (º)')
ax1.set_yticks( [0, 90, 180, 270, 360 ]  )
ax1.legend()
ax1.grid(True)

# Plotar os dados de Setpoint e Sensor no primeiro subplot
ax2.plot( relative_times, pos, label='Sensor', color='blue', linestyle='-')
ax2.set_title('Saída do sensor')
ax2.set_ylabel('Ângulo (º)')
ax2.set_yticks( [0, 90, 180, 270, 360 ]  )
ax2.legend()
ax2.grid(True)

# Plotar os dados de Atuador no segundo subplot
ax3.plot( relative_times, actuator, label='Atuador', color='red', linestyle='-')
ax3.set_title('Variável de atuação')
ax3.set_xlabel('Tempo (s)')
ax3.set_ylabel('Duty Cycle (%)')
ax3.legend()
ax3.grid(True)

# Ajustar o layout
plt.tight_layout()

# Salvar o gráfico em um arquivo
plt.savefig('grafico_dados.png', dpi=600, bbox_inches='tight')

# Mostrar o gráfico
plt.show()
