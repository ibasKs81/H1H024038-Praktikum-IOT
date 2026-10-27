#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

// ==================== KONFIGURASI WIFI & MQTT ====================
const char* ssid                 = "Hazee";
const char* password             = "abcd1234";
const char* mqttServer           = "broker.hivemq.com";
const int   mqttPort             = 1883;

// Topic MQTT
const char* topicData            = "unsoed/tk245004/kelompok2/data";
const char* topicPerintahLED     = "unsoed/tk245004/kelompok2/perintah";
const char* topicPerintahBuzzer  = "unsoed/tk245004/kelompok2/perintah_buzzer";

// Konfigurasi Pin Sensor & Aktuator
#define DHTPIN 4
#define DHTTYPE DHT22
const int ledPin    = 26; // Aktuator 1: LED
const int buzzerPin = 27; // Aktuator 2: Buzzer (sesuai tugas modifikasi)

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Pengaturan Interval Waktu Non-Blocking (millis)
unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // Publish data setiap 5 detik

// Callback function: Menangani pesan masuk dari semua topic yang di-subscribe
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan = "";
  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima pada topic [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  // Deserialisasi JSON payload
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, pesan);
  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Membedakan aksi berdasarkan topic sumber pesan
  if (strcmp(topic, topicPerintahLED) == 0) {
    const char* perintah = doc["perintah"];
    if (String(perintah) == "ON") {
      digitalWrite(ledPin, HIGH);
      Serial.println("Perintah Diterima -> LED: ON");
    } else if (String(perintah) == "OFF") {
      digitalWrite(ledPin, LOW);
      Serial.println("Perintah Diterima -> LED: OFF");
    }
  } 
  else if (strcmp(topic, topicPerintahBuzzer) == 0) {
    const char* perintah = doc["perintah"];
    if (String(perintah) == "ON") {
      digitalWrite(buzzerPin, HIGH);
      Serial.println("Perintah Diterima -> Buzzer: ON");
    } else if (String(perintah) == "OFF") {
      digitalWrite(buzzerPin, LOW);
      Serial.println("Perintah Diterima -> Buzzer: OFF");
    }
  }
}

// Menghubungkan ESP32 ke jaringan WiFi
void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

// Menghubungkan ke broker MQTT dan mendaftarkan topic subscribe
void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    Serial.print("Menghubungkan ke broker MQTT...");
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");

      // Mendaftarkan langganan (subscribe) topic LED dan Buzzer
      client.subscribe(topicPerintahLED);
      client.subscribe(topicPerintahBuzzer);

      Serial.print("Subscribed: ");
      Serial.println(topicPerintahLED);
      Serial.print("Subscribed: ");
      Serial.println(topicPerintahBuzzer);
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Inisialisasi pin output
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // Inisialisasi sensor DHT22
  dht.begin();

  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  // Pastikan koneksi MQTT selalu terjaga
  if (!client.connected()) {
    hubungkanMQTT();
  }

  // Wajib dipanggil terus-menerus untuk memproses incoming packet MQTT secara non-blocking
  client.loop();

  // Publish data sensor secara berkala non-blocking menggunakan millis()
  if (millis() - waktuTerakhirPublish >= intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = dht.readTemperature();
    float kelembaban = dht.readHumidity();

    if (!isnan(suhu) && !isnan(kelembaban)) {
      JsonDocument doc;
      doc["suhu"] = suhu;
      doc["kelembaban"] = kelembaban;

      char buffer[128];
      serializeJson(doc, buffer);

      client.publish(topicData, buffer);
      Serial.print("Data terkirim -> ");
      Serial.println(buffer);
    } else {
      Serial.println("Gagal membaca data dari sensor DHT22!");
    }
  }
}
