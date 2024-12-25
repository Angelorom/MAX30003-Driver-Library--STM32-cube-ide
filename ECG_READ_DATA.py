# importing libraries
import numpy as np
import time
import matplotlib.pyplot as plt
import serial

serialPort = '/dev/ttyACM0'  # Porta seriale

def readSerialData(ser):
    try:
        # Legge i dati dalla seriale
        data = ser.readline().decode('utf-8').strip()
        data = int(data)
        return data
    except serial.SerialException as e:
        print(f"Errore nella lettura dei dati: {e}")
        exit()

# Configura la connessione seriale
try:
    ser = serial.Serial(serialPort, 115200)
except serial.SerialException as e:
    print(f"Errore nella connessione seriale: {e}")
    exit()

# Calcolo della frequenza di invio dei dati
print("Calcolo della frequenza di invio dei dati...")
prev_time = None
frequencies = []
num_samples = 50  # Numero di campioni da usare per stimare la frequenza

for _ in range(num_samples):
    start_time = time.time()
    newy = readSerialData(ser)  # Legge un dato
    if prev_time is not None:
        period = start_time - prev_time
        freq = 1 / period if period > 0 else 0
        frequencies.append(freq)
    prev_time = start_time

# Calcola la frequenza media
sampling_rate = sum(frequencies) / len(frequencies)
print(f"Frequenza stimata: {sampling_rate:.2f} Hz")
sampling_period = 1 / sampling_rate  # Periodo di campionamento

# Creazione dei dati iniziali per il grafico
historyLength = 5  # Secondi di dati visualizzati
data = np.zeros(int(historyLength * sampling_rate))

x = np.linspace(0, historyLength, int(historyLength * sampling_rate))
y = data

# Avvio della modalità interattiva di Matplotlib
plt.ion()

# Creazione del grafico
figure, ax = plt.subplots(figsize=(10, 8))
line1, = ax.plot(x, y)

# Impostazioni del grafico
plt.title("Serial data", fontsize=20)
plt.xlabel("Tempo (s)")
plt.ylabel("Valore")

# Loop principale
print("Avvio lettura dati...")
while True:
    # Legge un nuovo dato dalla seriale
    newy = readSerialData(ser)

    # Aggiorna il buffer dei dati
    data = np.roll(data, -1)
    data[-1] = newy

    # Aggiorna il grafico
    line1.set_xdata(x)
    line1.set_ydata(data)
    figure.canvas.draw()
    figure.canvas.flush_events()

    # Aggiorna i limiti degli assi
    ax.relim()
    ax.autoscale_view()

    # Aspetta il periodo di campionamento
    time.sleep(sampling_period)
