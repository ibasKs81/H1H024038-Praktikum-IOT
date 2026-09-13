# Laporan Praktikum Modul 1: Akuisisi Data Sensor dan Kendali Aktuator

**Nama:** Ibnu Abbas  
**NIM:** H1H024038  
**Mata Kuliah:** Praktikum Internet of think 

---

## 1. Detail Percobaan
Praktikum ini bertujuan untuk memahami dan mengimplementasikan proses akuisisi data fisik berupa suhu dan kelembaban menggunakan sensor **DHT22** dengan mikrokontroler **ESP32**. Selain itu, data yang diakuisisi digunakan untuk mengontrol aktuator berupa **Relay 1-Channel** (disimulasikan dengan LED) secara otomatis berdasarkan ambang batas (threshold) suhu yang telah ditentukan. 

Percobaan ini dibagi menjadi dua bagian:
1. **Percobaan 1A:** Akuisisi data suhu dan kelembaban dengan sensor DHT22, lalu menampilkan rata-rata 5 pembacaan ke *Serial Monitor*.
2. **Percobaan 2A:** Kendali aktuator menggunakan sistem *histerisis* (dua ambang batas) untuk menyalakan dan mematikan relay/LED berdasarkan data suhu dari DHT22.

---

## 2. Library atau Dependencies
Untuk menjalankan program ini, diperlukan library berikut pada Arduino IDE:
* **`DHT sensor library` (oleh Adafruit):** Pustaka ini digunakan untuk membaca data suhu dan kelembaban dari sensor DHT22.
* **`ESP32 Board Manager`:** Diperlukan untuk melakukan proses *compile* dan *upload* kode ke mikrokontroler ESP32 DevKit.

---

## 3. Skematik dan Rangkaian

### Konfigurasi Pin
Tabel konfigurasi kabel pada percobaan ini:

| Komponen | Pin Modul | Pin ESP32 |
| :--- | :--- | :--- |
| **DHT22** | VCC | 3.3V |
| | DATA | GPIO 4 |
| | GND | GND |
| **Relay / LED** | IN / Anoda | GPIO 26 (menggunakan resistor 220 Ohm jika LED) |
| | VCC | 5V / VIN |
| | GND / Katoda| GND |

### Skematik Rangkaian
![Skematik Rangkaian ESP32, DHT22, dan Relay](link_gambar_skematik_anda.png)  
*(Ganti tautan di atas dengan lokasi file gambar skematik atau foto *wiring* Anda)*

### Foto Proses Praktikum
![Foto Praktikum](Dokum)

---
## 4. JAWABAN SOAL
### Flowchart proses akuisisi data sensor DHT22! 
![Flowchart] (Flowchart)
### Source Code
```cpp
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("Memulai akuisisi data sensor DHT22 (Rata-rata 5 pembacaan)...");
}

void loop() {
  float sumSuhu = 0;
  float sumKelembaban = 0;
  int validReadings = 0;

  // Mengambil 5 kali pembacaan data
  for (int i = 0; i < 5; i++) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    
    // Validasi pembacaan
    if (!isnan(h) && !isnan(t)) {
      sumKelembaban += h;
      sumSuhu += t;
      validReadings++;
    } else {
      Serial.println("Gagal membaca data dari sensor pada iterasi ini!");
    }
    delay(2000); // Jeda 2 detik per pembacaan agar sensor stabil
  }

  // Menghitung dan menampilkan rata-rata jika ada data valid
  if (validReadings > 0) {
    float avgSuhu = sumSuhu / validReadings;
    float avgKelembaban = sumKelembaban / validReadings;
    Serial.print("Rata-rata Suhu: ");
    Serial.print(avgSuhu);
    Serial.print(" °C, Rata-rata Kelembaban: ");
    Serial.print(avgKelembaban);
    Serial.println("%");
  } else {
    Serial.println("Gagal membaca data dari sensor DHT22 selama 5 kali percobaan!");
  }
}
