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

   Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
} 

 // Jika berhasil terhubung
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());
  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());

  digitalWrite(ledPin, HIGH);  // nyalakan LED sebagai indikator
}
  
void loop() { 
 // Cek status koneksi setiap 5 detik
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
  } else {
    Serial.println("Status: Terputus");
    digitalWrite(ledPin, LOW);
  }
  delay(5000);
}