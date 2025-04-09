#include <WiFiManager.h>
#include <HTTPClient.h>

const int FSR_PIN = 2;               // GPIO2 connected to the voltage divider
const float VCC = 3.3;               // ESP32 operating voltage
const int R_FIXED = 10000;           // 10k ohm resistor
const float THRESHOLD = 0.1;         // Minimum weight change (kg) to trigger send

float lastForceKg = 0;
WiFiManager wifiManager;

// Replace with your real server URL
const char* serverUrl = "http://yourdomain.com/upload.php";

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  analogReadResolution(12); // 12-bit ADC

  bool rs = wifiManager.autoConnect("ESP32-Setup", "password");
  if (!rs) {
    Serial.println("WiFi failed to connect. Try again.");
  } else {
    Serial.println("Connected to WiFi!");
  }
}

void sendForceToServer(float forceKg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "force=" + String(forceKg, 3);
    int responseCode = http.POST(postData);

    if (responseCode > 0) {
      Serial.print("Data sent. Server response: ");
      Serial.println(responseCode);
    } else {
      Serial.print("Failed to send data. Error: ");
      Serial.println(http.errorToString(responseCode));
    }

    http.end();
  }
}

void enterDeepSleep() {
  Serial.println("Entering deep sleep mode...");
  esp_sleep_enable_timer_wakeup(1000000);  // Wake after 1 second
  esp_deep_sleep_start();
}

void loop() {
  int adcValue = analogRead(FSR_PIN);
  float voltage = (adcValue / 4095.0) * VCC;
  float fsrResistance = (VCC * R_FIXED / voltage) - R_FIXED;

  float forceKg = fsrResistance > 0 ? (10000.0 / fsrResistance) : 0;

  if (abs(forceKg - lastForceKg) >= THRESHOLD) {
    Serial.print("Force: ");
    Serial.print(forceKg, 3);
    Serial.println(" kg");

    sendForceToServer(forceKg);
    lastForceKg = forceKg;
  } else {
    enterDeepSleep();
  }

  delay(500);
}
