#include <DHT.h>
#include <math.h>

// ==================== KONFIGURASI SENSOR DHT22 ====================
#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ==================== PENGATURAN MOVING AVERAGE ====================
const int N = 5;       // Jumlah sampel filter moving average
float buffer[N];       // Buffer sirkular untuk menyimpan N pembacaan terakhir
int indeks = 0;        // Penunjuk indeks penulisan berikutnya pada buffer sirkular
int jumlah = 0;        // Jumlah sampel yang telah terisi dalam buffer (maksimal N)

// Variabel pencatat nilai ekstrem
float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Fungsi untuk menghitung nilai rata-rata bergerak (Moving Average)
float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;           // Menyimpan nilai pembacaan baru ke posisi indeks saat ini
  indeks = (indeks + 1) % N;            // Memajukan indeks sirkular (kembali ke 0 saat mencapai N)
  if (jumlah < N) jumlah++;             // Menambah jumlah elemen aktif sampai mencapai kapasitas buffer (N)

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];                 // Menjumlahkan semua sampel yang tersimpan dalam buffer
  }
  return total / jumlah;                // Mengembalikan rata-rata bergerak
}

// Fungsi tugas modifikasi: Menghitung simpangan baku (Standard Deviation) data buffer
float hitungSimpanganBaku(float rataRata) {
  if (jumlah <= 1) return 0.0;          // Simpangan baku 0 jika sampel baru 1 atau kurang

  float jumlahKuadratSelisih = 0.0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rataRata;
    jumlahKuadratSelisih += (selisih * selisih); // Menjumlahkan kuadrat selisih dari rata-rata
  }

  // Menghitung varians sampel dan mengakar kuadratkan untuk mendapatkan standar deviasi
  float varians = jumlahKuadratSelisih / (jumlah - 1);
  return sqrt(varians);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("==================================================");
  Serial.println("Sistem Pengolahan Data Sensor ESP32 (Edge Processing)");
  Serial.println("==================================================");
}

void loop() {
  // Membaca suhu dari sensor DHT22
  float suhu = dht.readTemperature();

  // Validasi pembacaan data
  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    // 1. Hitung filter moving average
    float suhuFilter = hitungMovingAverage(suhu);

    // 2. Hitung simpangan baku (Standar Deviasi)
    float stdDev = hitungSimpanganBaku(suhuFilter);

    // 3. Catat nilai minimum dan maksimum historis
    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    // 4. Tampilkan data ke Serial Monitor
    Serial.print("Mentah: ");
    Serial.print(suhu, 2);
    Serial.print(" °C | Filter: ");
    Serial.print(suhuFilter, 2);
    Serial.print(" °C | StdDev: ");
    Serial.print(stdDev, 3);
    Serial.print(" | Min: ");
    Serial.print(suhuMin, 2);
    Serial.print(" °C | Max: ");
    Serial.print(suhuMax, 2);
    Serial.println(" °C");
  }

  // Interval pembacaan sensor setiap 2 detik
  delay(2000);
}
