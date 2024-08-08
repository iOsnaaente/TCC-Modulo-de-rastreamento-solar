import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from scipy.interpolate import interp1d
from datetime import datetime

# Função para ler o arquivo CSV e extrair dados
def read_csv_data(filename):
    timestamps = []
    setpoints = []
    sensors = []
    actuators = []
    errors = []
    
    with open(filename, 'r') as file:
        for line in file:
            if line.strip() == "":
                continue
            parts = line.split(',')
            if len(parts) == 4:
                timestamp_str = parts[0].strip()
                try:
                    timestamp = datetime.strptime(timestamp_str, '%Y-%m-%d %H:%M:%S.%f')
                    sensor = float(parts[1].strip())
                    actuator = float(parts[2].strip())
                    setpoint = float(parts[3].strip())
                    
                    timestamps.append(timestamp)
                    setpoints.append(setpoint/8192)
                    sensors.append(sensor)
                    actuators.append( actuator )
                    errors.append( setpoint-sensor )
                except ValueError as e:
                    print(f"Erro ao processar linha: {line}. Erro: {e}")
    
    return timestamps, setpoints, sensors, actuators, errors 

# Ler os dados do arquivo CSV
timestamps, setpoints, sensors, actuators, errors = read_csv_data('dados_ATUADOR.csv')

# Calcular o tempo relativo a partir do primeiro timestamp
start_time = timestamps[0]
relative_times = [(timestamp - start_time).total_seconds() for timestamp in timestamps]

# Calcular a média móvel de 5 valores
def moving_average(data, window_size):
    return np.convolve(data, np.ones(window_size)/window_size, mode='valid')

window_size = 15
setpoints_smooth = moving_average(setpoints, window_size)
sensors_smooth = moving_average(sensors, window_size)
relative_times_smooth = relative_times[window_size - 1:]

# Interpolação
interp_func_setpoint = interp1d(relative_times_smooth, setpoints_smooth, kind='linear', fill_value="extrapolate")
interp_func_sensor = interp1d(relative_times_smooth, sensors_smooth, kind='linear', fill_value="extrapolate")

# Definir novos pontos para interpolar
new_times = np.linspace(min(relative_times), max(relative_times), num=1000)
setpoints_interpolated = interp_func_setpoint(new_times)
sensors_interpolated = interp_func_sensor(new_times)

# Criar a figura e os eixos
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

# Plotar os dados de Setpoint e Sensor no primeiro subplot
#ax1.plot(new_times, setpoints_interpolated, label='Setpoint', color='blue', linestyle='-')
ax1.plot(new_times, sensors_interpolated, label='Sensor', color='blue', linestyle='-')
ax1.set_title('Malha de controle')
ax1.set_ylabel('Ângulos (º)')
ax1.legend()
ax1.grid(True)

# Plotar os dados de Atuador no segundo subplot
ax2.plot(new_times, setpoints_interpolated, label='Atuador', color='red', linestyle='-')
#ax2.plot(relative_times, errors, label='Erro', color='red', linestyle='-')
ax2.set_title('Variável de controle')
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
