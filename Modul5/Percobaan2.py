"""
Program Percobaan 5B & Modifikasi Analisis Data Sensor IoT
Modul 5: Pengolahan dan Analisis Data (MQTT -> CSV -> Moving Average -> Z-Score Anomaly Detection -> Visualisasi)
"""
import os
import json
import time
import pandas as pd
import matplotlib.pyplot as plt

# File paths
CSV_FILE = "data_suhu.csv"
OUTPUT_PLOT = "grafik_suhu.png"
OUTPUT_PLOT_MODIF = "grafik_suhu_modifikasi.png"

# ==============================================================================
# FUNGSI PENGOLAHAN DAN ANALISIS STATISTIK DATA SUHU
# ==============================================================================
def analisis_data(file_csv=CSV_FILE):
    if not os.path.exists(file_csv):
        print(f"File {file_csv} tidak ditemukan!")
        return

    # Membaca data historis dari file CSV
    df = pd.read_csv(file_csv)
    print(f"Memuat {len(df)} baris data dari {file_csv}...\n")

    # --- 1. Analisis Z-Score pada Data Mentah ---
    rata2_mentah = df["suhu"].mean()
    std_mentah = df["suhu"].std()
    df["zscore_mentah"] = 0.0 if std_mentah == 0 else (df["suhu"] - rata2_mentah) / std_mentah
    df["anomali_mentah"] = df["zscore_mentah"].abs() > 2

    print("=== [A] HASIL ANALISIS DATA MENTAH ===")
    print(f"Rata-rata       : {rata2_mentah:.2f} °C")
    print(f"Std Deviasi     : {std_mentah:.2f}")
    print(f"Nilai Minimum   : {df['suhu'].min():.2f} °C")
    print(f"Nilai Maksimum  : {df['suhu'].max():.2f} °C")
    print(f"Jumlah Anomali  : {df['anomali_mentah'].sum()} data")
    print("Indeks Anomali  :", list(df[df["anomali_mentah"]].index))
    print()

    # --- 2. Analisis Z-Score setelah Moving Average (Jendela 5 Data) [TUGAS MODIFIKASI] ---
    # Menghitung moving average 5 data dengan min_periods=1 agar data awal tidak bernilai NaN
    df["suhu_ma5"] = df["suhu"].rolling(window=5, min_periods=1).mean()
    rata2_ma5 = df["suhu_ma5"].mean()
    std_ma5 = df["suhu_ma5"].std()
    df["zscore_ma5"] = 0.0 if std_ma5 == 0 else (df["suhu_ma5"] - rata2_ma5) / std_ma5
    df["anomali_ma5"] = df["zscore_ma5"].abs() > 2

    print("=== [B] HASIL ANALISIS DENGAN MOVING AVERAGE (JENDELA 5) ===")
    print(f"Rata-rata (MA5) : {rata2_ma5:.2f} °C")
    print(f"Std Deviasi(MA5): {std_ma5:.2f}")
    print(f"Nilai Minimum   : {df['suhu_ma5'].min():.2f} °C")
    print(f"Nilai Maksimum  : {df['suhu_ma5'].max():.2f} °C")
    print(f"Jumlah Anomali  : {df['anomali_ma5'].sum()} data")
    print("Indeks Anomali  :", list(df[df["anomali_ma5"]].index))
    print()

    # --- 3. Visualisasi Grafik Utama (grafik_suhu.png) ---
    plt.figure(figsize=(10, 4.5))
    plt.plot(df.index, df["suhu"], marker="o", color="#1f77b4", linewidth=1.5, label="Suhu Mentah (°C)")
    anom_mentah = df[df["anomali_mentah"]]
    plt.scatter(anom_mentah.index, anom_mentah["suhu"], color="red", s=110, zorder=4, label="Anomali (|z| > 2)")
    plt.axhline(rata2_mentah, color="gray", linestyle="--", label=f"Rata-rata ({rata2_mentah:.2f} °C)")

    plt.xlabel("Urutan Data Ke-")
    plt.ylabel("Suhu (°C)")
    plt.title("Data Suhu dan Deteksi Anomali Menggunakan Z-Score (Percobaan 5B)")
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend(loc="upper left")
    plt.tight_layout()
    plt.savefig(OUTPUT_PLOT, dpi=300)
    plt.close()
    print(f"Grafik utama berhasil disimpan ke '{OUTPUT_PLOT}'")

    # --- 4. Visualisasi Grafik Perbandingan (grafik_suhu_modifikasi.png) ---
    plt.figure(figsize=(11, 5))
    plt.plot(df.index, df["suhu"], marker="o", alpha=0.5, color="#7f7f7f", linestyle=":", label="Suhu Mentah")
    plt.plot(df.index, df["suhu_ma5"], marker="s", color="#2ca02c", linewidth=2, label="Moving Average (Jendela 5)")
    anom_ma5 = df[df["anomali_ma5"]]
    if not anom_ma5.empty:
        plt.scatter(anom_ma5.index, anom_ma5["suhu_ma5"], color="purple", s=120, zorder=5, label="Anomali Setelah MA5")
    plt.axhline(rata2_ma5, color="gray", linestyle="--", label=f"Rata-rata MA5 ({rata2_ma5:.2f} °C)")

    plt.xlabel("Urutan Data Ke-")
    plt.ylabel("Suhu (°C)")
    plt.title("Perbandingan Efek Filter Moving Average terhadap Deteksi Anomali Z-Score")
    plt.grid(True, linestyle=":", alpha=0.6)
    plt.legend(loc="upper left")
    plt.tight_layout()
    plt.savefig(OUTPUT_PLOT_MODIF, dpi=300)
    plt.close()
    print(f"Grafik perbandingan berhasil disimpan ke '{OUTPUT_PLOT_MODIF}'")

if __name__ == "__main__":
    analisis_data()
