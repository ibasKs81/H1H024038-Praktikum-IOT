# Laporan Praktikum Modul 3: Protokol Komunikasi IoT (HTTP & MQTT dengan Format JSON)[cite: 21]

## 1. Detail Percobaan
Praktikum ini bertujuan untuk memahami dan mengimplementasikan protokol komunikasi data pada perangkat IoT menggunakan format data JSON (*JavaScript Object Notation*)[cite: 21, 22]:
* **Percobaan 3A (Komunikasi Data Menggunakan HTTP):** Mengonfigurasi ESP32 untuk mengirimkan data sensor dummy (suhu 28.5 °C dan kelembaban 65.0%) berformat JSON ke endpoint server uji (`https://httpbin.org/post`) menggunakan protokol HTTP metode POST[cite: 23, 24]. Serial Monitor menampilkan kode response HTTP (misal: 200 OK) beserta balasan (*response body/echo*) dari server[cite: 23, 25].
* **Percobaan 3B (Komunikasi MQTT):** Mengimplementasikan pengiriman data (*publish*) berformat JSON ke broker MQTT publik (`broker.hivemq.com:1883`) secara berkala (setiap 5 detik) pada topik tertentu (misal: `unsoed/tk245004/kelompok2/sensor`)[cite: 23, 26, 27]. Data yang dikirim kemudian diverifikasi menggunakan aplikasi client MQTT (seperti MQTT Explorer atau HiveMQ Web Client)[cite: 22, 23, 27].

## 2. Library atau Dependencies
Beberapa pustaka (*library*) yang digunakan pada percobaan ini meliputi[cite: 21, 22, 23]:
* **`WiFi.h` / `ESP8266WiFi.h`**: Mengelola koneksi dan konfigurasi jaringan WiFi pada mikrokontroler[cite: 21, 23, 25].
* **`HTTPClient.h`**: Menyediakan fungsi untuk melakukan permintaan (*request*) HTTP seperti GET dan POST ke server web[cite: 23].
* **`PubSubClient.h`**: Mengelola komunikasi berbasis protokol MQTT (koneksi ke broker, *publish*, dan *subscribe*)[cite: 22, 25].
* **`ArduinoJson.h`**: Mengelola pembuatan, pembacaan, dan serialisasi data berformat JSON[cite: 22].

## 3. Skematik dan Rangkaian

### Konfigurasi Pin
Tabel pemetaan koneksi komunikasi dan perangkat pada praktikum Modul 3:

| Komponen / Jalur | Pin Mikrokontroler | Keterangan |
| :--- | :--- | :--- |
| **Kabel USB Data** | Port Micro-USB / USB-C[cite: 23] | Komunikasi Serial (115200 baud)[cite: 23, 26] dan Daya ESP32 |
| **Modul WiFi Internal** | Built-in ESP32[cite: 21] | Jalur komunikasi nirkabel HTTP & MQTT[cite: 21, 23, 26] |
| **LED Indikator (Opsional)**| GPIO 2 / GPIO 4[cite: 23, 24] | Indikator status koneksi/aktuator melalui Resistor 220 Ω |
| **GND** | GND | Ground bersama rangkaian |

### Skematik Rangkaian
*(Tambahkan gambar skematik rangkaian kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Skematik Rangkaian Modul 3](./Dokum/skematik_modul3.png)

*Deskripsi Ringkas:* ESP32 terhubung ke komputer melalui kabel USB untuk pengiriman program dan monitoring data Serial[cite: 23]. Komunikasi ke server HTTP dan broker MQTT dilakukan secara nirkabel melalui antarmuka WiFi terintegrasi[cite: 21, 23, 26].

### Foto Proses Praktikum
*(Tambahkan foto dokumentasi praktikum kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Foto Praktikum Modul 3](./Dokum/foto_praktikum_modul3.png)

---

## 4. JAWABAN SOAL (dalam modul 3.7 Pertanyaan Analisis)[cite: 27]

### 1. Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan![cite: 27]
* **Percobaan 3A (HTTP POST):** ESP32 berhasil terhubung ke jaringan WiFi dan mengirimkan payload JSON `{"kelembaban":65.0,"suhu":28.5}` ke endpoint `https://httpbin.org/post`[cite: 23, 24]. Server merespon dengan kode HTTP 200 OK dan mengembalikan (*echo*) payload data yang membuktikan bahwa data sukses diterima server[cite: 23, 25].
* **Percobaan 3B (MQTT Publish):** ESP32 berhasil terhubung ke broker MQTT `broker.hivemq.com` pada port 1883[cite: 23, 26]. ESP32 mempublikasikan data JSON `{"suhu":28.5,"kelembaban":65.0}` setiap 5 detik ke topik `unsoed/tk245004/kelompok2/sensor`[cite: 26, 27]. Data pesan secara *real-time* dapat diterima dan diverifikasi menggunakan aplikasi MQTT Explorer / HiveMQ Client[cite: 22, 23, 27].

### 2. Bandingkan besar overhead data dan pola komunikasi antara protokol HTTP dan MQTT berdasarkan hasil percobaan yang telah dilakukan![cite: 27]
* **Pola Komunikasi:**
  * **HTTP:** Menggunakan pola *Request-Response* (stateless)[cite: 21]. Setiap kali data dikirim, ESP32 harus membuka koneksi TCP baru, mengirimkan request, menunggu balasan server, lalu menutup koneksi (atau menggunakan keep-alive)[cite: 21].
  * **MQTT:** Menggunakan pola *Publish-Subscribe* melalui perantara *Broker*[cite: 22]. ESP32 membuka satu koneksi TCP persisten (*persistent connection*) ke broker, sehingga pengiriman data dilakukan secara langsung tanpa perlu melakukan proses *handshake* ulang[cite: 22].
* **Besar Overhead Data:**
  * **HTTP:** Memiliki *overhead header* yang sangat besar (bisa mencapai ratusan byte per request) karena menyertakan HTTP Header, User-Agent, Content-Type, Host, dll[cite: 21].
  * **MQTT:** Memiliki *overhead header* yang sangat kecil (ringan, hanya sekitar 2 byte pada fixed header)[cite: 22], sehingga sangat efisien dalam penggunaan bandwidth dan daya baterai[cite: 22].

### 3. Untuk skenario pengiriman data sensor secara terus-menerus setiap beberapa detik dalam jangka waktu lama, protokol manakah (HTTP atau MQTT) yang lebih sesuai digunakan? Jelaskan alasannya![cite: 27]
Protokol yang **lebih sesuai adalah MQTT**[cite: 22].  
**Alasan:**
1. **Koneksi Persisten (Persistent Connection):** MQTT menjaga koneksi TCP tetap terbuka ke broker[cite: 22], sehingga tidak ada pemborosan waktu dan energi untuk proses *handshake* TCP/TLS berulang kali seperti pada HTTP[cite: 21].
2. **Efisiensi Bandwidth dan Daya:** Header paket MQTT yang sangat kecil meminimalkan kuota data nirkabel dan konsumsi daya mikrokontroler[cite: 22].
3. **Latensi Rendah:** Data dapat terkirim secara *real-time* dengan latensi jauh lebih rendah dibandingkan transaksi HTTP Request-Response[cite: 21, 22].

### 4. Bagaimana peran format JSON dalam mendukung interoperabilitas data antara perangkat IoT dan berbagai platform/aplikasi yang berbeda?[cite: 27]
* **Format Teks Standar Terstruktur:** JSON direpresentasikan dalam bentuk teks berbasis pasangan kunci dan nilai (*key-value*)[cite: 22], yang bersifat independen terhadap bahasa pemrogramannya (*language-agnostic*)[cite: 22].
* **Kemudahan Parsing:** Data yang dikirim oleh ESP32 (menggunakan C/C++) dapat dengan mudah diproses (*parsed*) oleh berbagai platform backend atau dashboard aplikasi yang ditulis menggunakan JavaScript, Python, PHP, Java, maupun Go[cite: 22].
* **Fleksibilitas Struktur Data:** Mengubah atau menambah parameter sensor baru (misal: menambahkan koordinat GPS atau timestamp) tidak mematahkan kompatibilitas sistem secara keseluruhan karena JSON mendukung struktur data dinamis[cite: 22].