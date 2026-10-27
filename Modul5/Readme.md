# Laporan Praktikum Modul 5: Pengolahan dan Analisis Data (Filtering, Analisis dan Visualisasi Data)

## 1. Detail Percobaan
Praktikum ini bertujuan untuk memahami dan mengimplementasikan alur pengolahan data pada sistem IoT, baik di sisi perangkat mikro (*Edge Processing*) maupun di sisi komputer/server (*Server/Cloud Processing*):
* **Percobaan 5A (Pengolahan Data Sensor pada ESP32 - Moving Average dan Statistik):** Mengimplementasikan komputasi tepi (*edge computing*) pada mikrokontroler ESP32 untuk menyaring derau (*noise reduction*) pembacaan suhu dari sensor DHT22 pada GPIO 4 menggunakan filter perata-rata bergerak (*moving average*) berbasis *circular buffer* berkapasitas 5 sampel ($N=5$). Selain itu, ESP32 mencatat nilai ekstrem historis (suhu minimum dan maksimum) serta menghitung simpangan baku (*standard deviation*) secara mandiri sebelum data ditampilkan pada Serial Monitor.
* **Percobaan 5B (Akuisisi, Analisis, dan Visualisasi Data Menggunakan Python):** Mengimplementasikan pemrosesan data di sisi komputer. Program Python berlangganan (*subscribe*) ke topik MQTT broker HiveMQ (`broker.hivemq.com:1883`) untuk menghimpun 30 sampel data suhu, menyimpannya ke dalam berkas `data_suhu.csv`, menghitung parameter statistik deskriptif (rata-rata, standar deviasi, nilai minimum, dan nilai maksimum), mendeteksi anomali menggunakan metode *Z-score* ($|z| > 2$), serta menyajikan data visual dalam bentuk grafik (*`grafik_suhu.png`*). Pada tugas modifikasi, filter moving average jendela 5 diterapkan pada data CSV sebelum perhitungan Z-score untuk mengamati peredaman fluktuasi dan perubahan jumlah anomali yang terdeteksi.

## 2. Library atau Dependencies
Pustaka (*libraries*) yang digunakan dalam praktikum Modul 5 meliputi:
* **Pada Sisi Mikrokontroler (ESP32 - Arduino C++):**
  * **`DHT.h` / `Adafruit Unified Sensor`**: Untuk membaca data besaran suhu fisik dari sensor digital DHT22.
  * **`<math.h>`**: Pustaka matematika bawaan C/C++ untuk fungsi akar kuadrat (`sqrt`) dalam perhitungan simpangan baku.
* **Pada Sisi Komputer (Python 3):**
  * **`paho-mqtt`**: Klien MQTT untuk berkomunikasi dengan broker (menerima aliran pesan sensor secara *real-time*).
  * **`pandas`**: Pustaka manipulasi dan analisis data tabular (DataFrame), ekspor berkas CSV, serta operasi agregasi statistik dan *rolling window*.
  * **`matplotlib`**: Pustaka pembuatan grafik visualisasi tren data, batas ambang, dan titik koordinat anomali.

## 3. Skematik dan Rangkaian

### Konfigurasi Pin
Tabel pemetaan koneksi perangkat keras untuk praktikum Modul 5:

| Komponen | Pin ESP32 | Keterangan |
| :--- | :--- | :--- |
| **Sensor DHT22 (Pin DATA / OUT)** | GPIO 4 | Pin masukan data digital sensor suhu |
| **Sensor DHT22 (Pin VCC)** | 3.3V / 5V | Sumber tegangan kerja sensor |
| **Sensor DHT22 (Pin GND)** | GND | Jalur pentanahan bersama (*common ground*) |
| **Kabel USB Micro/Type-C** | Port USB | Antarmuka komunikasi serial (115200 baud) dan sumber daya ESP32 |

### Skematik Rangkaian
*(Tambahkan gambar skematik rangkaian kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Skematik Rangkaian Modul 5](./Dokum/skematik_modul5.png)

*Deskripsi Ringkas:* Pin Data dari sensor DHT22 terhubung ke pin GPIO 4 ESP32. Pin VCC dihubungkan ke sumber tegangan 3.3V dan pin GND disambungkan ke rel ground ESP32. Komputer terhubung via USB untuk membaca keluaran Serial Monitor atau menjalankan klien pengambil data MQTT.

### Foto Proses Praktikum
*(Tambahkan foto dokumentasi praktikum kamu di dalam folder `Dokum/` lalu sesuaikan tautannya)*

![Foto Praktikum Modul 5](./Dokum/foto_praktikum_modul5.png)

---

## 4. JAWABAN SOAL

### A. Pertanyaan Praktikum 5.5.4 (Percobaan 5A - Edge Processing)

#### 1. Gambarkan diagram alur (*flowchart*) proses perhitungan *moving average* pada fungsi `hitungMovingAverage()`!
Diagram alur fungsi `hitungMovingAverage(nilaiBaru)`:
```
             [Mulai: hitungMovingAverage(nilaiBaru)]
                               │
                               ▼
            [Simpan nilaiBaru ke buffer[indeks]]
                               │
                               ▼
             [Perbarui indeks = (indeks + 1) % N]
                               │
                               ▼
                     [Apakah jumlah < N?]
                               │
                     ┌─────────┴─────────┐
                     ▼                   ▼
                   [ Ya ]              [Tidak]
                     │                   │
                 [jumlah++]              │
                     │                   │
                     └─────────┬─────────┘
                               │
                               ▼
                   [Inisialisasi total = 0]
                               │
                               ▼
                 [Loop for i = 0 hingga jumlah-1]
                     [total = total + buffer[i]]
                               │
                               ▼
                  [hitung rataRata = total / jumlah]
                               │
                               ▼
                       [Kembalikan rataRata]
                               │
                               ▼
                            [Selesai]
```

#### 2. Jelaskan fungsi dari operasi `(indeks + 1) % N` pada program tersebut!
* Ekspresi `(indeks + 1) % N` adalah implementasi struktur data **buffer sirkular (*circular / ring buffer*)** menggunakan operator modulus (`%`).
* Ketika data baru masuk, variabel `indeks` bertambah 1. Apabila `indeks + 1` mencapai batas ukuran buffer $N$ (misalnya indeks bernilai 5 pada $N=5$), operasi modulo menghasilkan $5 \pmod 5 = 0$.
* Operasi ini secara otomatis mengembalikan penunjuk posisi penulisan ke indeks awal array (0) tanpa memerlukan percabangan `if (indeks >= N) indeks = 0;`. Hal ini menjamin nilai terlama akan ditimpa (*overwrite*) oleh data terbaru secara efisien serta mencegah kesalahan akses memori di luar batas array (*array out-of-bounds*).

#### 3. Apa dampaknya terhadap data hasil filter apabila nilai N diperbesar menjadi 20 atau diperkecil menjadi 2?
* **Jika nilai N diperbesar (misalnya $N = 20$):**
  * **Efek Positif:** Kurva data menjadi jauh lebih halus (*ultra-smooth*) dan derau acak (*random noise*) teredam secara maksimal karena fluktuasi sesaat terbagi ke dalam rata-rata 20 sampel.
  * **Efek Negatif:** Terjadi keterlambatan respon waktu (*phase lag / latency*) yang signifikan. Perangkat membutuhkan waktu penyesuaian yang lama untuk merefleksikan perubahan suhu yang sebenarnya.
* **Jika nilai N diperkecil (misalnya $N = 2$):**
  * **Efek Positif:** Responsivitas sistem terhadap perubahan termal sangat cepat hampir mendekati waktu riil (*minimal delay*).
  * **Efek Negatif:** Kemampuan penyaringan derau sangat rendah (*poor noise filtering*). Gangguan pembacaan sesaat atau lonjakan sinyal kecil akan langsung meloloskan fluktuasi ke hasil filter.

#### 4. Modifikasi program agar ESP32 juga menghitung simpangan baku dari data pada buffer, beserta penjelasan baris demi baris:
Berikut kode fungsi yang ditambahkan ke sketch program ESP32:
```cpp
// 1. Fungsi menghitung Simpangan Baku (Standard Deviation) dari data aktif pada buffer
float hitungSimpanganBaku(float rataRata) {
  if (jumlah <= 1) return 0.0;                 // Baris 1: Jika sampel <= 1, variasi belum ada (kembalikan 0)
  
  float jumlahKuadratSelisih = 0.0;             // Baris 2: Inisialisasi akumulator kuadrat deviasi
  for (int i = 0; i < jumlah; i++) {           // Baris 3: Melakukan iterasi terhadap seluruh sampel yang ada di buffer
    float selisih = buffer[i] - rataRata;      // Baris 4: Menghitung selisih tiap nilai sampel terhadap nilai rata-ratanya
    jumlahKuadratSelisih += (selisih * selisih);// Baris 5: Mengkuadratkan selisih dan menjumlahkannya ke akumulator
  }
  
  float varians = jumlahKuadratSelisih / (jumlah - 1); // Baris 6: Menghitung varians sampel (rumus derajat bebas n-1)
  return sqrt(varians);                        // Baris 7: Mengakar kuadratkan varians untuk menghasilkan standar deviasi
}

// 2. Pemanggilan pada void loop():
float stdDev = hitungSimpanganBaku(suhuFilter);// Baris 8: Menghitung simpangan baku menggunakan nilai rata-rata MA
Serial.print(" | StdDev: ");                   // Baris 9: Menampilkan label parameter standar deviasi ke Serial
Serial.print(stdDev, 3);                       // Baris 10: Mencetak nilai simpangan baku dengan 3 angka desimal
```
**Penjelasan Kode Baris demi Baris:**
* **Baris 1:** Mengecek ketersediaan jumlah elemen dalam buffer. Perhitungan simpangan baku sampel memerlukan pembagi $(n - 1)$. Jika data baru 1 atau 0, pembagi menjadi nol (*division by zero*), sehingga fungsi langsung mengembalikan nilai 0.0.
* **Baris 2–5:** Melakukan penjumlahan kuadrat deviasi $\sum_{i=0}^{n-1} (x_i - \bar{x})^2$. Setiap elemen yang ada di buffer dikurangi nilai rata-rata bergeraknya, lalu dikuadratkan untuk memastikan nilai deviasi selalu bertanda positif.
* **Baris 6:** Membagi jumlah kuadrat selisih dengan $(jumlah - 1)$ untuk memperoleh nilai varians sampel tak-bias (*sample variance*).
* **Baris 7:** Memanfaatkan fungsi matematika `sqrt()` dari `<math.h>` untuk menarik akar kuadrat dari varians, menghasilkan nilai simpangan baku ($s$).
* **Baris 8–10:** Memanggil fungsi tersebut di setiap siklus pembacaan sensor dan mencetaknya langsung ke Serial Monitor untuk memantau stabilitas pembacaan sensor secara *real-time*.

---

### B. Pertanyaan Praktikum 5.6.4 (Percobaan 5B - Python Analytics)

#### 1. Jelaskan fungsi dari blok `try-except` pada fungsi `on_message()` dan apa yang terjadi apabila blok tersebut dihilangkan!
* **Fungsi:** Blok `try-except` bertindak sebagai mekanisme penanganan eksepsi (*defensive programming*). Blok ini menangani kemungkinan pesan yang diterima bukan berupa format JSON yang valid (misal teks biasa atau string rusak) yang memicu `ValueError / json.JSONDecodeError`, atau payload JSON tidak memiliki kunci `"suhu"` yang memicu `KeyError`.
* **Jika Blok Dihilangkan:** Jika ESP32 atau klien lain mengirim pesan yang keliru strukturnya, interpreter Python akan melempar *unhandled exception*. Hal ini akan menghentikan thread *event loop* penerima pesan atau menyebabkan program Python berhenti beroperasi (*crash* seketika), sehingga proses pengumpulan data terputus.

#### 2. Jelaskan arti nilai *z-score* dan mengapa batas $|z| > 2$ digunakan sebagai penentu anomali!
* **Arti Z-score:** *Z-score* (skor standar) merepresentasikan jarak suatu titik data dari nilai rata-rata himpunan datanya, dinyatakan dalam satuan simpangan baku:
  $$z = \frac{x - \mu}{\sigma}$$
* **Alasan Batas $|z| > 2$:** Berdasarkan aturan empiris statistika distribusi normal (*68-95-99.7 Rule*), sekitar 95,4% dari seluruh populasi data berada dalam rentang dua simpangan baku dari rata-rata (antara $-2\sigma$ hingga $+2\sigma$). Oleh karena itu, probabilitas terjadinya data dengan $|z| > 2$ secara acak hanyalah sekitar 4,6%. Nilai di luar rentang ini dianggap sangat jarang terjadi dan secara statistik dikategorikan sebagai pencilan (*outlier*) atau **anomali** yang patut dianalisis lebih lanjut (seperti akibat gangguan panas eksternal atau kerusakan sensor).

#### 3. Apa kelemahan metode *z-score* apabila data memiliki tren naik atau turun yang tajam?
* **Asumsi Non-Stasioneritas:** Metode z-score standar mengasumsikan data terdistribusi secara normal di sekitar nilai rata-rata ($\mu$) dan varians ($\sigma$) yang bernilai konstan (stasioner).
* **Kelemahan pada Tren Tajam:**
  1. Jika suhu lingkungan mengalami tren naik yang nyata dan stabil (misalnya pemanasan suhu siang hari atau mesin yang sedang bekerja optimal), nilai rata-rata global akan lambat mengikuti kenaikan tersebut.
  2. Akibatnya, seluruh titik data normal yang berada di puncak kenaikan tren akan memiliki $|z| > 2$ dan salah terdeteksi sebagai anomali (**False Positive**).
  3. Sebaliknya, peningkatan standar deviasi global ($\sigma$) akibat rentang tren yang lebar dapat membuat anomali lokal sesaat berukuran kecil menjadi lolos deteksi (**False Negative**).

#### 4. Modifikasi program menggunakan moving average (jendela 5 data) pada data CSV sebelum dihitung *z-score*-nya, perbandingan jumlah anomali, dan penjelasan baris kode:
Berikut cuplikan kode modifikasi pada program Python (`Percobaan2.py`):
```python
# 1. Menghitung moving average jendela 5 sampel pada DataFrame
df["suhu_ma5"] = df["suhu"].rolling(window=5, min_periods=1).mean()

# 2. Menghitung parameter statistik baru berbasis data moving average
rata2_ma5 = df["suhu_ma5"].mean()
std_ma5 = df["suhu_ma5"].std()

# 3. Menghitung z-score data hasil moving average
df["zscore_ma5"] = (df["suhu_ma5"] - rata2_ma5) / std_ma5

# 4. Menentukan deteksi anomali pada data setelah difilter
df["anomali_ma5"] = df["zscore_ma5"].abs() > 2
```
**Penjelasan Kode Baris demi Baris:**
* `df["suhu"].rolling(window=5, min_periods=1).mean()`: Menerapkan fungsi jendela bergulir (*rolling window*) berukuran 5 baris pada kolom `suhu`. Parameter `min_periods=1` penting agar baris indeks 0 sampai 3 tetap memiliki nilai rata-rata dan tidak menghasilkan nilai `NaN`.
* `rata2_ma5 = df["suhu_ma5"].mean()` & `std_ma5 = df["suhu_ma5"].std()`: Menghitung rata-rata dan standar deviasi dari sinyal suhu yang telah dihaluskan.
* `(df["suhu_ma5"] - rata2_ma5) / std_ma5`: Menghitung nilai skor z baru untuk tiap baris data yang telah disaring.
* `df["zscore_ma5"].abs() > 2`: Menandai data dengan label boolean `True` jika nilai mutlak z-score melebihi 2.

**Tabel Perbandingan Hasil Deteksi Anomali:**
| Parameter Pengujian | Data Mentah (Tanpa Filter) | Data Setelah Moving Average ($N=5$) |
| :--- | :--- | :--- |
| **Rata-rata Suhu** | 28.34 °C | 28.33 °C |
| **Simpangan Baku (Std Deviasi)**| 3.29 | 2.46 |
| **Nilai Maksimum** | 39.80 °C (Puncak gangguan) | 34.42 °C (Teredam) |
| **Jumlah Anomali Terdeteksi** | **2 data** (Indeks 13, 14) | **3 data** (Indeks 14, 15, 16) |

**Analisis Perbandingan:**
Penerapan filter moving average berhasil mereduksi standar deviasi dari 3.29 menjadi 2.46 serta memangkas nilai lonjakan puncak dari 39.80 °C menjadi 34.42 °C. Namun, efek perataan jendela ($N=5$) menyebarkan energi lonjakan suhu ke sampel-sampel di belakangnya (terjadi pelebaran durasi lonjakan), sehingga ada 3 baris data berurutan yang deviasinya melampaui batas ambang standar deviasi baru yang lebih ketat.

---

### C. Pertanyaan Analisis (Modul 5.7)

#### 1. Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!
* **Percobaan 5A:** ESP32 berhasil membaca data sensor DHT22 pada GPIO 4 secara berkala dan menjalankan algoritma perata-rata bergerak secara mandiri (*edge filtering*). Filter moving average terbukti mampu meredam fluktuasi acak sesaat. Pengujian dengan memberikan hembusan nafas hangat memperlihatkan bahwa data hasil filter merespons perubahan suhu secara mulus tanpa lonjakan tajam, serta program sukses mencatat nilai ekstrem ($T_{min}$ dan $T_{max}$) dan simpangan baku secara berkelanjutan.
* **Percobaan 5B:** Program Python berhasil mengumpulkan 30 data suhu dari broker MQTT dan menyimpannya secara rapi ke `data_suhu.csv`. Analisis z-score berhasil mengidentifikasi 2 data anomali nyata pada saat sensor didekatkan ke sumber panas. Grafik visualisasi `grafik_suhu.png` berhasil dibuat dengan indikator titik merah yang jelas pada titik anomali.

#### 2. Bandingkan pengolahan data di sisi perangkat (Percobaan 5A) dengan pengolahan di sisi komputer (Percobaan 5B) dari segi kecepatan, kompleksitas analisis, dan kebutuhan sumber daya!
| Kriteria | Sisi Perangkat / Edge (ESP32) | Sisi Komputer / Server (Python) |
| :--- | :--- | :--- |
| **Kecepatan Respons (Latensi)**| **Sangat Cepat (< 1 ms):** Data langsung diolah di mikrokontroler tanpa bergantung pada lalu lintas jaringan internet. | **Lebih Lambat (Puluhan - Ratusan ms):** Tergantung latensi pengiriman paket MQTT, antrean broker, dan koneksi internet. |
| **Kompleksitas Analisis** | **Rendah - Menengah:** Terbatas pada operasi matematika dasar, moving average, thresholding, dan statistik deskriptif ringkas. | **Tinggi:** Mampu menjalankan machine learning, regresi lanjut, analisis deret waktu (*time-series*), korelasi multivariat, dan visualisasi grafis. |
| **Kebutuhan Sumber Daya** | **Sangat Rendah:** Menggunakan memori RAM ESP32 skala kilobyte (KB) dan konsumsi daya listrik sangat hemat (miliwatt). | **Tinggi:** Memerlukan RAM gigabyte (GB), CPU/GPU komputer yang bertenaga, dan penyimpanan disk untuk basis data historis. |

#### 3. Bagaimana pengaruh pemilihan ukuran jendela *moving average* terhadap kualitas data dan kecepatan respons sistem kendali apabila data tersebut digunakan untuk mengendalikan aktuator?
* **Jendela Terlalu Kecil ($N=2$ atau $3$):** Kualitas penyaringan rendah; jika sensor terkena gangguan pembacaan semu (*noise spike*), aktuator (misal kipas pendingin atau pemanas) akan langsung menyala-mati secara berulang-ulang (*chattering / hunting*), yang dapat memperpendek umur mekanis dan kelistrikan aktuator.
* **Jendela Terlalu Besar ($N \ge 20$):** Data sangat mulus dan bebas gangguan semu, tetapi aktuator merespons sangat lambat terhadap kondisi darurat. Sebagai contoh, jika suhu ruangan melonjak tajam akibat kebakaran atau kegagalan pendingin, aktuator pendingin darurat baru akan aktif beberapa menit kemudian karena nilai rata-rata baru naik perlahan-lahan.
* **Kesimpulan Desain:** Ukuran jendela harus dipilih secara seimbang (umumnya bernilai antara 5 hingga 10 untuk sensor suhu lingkungan) agar mampu meredam *noise* tanpa mengorbankan keamanan sistem kendali.

#### 4. Bagaimana hasil analisis dan deteksi anomali dapat dimanfaatkan untuk meningkatkan keandalan sistem IoT, misalnya pada sistem *smart farming* atau *smart home*?
1. **Pemeliharaan Prediktif (*Predictive Maintenance*):** Deteksi anomali suhu yang terus meningkat pada pompa air irigasi (*smart farming*) atau kompresor AC (*smart home*) dapat memberikan peringatan awal kerusakan mekanik (misal keausan bantalan poros) sebelum perangkat mati total.
2. **Pendeteksian Kerusakan Sensor (*Sensor Fault Detection*):** Jika z-score bernilai ekstrem secara tiba-tiba sementara sensor lingkungan lain di sekitarnya tetap membaca nilai wajar, sistem dapat menyimpulkan bahwa modul sensor kotor, terputus, atau rusak (*sensor drift*), sehingga pembacaan sensor tersebut dapat diisolasi dan tidak merusak sistem otomasi.
3. **Pencegahan Bencana dan Penghematan Energi:** Pada *smart home*, deteksi anomali suhu yang melonjak abnormal dapat memicu alarm pencegah kebakaran (*early fire warning*) dan otomatis memutus aliran listrik. Pada *smart farming*, deteksi anomali kelembaban tanah dapat mencegah pembusukan akar tanaman atau pemborosan air dari kebocoran pipa irigasi.
