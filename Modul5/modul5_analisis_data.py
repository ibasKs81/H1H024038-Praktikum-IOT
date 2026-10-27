import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

# ==================== KONFIGURASI MQTT ====================
BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "unsoed/tk245004/kelompok2/data" # Topik data ESP32
JUMLAH_SAMPEL = 30 # Target jumlah sampel data

data = []

def on_connect(client, userdata, flags, reason_code, properties=None):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu:.2f} °C")

    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# --- Tahap 1: Akuisisi Data dari MQTT ---
print("Memulai akuisisi data dari MQTT...")
try:
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
except AttributeError:
    client = mqtt.Client()

client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# --- Tahap 2: Simpan ke CSV ---
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("\nData berhasil disimpan di data_suhu.csv")

# --- Tahap 3: Analisis Statistik dan Deteksi Anomali (z-score) ---
rata2 = df["suhu"].mean()
std = df["suhu"].std()

if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std

df["anomali"] = df["zscore"].abs() > 2

print("\n--- HASIL ANALISIS STATISTIK DATA SUHU ---")
print(f"Rata-rata       : {rata2:.2f} °C")
print(f"Std deviasi     : {std:.2f}")
print(f"Minimum         : {df['suhu'].min():.2f} °C")
print(f"Maksimum        : {df['suhu'].max():.2f} °C")
print(f"Jumlah anomali  : {df['anomali'].sum()}")

# --- Tahap 4: Visualisasi ---
plt.figure(figsize=(10, 4))
plt.plot(df.index, df["suhu"], marker="o", label="Suhu Mentah", color="#1f77b4")
anom = df[df["anomali"]]
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali (|z| > 2)", zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")

plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Data Suhu dan Deteksi Anomali Menggunakan Z-Score")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png", dpi=300)
print("Grafik berhasil disimpan sebagai grafik_suhu.png")
plt.show()
