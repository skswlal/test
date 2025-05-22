#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>
#include <HTTPClient.h>
#include "time.h"
#include <WiFiClientSecure.h>

// Wi-Fi credentials
#define WIFI_SSID "JICO"
#define WIFI_PASSWORD "08034323638791160606"

// Backend server URL - Vercel deployment
#define SERVER_URL "https://sensordashboardjk297-4wd7hy0jb-jimi-kims-projects.vercel.app/api/sensor-data"

// DHT11 sensor pin - Changed from GPIO 15 to GPIO 4 to avoid conflicts
#define DHTPIN 4       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// WiFi connection timeout
#define WIFI_TIMEOUT_MS 20000 // 20 seconds timeout

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  Serial.println("Test message");
  delay(1000);
}
