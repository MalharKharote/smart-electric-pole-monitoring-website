#include <WiFi.h>
#include <HTTPClient.h>

// ===== WiFi Credentials =====
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// ===== Flask Server URL =====
const char* serverUrl = "http://172.18.176.84:5000/update_pole";

// ===== Pin Definitions =====
const int analogPin = 34;
const int greenLED = 2;
const int redLED = 4;
const int buzzer = 5;

// ===== Variables =====
int lastStatus = -1;  // -1 = unknown, 0 = fault, 1 = normal

void setup() {
  Serial.begin(115200);

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);

  // ===== Connect WiFi =====
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi!");
}

void loop() {

  int minVal = 4095;
  int maxVal = 0;

  // ===== Sample AC signal =====
  for (int i = 0; i < 100; i++) {
    int val = analogRead(analogPin);

    if (val < minVal) minVal = val;
    if (val > maxVal) maxVal = val;

    delayMicroseconds(200);
  }

  int diff = maxVal - minVal;

  Serial.print("Signal diff: ");
  Serial.println(diff);

  int currentStatus;

  // ===== Fault Detection =====
  if (diff > 150) {
    currentStatus = 1; // normal

    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzer, LOW);

    Serial.println("AC Signal Detected → NORMAL");

  } else {
    currentStatus = 0; // fault

    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    digitalWrite(buzzer, HIGH);

    Serial.println("No AC Signal → FAULT");
  }

  // ===== Send only if status changed =====
  if (currentStatus != lastStatus) {

    if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonData = "{";
      jsonData += "\"pole_id\":\"P02\",";   // change ID per device
      jsonData += "\"status\":\"";
      jsonData += (currentStatus == 1 ? "normal" : "fault");
      jsonData += "\"}";

      int httpResponseCode = http.POST(jsonData);

      Serial.print("HTTP Response: ");
      Serial.println(httpResponseCode);

      http.end();

    } else {
      Serial.println("WiFi Disconnected!");
    }

    lastStatus = currentStatus;
  }

  delay(2000); // check every 2 seconds
}