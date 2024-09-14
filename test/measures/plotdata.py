import matplotlib.pyplot as plt
import numpy as np
import datetime
import csv
import os  


LAT = -29.16530765942215

# Inicializar listas para armazenar os dados
timestamps = {
    "equ_out" : [],
    "sol_inv" : [],
    "equ_pri" : [],
    "sol_ver" : []
    }

zenite     = {
    "equ_out" : [],
    "sol_inv" : [],
    "equ_pri" : [],
    "sol_ver" : []
    }

azimute    = {
    "equ_out" : [],
    "sol_inv" : [],
    "equ_pri" : [],
    "sol_ver" : []
    }
    
# Ler os dados do arquivo CSV
for file_name in timestamps.keys():
    with open(os.path.join( os.path.dirname(__file__), "measures",file_name + '.csv' ), 'r') as csvfile:
    # with open(os.path.join( os.path.dirname(__file__), file_name + '.csv' ), 'r') as csvfile:
        csvreader = csv.reader(csvfile)
        for row in csvreader:
            row = row[0].split(";")
            timestamps[file_name].append( datetime.datetime.strptime( row[0], "%Y-%m-%d %H:%M:%S") )
            zenite[file_name].append(float(row[1]))
            azimute[file_name].append(float(row[2]) + 180 )
            
            
# Criar a figura e os eixos
# fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

# # Plot para Zenite
# ax1.plot( timestamps, zenite, 'b', label = 'Zenite' )
# ax1.set_xlabel('Time')
# ax1.set_ylabel('Zenite(°)')
# ax1.set_title('Zenite')
# ax1.legend()
# ax1.grid(True)

# # Plot para Azimute
# ax2.plot( timestamps, azimute, 'g', label = 'Azimute')
# ax2.set_xlabel('Time')
# ax2.set_ylabel('Azimute(°)')
# ax2.set_title('Azimute')
# ax2.legend()
# ax2.grid(True)


# # Definir os ticks no eixo x para intervalos de 2 segundos
# x_ticks = np.arange(len(timestamps), 2)
# for ax in [ax1, ax2 ]:
#     ax.set_xticks(x_ticks)


# from scipy.signal import savgol_filter
# window_length = 10  # Deve ser ímpar
# polyorder = 4
# for file_name in timestamps.keys():
#     # Aplicar o filtro de Savitzky-Golay
#     # azimute[file_name] = savgol_filter(azimute[file_name], window_length, polyorder)
#     zenite[file_name] = savgol_filter(zenite[file_name], window_length, polyorder)



# Gráfico polar para o zênite
for file_name in timestamps.keys():
    if file_name == 'equ_out':
        label = 'Equinócio de outono [21/03/2024]'
    elif file_name =='sol_inv':
        label = 'Solstício de inverno [20/06/2024]'
    elif file_name == 'equ_pri':
        label = 'Equinócio de primavera [22/09/2024]'
    elif file_name =='sol_ver':
        label = 'Solstício de verão [21/12/2024]'
    plt.polar( np.radians(azimute[file_name]), zenite[file_name], label=label)
    print( max(zenite[file_name]))

plt.gca().set_theta_zero_location('N')  # Opcional: define o zero em 'Norte'
plt.gca().set_theta_direction(-1)  # Opcional: inverte a direção para horário
plt.title("Carta solar para Latitude -29.16530765" )

plt.grid(True)

# Adiciona legendas para os pontos cardeais
plt.gca().set_xticks(np.radians( [0, 45, 90, 135, 180, 225, 270, 315 ]) )
plt.gca().set_xticklabels(['N','', 'L', '', 'S', '', 'O', ''])

plt.tight_layout()
plt.legend(loc='lower right', fancybox=True)
plt.show()
