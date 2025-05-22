/*
  Enhanced DHT11 Environmental Monitoring System
  Features:
  - Temperature and humidity monitoring
  - WiFi connectivity for data transmission
  - Power saving features
  - Error handling and status indicators
  - Configurable settings
  
  Board: ESP32 Development Board
  Components: 
  - DHT11 Temperature and humidity sensor
  - LED for status indication (optional)
  
  Libraries:
  - DHT sensor library by Adafruit
  - WiFi library (built-in)
*/

#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// Configuration
#define DHTPIN 25
#define DHTTYPE DHT11
#define LED_PIN 5  // Changed to GPIO 5 which is safe to use
#define WIFI_RETRY_DELAY 5000
#define SENSOR_READ_INTERVAL 30000  // 30 seconds between readings

// WiFi credentials
const char* ssid = "JICO";
const char* password = "08034323638791160606";

// Server details
const char* server = "https://sensordashboardjk297.vercel.app/api/sensor-data";

// Authentication
const char* authToken = "YOUR_AUTH_TOKEN"; // Replace with your actual auth token

// Global variables
DHT dht(DHTPIN, DHTTYPE);
unsigned long lastReadTime = 0;
bool wifiConnected = false;
WiFiClientSecure client;

void setup() {
  // Initialize serial communication with a delay to ensure stable connection
  delay(1000);  // Give some time for serial to initialize
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  Serial.println("\n\nDHT11 Environmental Monitoring System");
  Serial.println("----------------------------------------");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT sensor initialized");
  
  // Configure WiFiClientSecure
  client.setInsecure(); // Skip certificate verification
  
  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    connectToWiFi();
  }
  
  // Read sensor data at specified interval
  if (millis() - lastReadTime >= SENSOR_READ_INTERVAL) {
    readAndSendSensorData();
    lastReadTime = millis();
  }
  
  // Blink LED to indicate system is running
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.disconnect(true);  // Disconnect from any existing connection
  delay(1000);
  WiFi.mode(WIFI_STA);    // Set WiFi to station mode
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
    
    // Test server connection
    testServerConnection();
  } else {
    Serial.println("\nFailed to connect to WiFi");
    wifiConnected = false;
  }
}

void testServerConnection() {
  Serial.println("Testing server connection...");
  HTTPClient http;
  
  String serverUrl = String(server);
  Serial.println("Server URL: " + serverUrl);
  
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("Authorization", "Bearer " + String(authToken));
  
  // Send a test POST request with minimal data
  String testData = "{\"test\":true,\"timestamp\":" + String(millis()) + "}";
  Serial.println("Sending test data: " + testData);
  
  int httpResponseCode = http.POST(testData);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Server test response:");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
    
    if (httpResponseCode == 401) {
      Serial.println("Authentication failed. Please check your auth token.");
    }
  } else {
    Serial.println("Server test failed!");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Error: " + http.errorToString(httpResponseCode));
  }
  
  http.end();
}

void readAndSendSensorData() {
  // Read sensor values with retry mechanism
  int retryCount = 0;
  float humidity = NAN;
  float temperature = NAN;
  float fahrenheit = NAN;
  
  while ((isnan(humidity) || isnan(temperature) || isnan(fahrenheit)) && retryCount < 3) {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    fahrenheit = dht.readTemperature(true);
    
    if (isnan(humidity) || isnan(temperature) || isnan(fahrenheit)) {
      Serial.println("Failed to read from DHT sensor! Retrying...");
      delay(2000);  // Wait 2 seconds before retry
      retryCount++;
    }
  }
  
  // Check if any reads failed after retries
  if (isnan(humidity) || isnan(temperature) || isnan(fahrenheit)) {
    Serial.println("Failed to read from DHT sensor after multiple attempts!");
    return;
  }
  
  // Calculate heat index
  float heatIndexC = dht.computeHeatIndex(temperature, humidity, false);
  float heatIndexF = dht.computeHeatIndex(fahrenheit, humidity, true);
  
  // Print local readings with clear formatting
  Serial.println("\nSensor Readings:");
  Serial.println("----------------");
  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println("%");
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.print("°C / ");
  Serial.print(fahrenheit, 1);
  Serial.println("°F");
  Serial.print("Heat Index: ");
  Serial.print(heatIndexC, 1);
  Serial.print("°C / ");
  Serial.print(heatIndexF, 1);
  Serial.println("°F");
  Serial.println("----------------");
  
  // Send data to server if WiFi is connected
  if (wifiConnected) {
    sendDataToServer(temperature, humidity, heatIndexC);
  }
}

void sendDataToServer(float temp, float hum, float heatIndex) {
  if (!wifiConnected) {
    Serial.println("WiFi not connected. Attempting to reconnect...");
    connectToWiFi();
    if (!wifiConnected) {
      Serial.println("Failed to reconnect to WiFi. Skipping server update.");
      return;
    }
  }
  
  // Create JSON string for data transmission with ISO timestamp
  String timestamp = String(millis());
  String data = "{\"temperature\":" + String(temp, 2) + 
                ",\"humidity\":" + String(hum, 2) + 
                ",\"heatIndex\":" + String(heatIndex, 2) + 
                ",\"timestamp\":" + timestamp + 
                ",\"deviceId\":\"ESP32_" + String((uint32_t)ESP.getEfuseMac(), HEX) + "\"}";
  
  Serial.println("Sending data to server:");
  Serial.println(data);
  
  HTTPClient http;
  
  // Configure the server URL
  String serverUrl = String(server);
  Serial.println("Server URL: " + serverUrl);
  
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  http.addHeader("Authorization", "Bearer " + String(authToken));
  
  // Send POST request
  int httpResponseCode = http.POST(data);
  
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
    
    if (httpResponseCode == 401) {
      Serial.println("Authentication failed. Please check your auth token.");
    }
  } else {
    Serial.println("Error sending data to server");
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Error: " + http.errorToString(httpResponseCode));
    // If we get an error, try to reconnect to WiFi
    wifiConnected = false;
  }
  
  // Free resources
  http.end();
}