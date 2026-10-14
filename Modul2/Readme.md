# Laporan Praktikum Modul 2: Konfigurasi Jaringan dan Komunikasi IoT[cite: 3, 14]

## 1. Detail Percobaan
Praktikum ini bertujuan untuk memahami dan mengimplementasikan konfigurasi jaringan nirkabel serta protokol komunikasi pada perangkat IoT[cite: 3, 14]:
* **Percobaan 1 (Konfigurasi WiFi Mode Station - STA):** Mengonfigurasi mikrokontroler agar terhubung ke jaringan WiFi lokal (SSID: `Hazee`)[cite: 10, 12], menampilkan informasi parameter jaringan seperti IP Address, MAC Address, dan kekuatan sinyal (RSSI) pada Serial Monitor[cite: 5, 10, 16, 12], serta mengontrol indikator LED berdasarkan status koneksi[cite: 6, 10, 17, 12].
* **Percobaan 2 (Komunikasi MQTT & JSON):** Mengimplementasikan protokol komunikasi MQTT untuk pengiriman (*publish*) data sensor dummy (suhu dan kelembapan) berformat JSON[cite: 11, 13] ke broker MQTT HiveMQ (`broker.hivemq.com`) secara *non-blocking*[cite: 11, 13], serta menerima (*subscribe*) perintah kontrol aktuator (LED) dari broker secara *real-time*[cite: 11, 13].

## 2. Library atau Dependencies
Beberapa pustaka (*library*) yang digunakan pada percobaan ini meliputi[cite: 4, 11, 15, 13]:
* **`ESP8266WiFi.h` / `WiFi.h`**: Digunakan untuk mengatur koneksi dan konfigurasi mode jaringan WiFi pada mikrokontroler[cite: 4, 10, 15, 12].
* **`PubSubClient.h`**: Digunakan untuk mengelola koneksi, publish, dan subscribe pada protokol MQTT[cite: 11, 13].
* **`ArduinoJson.h`**: Digunakan untuk pembuatan (*serialization*) dan pembacaan (*deserialization*) data berformat JSON[cite: 11, 13].

## 3. Skematik dan Rangkaian

### Konfigurasi Pin
Tabel pemetaan pin komponen yang digunakan pada rangkaian:

| Komponen | Pin Mikrokontroler | Keterangan |
| :--- | :--- | :--- |
| **LED Indikator / Aktuator** | GPIO 4 / D2[cite: 11, 13] | Terhubung ke Anoda LED melalui Resistor 220 Ω[cite: 5, 16] |
| **Resistor** | 220 Ohm[cite: 5, 16] | Pembatas arus listrik untuk LED[cite: 5, 16] |
| **GND** | GND | Terhubung ke Katoda LED |

### Skematik Rangkaian
*(Tambahkan gambar skematik rangkaian kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Skematik Rangkaian](./Dokum/skematik.png)

*Deskripsi Ringkas:* Pin GPIO 4 / D2 dihubungkan ke kaki Anoda LED melalui resistor 220 Ohm[cite: 5, 11, 16, 13], sedangkan kaki Katoda LED dihubungkan langsung ke jalur Ground (GND) mikrokontroler[cite: 6, 17].

### Foto Proses Praktikum
*(Tambahkan foto dokumentasi praktikum kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Foto Praktikum](./Dokum/foto_praktikum.png)

---

## 4. JAWABAN SOAL (dalam modul 2.7 Pertanyaan Praktikum)[cite: 8, 19]

### 1. Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan![cite: 8, 19]
* **Percobaan 1:** Perangkat berhasil terhubung ke jaringan WiFi lokal dengan SSID `Hazee`[cite: 10, 12]. Serial Monitor menampilkan alokasi IP Address lokal, alamat MAC perangkat, serta nilai kuat sinyal (RSSI)[cite: 5, 10, 16, 12]. LED indikator menyala saat status jaringan menunjukkan `WL_CONNECTED`[cite: 5, 10, 16, 12].
* **Percobaan 2:** Perangkat terhubung ke jaringan WiFi dan berhasil menjalin koneksi ke broker MQTT HiveMQ (`broker.hivemq.com:1883`)[cite: 11, 13]. Data dummy suhu dan kelembapan terkirim otomatis berformat JSON ke topik `unsoed/tk245004/kelompok2/data` setiap 5 detik[cite: 11, 13]. Selain itu, perangkat dapat menerima perintah JSON dari topik `unsoed/tk245004/kelompok2/perintah` untuk menyalakan atau mematikan LED[cite: 11, 13].

### 2. Bagaimana pengaruh kekuatan sinyal (RSSI) terhadap kestabilan koneksi WiFi pada perangkat IoT?[cite: 8, 19]
* Sinyal **RSSI (*Received Signal Strength Indicator*)** diukur dalam satuan dBm (bernilai negatif)[cite: 4, 15].
* Semakin mendekati 0 dBm (misalnya -30 dBm hingga -60 dBm), kekuatan sinyal tergolong sangat baik dan koneksi data relatif stabil.
* Apabila RSSI bernilai terlalu kecil (misalnya di bawah -80 dBm), transmisi paket data akan mengalami *delay* (latensi tinggi), *packet loss*, hingga berpotensi menyebabkan koneksi terputus (*disconnection*).

### 3. Bagaimana cara kerja ESP32 dalam membedakan peran sebagai klien (Station) dan sebagai penyedia jaringan (Access Point)?[cite: 8, 19]
* ESP32 membedakan peran jaringan menggunakan konfigurasi mode operasi pada lapisan stack Wi-Fi melalui fungsi `WiFi.mode()`[cite: 3, 5, 14, 16]:
  * **Mode Station (STA):** Perangkat bertindak sebagai *Station/Client* yang melakukan proses pemindaian (*scanning*) beacon frame jaringan WiFi lain, kemudian mengirimkan permintaan *association* dan *DHCP Request* untuk mendapatkan alokasi IP[cite: 3, 14].
  * **Mode Access Point (AP):** Perangkat bertindak sebagai *SoftAP/Host* yang memancarkan beacon frame jaringan dengan SSID-nya sendiri dan menjalankan server DHCP internal (secara default mengalokasikan IP `192.168.4.1`)[cite: 3, 8, 14, 19] untuk membagikan IP address kepada perangkat lain yang mencoba terhubung[cite: 3, 14].

### 4. Bagaimana kombinasi mode Station dan Access Point (AP+STA) dapat dimanfaatkan dalam skenario nyata sistem IoT, misalnya pada proses konfigurasi awal perangkat (provisioning)?[cite: 8, 19]
* Mode kombinasi **AP+STA** memungkinan ESP32 menjalankan fungsi *Host* dan *Client* secara bersamaan[cite: 4, 15].
* **Skenario Provisioning:**
  1. Saat perangkat IoT pertama kali dinyalakan (belum mengantongi kredensial WiFi rumah/kantor), ESP32 akan aktif dalam mode **AP** memancarkan SSID konfigurasi[cite: 3, 14].
  2. Pengguna menghubungkan smartphone ke jaringan AP ESP32 tersebut dan membuka halaman web internal (*captive portal*) untuk memasukkan SSID dan Password WiFi lokasi.
  3. Menggunakan fungsi **STA**, ESP32 akan mencoba terhubung ke WiFi utama sambil tetap mempertahankan koneksi **AP** ke smartphone.
  4. Apabila koneksi STA berhasil, ESP32 menyimpan data kredensial WiFi ke dalam memori non-volatile (NVS/EEPROM) dan dapat mematikan mode AP secara otomatis untuk menghemat daya.