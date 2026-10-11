#include <ESP8266WiFi.h> 
  
const char* ssid     = "Hazee";
const char* password = "abcd123";
  
void setup() { 
  Serial.begin(115200); 
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, LOW); // Mematikan LED saat awal
  
  //setwifi
  Wifi.mode(WIFI_STA);
  Wifi.begin(ssid, password);
} 
  
void loop() { 
  // Membaca data kelembaban dan suhu 
  float kelembaban = dht.readHumidity(); 
  float suhu       = dht.readTemperature(); 
  
  // Periksa apakah pembacaan berhasil 
  if (isnan(kelembaban) || isnan(suhu)) { 
    Serial.println("Gagal membaca data dari sensor DHT22!"); 
  } else { 
    Serial.print("Suhu: "); 
    Serial.print(suhu); 
    Serial.print(" °C, Kelembaban: "); 
    Serial.print(kelembaban); 
    Serial.println(" %"); 
} 
delay(2000);   //jeda pembacaan setiap 2 detik 
} 