#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ==================== KONFIGURASI WIFI & MQTT ====================
const char* ssid          = "Hazee";
const char* password      = "abcd1234";
const char* mqttServer    = "broker.hivemq.com";
const int   mqttPort      = 1883;
const char* topicPerintah = "unsoed/tk245004/kelompok2/perintah";

// Pin Aktuator LED
const int ledPin = 26;

// Konfigurasi PWM LEDC ESP32
const int pwmChannel   = 0;
const int pwmFreq      = 5000;
const int pwmResolusi  = 8; // 8-bit: rentang nilai 0 - 255

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi callback dipanggil otomatis setiap ada pesan baru masuk pada topic yang di-subscribe
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // Deserialisasi data JSON yang diterima
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);

  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Mengambil parameter 'perintah' dari JSON
  const char* perintah = doc["perintah"];

  // Mengecek apakah payload memuat parameter 'intensitas' (0 - 255)
  int intensitas = 255; // Default intensitas penuh
  if (doc.containsKey("intensitas")) {
    intensitas = doc["intensitas"];
  }

  // Kendali aktuator berdasarkan perintah yang diterima
  if (String(perintah) == "ON") {
    ledcWrite(pwmChannel, intensitas); // Menyalakan LED dengan intensitas PWM tertentu
    Serial.print("Aktuator: ON | Intensitas: ");
    Serial.println(intensitas);
  } else if (String(perintah) == "OFF") {
    ledcWrite(pwmChannel, 0);          // Mematikan LED (duty cycle 0)
    Serial.println("Aktuator: OFF | Intensitas: 0");
  } else {
    Serial.print("Perintah tidak dikenali: ");
    Serial.println(perintah);
  }
}

// Fungsi untuk menghubungkan ESP32 ke jaringan WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// Fungsi untuk menghubungkan dan reconnect ke broker MQTT
void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
      // Mendaftarkan subscription setelah koneksi berhasil terbentuk
      client.subscribe(topicPerintah);
      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Konfigurasi pin PWM untuk LED pada ESP32
  ledcSetup(pwmChannel, pwmFreq, pwmResolusi);
  ledcAttachPin(ledPin, pwmChannel);
  ledcWrite(pwmChannel, 0); // Pastikan LED mati saat awal mulai

  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback); // Mendaftarkan fungsi callback
}

void loop() {
  // Menjaga agar koneksi MQTT tetap aktif
  if (!client.connected()) {
    hubungkanMQTT();
  }
  // Memproses antrean paket MQTT yang masuk secara terus-menerus
  client.loop();
}
