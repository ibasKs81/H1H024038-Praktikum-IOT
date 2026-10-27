# Laporan Praktikum Modul 4: Komunikasi Pertukaran Data (Pertukaran Data Dua Arah - Publish dan Subscribe)

## 1. Detail Percobaan
Praktikum ini bertujuan untuk memahami dan mengimplementasikan konsep komunikasi pertukaran data dua arah (*bidirectional / full duplex*) pada sistem IoT menggunakan protokol MQTT dan format data JSON:
* **Percobaan 4A (Subscribe dan Deserialisasi Data JSON untuk Kendali Aktuator):** Mengonfigurasi ESP32 untuk berlangganan (*subscribe*) ke topik perintah (`unsoed/tk245004/kelompok2/perintah`) pada broker HiveMQ (`broker.hivemq.com:1883`). ESP32 menerima payload JSON (misal: `{"perintah": "ON"}` atau `{"perintah": "OFF"}`), mendeserialisasi string JSON tersebut menggunakan pustaka ArduinoJson, dan mengendalikan status LED pada GPIO 26 secara *real-time*. Pada pengembangan tugas modifikasi, program diperluas untuk menerima parameter `intensitas` (0–255) yang digunakan untuk mengatur kecerahan LED menggunakan modulasi lebar pulsa (*PWM / LEDC*).
* **Percobaan 4B (Pertukaran Data Dua Arah - Publish dan Subscribe Secara Bersamaan):** Mengintegrasikan mekanisme pengiriman data sensor (*publish*) dan penerimaan perintah kendali (*subscribe*) secara simultan. ESP32 secara periodik (setiap 5 detik) membaca sensor suhu & kelembapan DHT22 pada GPIO 4 dan mempublikasikannya ke topik `unsoed/tk245004/kelompok2/data` menggunakan pendekatan *non-blocking* (`millis()`). Secara bersamaan, ESP32 tetap responsif menerima perintah kendali aktuator (LED dan Buzzer) melalui fungsi `client.loop()` tanpa terputus ataupun terhambat oleh proses *publish*.

## 2. Library atau Dependencies
Beberapa pustaka (*library*) yang digunakan pada percobaan Modul 4 meliputi:
* **`WiFi.h`**: Mengelola koneksi jaringan WiFi lokal pada mikrokontroler ESP32 agar terhubung ke jaringan internet.
* **`PubSubClient.h`**: Mengelola komunikasi protokol MQTT (koneksi ke broker HiveMQ, *publish*, *subscribe*, dan eksekusi fungsi *callback* saat pesan masuk).
* **`ArduinoJson.h`**: Mengelola proses serialisasi (pembuatan string JSON dari objek) dan deserialisasi (penguraian string JSON menjadi dokumen objek data).
* **`DHT.h` / `Adafruit Unified Sensor`**: Digunakan untuk membaca nilai fisik suhu dan kelembapan dari modul sensor DHT22.

## 3. Skematik dan Rangkaian

### Konfigurasi Pin
Tabel pemetaan pin komponen yang digunakan pada praktikum Modul 4:

| Komponen / Perangkat | Pin Mikrokontroler | Keterangan |
| :--- | :--- | :--- |
| **Sensor DHT22 (Pin Data)** | GPIO 4 | Pin komunikasi sinyal digital sensor DHT22 |
| **VCC Sensor DHT22** | 3.3V / 5V | Sumber tegangan daya sensor |
| **GND Sensor DHT22** | GND | Ground bersama rangkaian |
| **LED Indikator 1 (Aktuator Utama)** | GPIO 26 | Terhubung ke Anoda LED melalui Resistor 220 Ω (Mendukung PWM) |
| **Buzzer / LED 2 (Aktuator Kedua)** | GPIO 27 | Terhubung ke pin positif buzzer/LED melalui Resistor pembatas |
| **Kabel USB Data** | Micro-USB / USB-C | Catu daya ESP32 dan komunikasi Serial (115200 baud) |

### Skematik Rangkaian
*(Tambahkan gambar skematik rangkaian kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Skematik Rangkaian Modul 4](./Dokum/skematik_modul4.png)

*Deskripsi Ringkas:* Pin GPIO 4 ESP32 dihubungkan ke pin Data modul DHT22. Pin GPIO 26 dihubungkan ke anoda LED melalui resistor 220 Ohm, dan pin GPIO 27 dihubungkan ke aktuator kedua (buzzer/LED tambahan). Katoda aktuator serta ground sensor disatukan ke rel ground (GND) mikrokontroler.

### Foto Proses Praktikum
*(Tambahkan foto dokumentasi praktikum kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Foto Praktikum Modul 4](./Dokum/foto_praktikum_modul4.png)

---

## 4. JAWABAN SOAL

### A. Pertanyaan Praktikum 4.5.4 (Percobaan 4A)

#### 1. Gambarkan diagram alur (*flowchart*) proses penerimaan dan pemrosesan pesan pada fungsi `callback`!
Diagram alur proses fungsi `callback`:
```
[Start Callback (topic, payload, length)]
                   │
                   ▼
[Konversi byte payload menjadi String 'pesan']
                   │
                   ▼
[Tampilkan pesan mentah dan nama topik ke Serial]
                   │
                   ▼
[Lakukan deserializeJson(doc, pesan)]
                   │
         ┌─────────┴─────────┐
         ▼                   ▼
    [Gagal parsing?]      [Berhasil]
         │                   │
        Ya                   Tidak
         │                   │
         ▼                   ▼
[Cetak pesan error     [Ambil nilai 'perintah' dari doc]
 & Keluar (return)]          │
                             ▼
                   [Cek apakah doc memiliki 'intensitas'?]
                             │
                   ┌─────────┴─────────┐
                   ▼                   ▼
                 [Ada]              [Tidak]
                   │                   │
         [intensitas = doc]   [intensitas = 255 (default)]
                   │                   │
                   └─────────┬─────────┘
                             │
                             ▼
                  [Apakah perintah == "ON"?]
                             │
                   ┌─────────┴─────────┐
                   ▼                   ▼
                 [ Ya ]              [Tidak]
                   │                   │
        [Nyalakan LED (PWM)]   [Apakah perintah == "OFF"?]
                   │                   │
                   │         ┌─────────┴─────────┐
                   │         ▼                   ▼
                   │       [ Ya ]              [Tidak]
                   │         │                   │
                   │  [Matikan LED]    [Cetak perintah tidak dikenal]
                   │         │                   │
                   └─────────┼───────────────────┘
                             │
                             ▼
                      [Selesai (End)]
```

#### 2. Apa yang akan terjadi apabila pesan yang dipublikasikan bukan merupakan format JSON yang valid?
* Pustaka `ArduinoJson` melalui fungsi `deserializeJson(doc, pesan)` akan mengembalikan kode status galat (*error code*) berupa `DeserializationError::InvalidInput` atau `IncompleteInput`.
* Kondisi `if (error)` akan bernilai benar (`true`), sehingga program akan mencetak string galat ke Serial Monitor (misalnya `Gagal parsing JSON: InvalidInput`) dan segera mengeksekusi perintah `return;`.
* Dengan adanya penanganan galat (*error handling*) ini, ESP32 tidak akan mengalami *crash* atau *kernel panic* (Watchdog Reset), melainkan mengabaikan paket rusak tersebut secara aman dan tetap siap menerima pesan berikutnya.

#### 3. Jelaskan mengapa fungsi `client.subscribe()` dipanggil di dalam fungsi `hubungkanMQTT()`, bukan di dalam `setup()`!
* Protokol MQTT mensyaratkan sesi koneksi TCP dan *handshake* protokol MQTT telah berstatus terhubung (*connected*) sebelum klien dapat mendaftarkan langganan topik ke broker.
* Jika `client.subscribe()` diletakkan di `setup()`, pemanggilan hanya terjadi satu kali di awal penyalaan perangkat. Apabila saat itu koneksi belum sepenuhnya aktif atau jika di kemudian waktu terjadi pemutusan koneksi jaringan WiFi/MQTT (*reconnection*), tabel langganan (*subscription list*) pada sesi broker yang bersifat non-persisten akan hilang.
* Menempatkan `client.subscribe()` di dalam blok `if (client.connect(...))` pada fungsi `hubungkanMQTT()` menjamin bahwa setiap kali ESP32 berhasil tersambung (maupun tersambung kembali setelah putus), ESP32 akan selalu mendaftarkan ulang semua topik yang ingin didengarkan secara otomatis.

#### 4. Modifikasi program agar data JSON yang diterima memuat nilai intensitas untuk mengatur kecerahan LED menggunakan PWM, beserta penjelasan kode baris demi baris:
Berikut adalah kode program hasil modifikasi:
```cpp
// 1. Inisialisasi parameter PWM LEDC untuk ESP32
const int pwmChannel = 0;       // Kanal PWM 0
const int pwmFreq = 5000;       // Frekuensi modulasi 5 kHz
const int pwmResolusi = 8;      // Resolusi 8-bit (rentang nilai duty cycle 0 - 255)

// 2. Di dalam setup():
ledcSetup(pwmChannel, pwmFreq, pwmResolusi); // Konfigurasi kanal PWM, frekuensi, dan resolusi
ledcAttachPin(ledPin, pwmChannel);           // Menghubungkan pin fisik GPIO 26 ke kanal PWM
ledcWrite(pwmChannel, 0);                    // Kondisi awal LED mati (duty cycle 0)

// 3. Di dalam callback():
int intensitas = 255;                        // Deklarasi variabel intensitas default penuh (255)
if (doc.containsKey("intensitas")) {         // Memeriksa keberadaan kunci 'intensitas' pada payload JSON
  intensitas = doc["intensitas"];            // Mengekstrak nilai intensitas dari objek JSON
}

if (String(perintah) == "ON") {
  ledcWrite(pwmChannel, intensitas);         // Mengatur duty cycle PWM sesuai intensitas yang diterima
  Serial.print("Aktuator: ON | Intensitas: ");
  Serial.println(intensitas);
} else if (String(perintah) == "OFF") {
  ledcWrite(pwmChannel, 0);                  // Mengatur duty cycle ke 0 untuk mematikan LED
  Serial.println("Aktuator: OFF | Intensitas: 0");
}
```
**Penjelasan Baris per Baris:**
* `ledcSetup(0, 5000, 8);`: Mengonfigurasi modul timer hardware LEDC ESP32 pada kanal 0 dengan frekuensi gelombang 5 kHz dan resolusi 8-bit (nilai 0 = mati total, nilai 255 = kecerahan maksimal).
* `ledcAttachPin(ledPin, 0);`: Memetakan sinyal kanal PWM 0 langsung ke pin fisik GPIO 26 tempat kaki resistor dan anoda LED terpasang.
* `if (doc.containsKey("intensitas"))`: Memeriksa apakah pengirim perintah mengirimkan properti intensitas. Hal ini mencegah pembacaan nilai *null* atau kesalahan konversi jika pengirim hanya mengirim `{"perintah": "ON"}`.
* `intensitas = doc["intensitas"];`: Mengambil data numerik intensitas dari dokumen JSON yang telah terurai.
* `ledcWrite(pwmChannel, intensitas);`: Menghasilkan sinyal modulasi lebar pulsa (*duty cycle*) pada pin LED secara presisi sesuai nilai intensitas yang diminta.

---

### B. Pertanyaan Praktikum 4.6.4 (Percobaan 4B)

#### 1. Mengapa penggunaan `delay()` yang lama sebaiknya dihindari pada program yang menggabungkan proses *publish* dan *subscribe* secara bersamaan?
Fungsi `delay()` bersifat *blocking*, yaitu menahan CPU mikrokontroler dalam loop tunggu pasif (*busy wait*) sehingga instruksi berikutnya tidak dapat dieksekusi selama durasi tersebut. Jika `delay(5000)` digunakan, maka selama 5 detik fungsi `client.loop()` tidak akan terpanggil. Akibatnya:
1. ESP32 tidak dapat merespons paket perintah yang masuk dari broker secara *real-time* (terjadi keterlambatan atau penumpukan antrean).
2. ESP32 tidak dapat mengirimkan paket sinyal *heartbeat* (*PINGREQ*) tepat waktu ke broker, yang menyebabkan broker menganggap perangkat telah *offline* atau *timed out*, sehingga sambungan MQTT diputus secara sepihak.

#### 2. Jelaskan cara kerja mekanisme *non-blocking* menggunakan fungsi `millis()` pada program!
* Fungsi `millis()` mengembalikan jumlah milidetik yang telah berlalu sejak ESP32 pertama kali dinyalakan.
* Mekanisme *non-blocking* bekerja dengan menyimpan stempel waktu eksekusi terakhir pada variabel `waktuTerakhirPublish`.
* Pada setiap siklus `loop()`, program melakukan evaluasi logika:
  `if (millis() - waktuTerakhirPublish >= intervalPublish)`
* Jika selisih waktu belum mencapai 5000 ms, eksekusi kode di dalam blok kondisi akan dilewati, dan CPU dapat langsung mengeksekusi instruksi berikutnya seperti `client.loop()`.
* Ketika selisih waktu mencapai atau melampaui 5000 ms, sensor dibaca, data dipublikasikan, dan variabel `waktuTerakhirPublish` diperbarui dengan nilai `millis()` saat itu. Dengan demikian, pengiriman data berkala terjadi tanpa pernah menghentikan responsivitas sistem.

#### 3. Apa yang akan terjadi apabila fungsi `client.loop()` jarang dipanggil (misalnya hanya sekali setiap 10 detik)?
* **Respon Tertunda Ekstrem:** Setiap kali ada klien lain mengirimkan perintah kendali aktuator, perintah tersebut baru akan dibaca dan dieksekusi setelah jeda hingga 10 detik.
* **Potensi Koneksi Terputus (*Keep-Alive Timeout*):** Secara default, *keep-alive interval* pada pustaka PubSubClient adalah 15 detik. Jika loop mengalami keterlambatan atau fluktuasi jaringan, ESP32 gagal mengirim pesan ping ke broker tepat waktu, sehingga broker memutuskan koneksi klien secara otomatis.
* **Risiko Buffer Penuh (*Buffer Overflow*):** Data paket yang masuk ke soket TCP mikrokontroler dapat menumpuk di buffer internal chip, berpotensi menimbulkan *packet drop* jika kapasitas buffer terlampaui.

#### 4. Modifikasi program penambahan topik perintah aktuator kedua (Buzzer) dengan seleksi topik pada fungsi `callback`:
Berikut kode program hasil modifikasi:
```cpp
// 1. Menambahkan definisi topik dan pin untuk aktuator kedua
const char* topicPerintahLED = "unsoed/tk245004/kelompok2/perintah";
const char* topicPerintahBuzzer = "unsoed/tk245004/kelompok2/perintah_buzzer";
const int buzzerPin = 27; // Pin GPIO Buzzer

// 2. Di dalam hubungkanMQTT():
client.subscribe(topicPerintahLED);     // Berlangganan topik LED
client.subscribe(topicPerintahBuzzer);  // Berlangganan topik Buzzer

// 3. Di dalam callback(char* topic, byte* payload, unsigned int length):
if (strcmp(topic, topicPerintahLED) == 0) {
  const char* perintah = doc["perintah"];
  if (String(perintah) == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("Perintah LED: ON");
  } else if (String(perintah) == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("Perintah LED: OFF");
  }
} 
else if (strcmp(topic, topicPerintahBuzzer) == 0) {
  const char* perintah = doc["perintah"];
  if (String(perintah) == "ON") {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Perintah Buzzer: ON");
  } else if (String(perintah) == "OFF") {
    digitalWrite(buzzerPin, LOW);
    Serial.println("Perintah Buzzer: OFF");
  }
}
```
**Penjelasan Baris per Baris:**
* `strcmp(topic, topicPerintahLED) == 0`: Menggunakan fungsi pembanding string standar C untuk memeriksa apakah nama topik asal pesan sama persis dengan `topicPerintahLED`.
* `digitalWrite(ledPin, HIGH/LOW)`: Mengubah logika pin GPIO 26 untuk menyalakan/mematikan lampu LED jika topik yang cocok adalah topik kendali LED.
* `else if (strcmp(topic, topicPerintahBuzzer) == 0)`: Cabang logika kedua untuk mendeteksi apakah pesan masuk ditujukan bagi topik aktuator kedua (`topicPerintahBuzzer`).
* `digitalWrite(buzzerPin, HIGH/LOW)`: Mengaktifkan atau mematikan modul buzzer pada GPIO 27 sesuai nilai perintah yang diterima dari topik khusus tersebut.

---

### C. Pertanyaan Analisis (Modul 4.7)

#### 1. Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!
* **Percobaan 4A:** Perangkat ESP32 berhasil terhubung ke jaringan nirkabel dan broker MQTT HiveMQ. ESP32 mampu mendengarkan (*subscribe*) topik perintah secara pasif. Saat pesan JSON seperti `{"perintah":"ON","intensitas":180}` dikirim melalui aplikasi MQTT Explorer, fungsi *callback* secara seketika mengurai dokumen JSON dan mengatur keluaran daya LED melalui pin PWM GPIO 26.
* **Percobaan 4B:** Sistem komunikasi *full-duplex* dua arah berhasil beroperasi secara stabil. ESP32 mampu membaca data suhu dan kelembapan dari modul DHT22 secara berkala setiap 5 detik lalu mempublikasikannya ke topik data dalam format JSON. Pada saat yang sama, perintah kontrol LED dan Buzzer yang dikirimkan ke topik terpisah dapat diterima dan dieksekusi seketika tanpa mengalami latensi dan tanpa memutus koneksi broker.

#### 2. Bandingkan mekanisme komunikasi satu arah (*publish* saja, Modul 3) dengan komunikasi dua arah (*publish* dan *subscribe*) yang diimplementasikan pada modul ini!
| Parameter | Komunikasi Satu Arah (Modul 3) | Komunikasi Dua Arah (Modul 4) |
| :--- | :--- | :--- |
| **Arah Aliran Data** | Perangkat $\rightarrow$ Broker / Server (Hanya Telemetri) | Perangkat $\rightleftharpoons$ Broker / Server (Telemetri & Kendali) |
| **Mekanisme Perangkat**| Klien aktif mengirimkan data (*Publish*) | Klien mengirim data (*Publish*) sekaligus mendengarkan (*Subscribe*) |
| **Fungsi Pemrosesan** | Cukup serialisasi data dan `client.publish()` | Memerlukan *Callback handler*, deserialisasi JSON, dan manajemen *buffer* |
| **Ketergantungan Waktu**| Sederhana (dapat menggunakan delay periodik) | Wajib *Non-blocking* (`millis()`) agar *event loop* tidak terinterupsi |
| **Interaktivitas** | Pasif; tidak dapat menerima perintah dari jarak jauh | Interaktif; aktuator dapat dikendalikan langsung oleh pengguna |

#### 3. Mengapa pendekatan *non-blocking* (menggunakan `millis()`) lebih sesuai dibandingkan pendekatan *blocking* (menggunakan `delay()`) pada sistem IoT yang memerlukan komunikasi dua arah secara *real-time*?
Sistem IoT dua arah menuntut kesiapan mendengarkan (*listener responsiveness*) yang konstan. Dengan pendekatan *non-blocking*:
1. **Loop Eksekusi Kontinu:** Fungsi utama `loop()` dapat berputar ribuan kali per detik, sehingga fungsi `client.loop()` dapat langsung memproses paket TCP yang tiba di kartu jaringan secara *real-time*.
2. **Multi-tasking Kooperatif:** ESP32 dapat menangani beberapa pekerjaan secara paralel (misalnya memantau sensor, menghitung timer, dan merespons tombol/aktuator) dalam satu inti mikroprosesor tanpa saling memblokir.
3. **Integritas Sesi Jaringan:** Ping periodik MQTT terjaga secara presisi sehingga koneksi terhadap broker tetap awet dan tidak rentan mengalami *timeout* atau *reconnect loop*.

#### 4. Berikan contoh penerapan komunikasi dan pertukaran data dua arah pada sistem IoT nyata, dan jelaskan manfaatnya dibandingkan sistem yang hanya satu arah!
* **Contoh Penerapan:** **Sistem Presisi *Smart Greenhouse* / Pertanian Cerdas (*Smart Farming*)**.
  * **Arah Unggah (*Publish*):** ESP32 membaca suhu udara, kelembapan tanah (*soil moisture*), dan intensitas radiasi cahaya matahari secara berkala, kemudian mengirimkannya ke *cloud server* untuk dicatat ke basis data dan ditampilkan pada dasbor petani.
  * **Arah Unduh (*Subscribe*):** Server atau aplikasi smartphone petani dapat mengirimkan instruksi balik secara instan, seperti menyalakan pompa irigasi tetes (*drip irrigation pump*), mengaktifkan jaring peneduh (*shade screen motor*), atau menyalakan kipas exhaust ventilasi.
* **Manfaat Dibandingkan Sistem Satu Arah:**
  Pada sistem satu arah, perangkat hanya bertindak sebagai pencatat data (*data logger* pasif); jika terjadi kekeringan tanah atau lonjakan suhu ekstrem, sistem tidak dapat melakukan tindakan penyelamatan apapun secara otomatis atau jarak jauh. Dengan komunikasi dua arah, sistem berubah menjadi ekosistem kendali otomatis (*closed-loop control system*) yang dapat merespons anomali secara *real-time*, menghemat air dan energi, serta mencegah kerusakan tanaman secara efisien.
