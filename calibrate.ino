#include <WiFiManager.h>

const int FSR_PIN = 2;  // GPIO2 connected to the voltage divider
const float VCC = 3.3;  // ESP32 operating voltage
const int R_FIXED = 10000;  // 10k ohm resistor
const float THRESHOLD = 0.1;  // Minimum weight change (kg) to wake up ESP32

float lastForceKg = 0;
WiFiManager wifiManager;

void setup() {
  WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    analogReadResolution(12); // Set ADC resolution to 12-bit (0-4095)
    
    bool rs=wifiManager.autoConnect("ESP32-Setup","password"); // Creates an AP if no WiFi is configured
    if(!rs){
      Serial.println("Try again");
           }
    else{
      Serial.println("Connected to WiFi!");
           }
}

void enterDeepSleep() {
    Serial.println("Entering deep sleep mode...");
    esp_sleep_enable_timer_wakeup(1000000);  // Wake up periodically (adjust if needed)
    esp_deep_sleep_start();
}

void loop() {
    int adcValue = analogRead(FSR_PIN);
    float voltage = (adcValue / 4095.0) * VCC;
    float fsrResistance = (VCC * R_FIXED / voltage) - R_FIXED;
    
    // Approximate force calculation (adjust based on calibration)
    float forceKg = fsrResistance > 0 ? (10000.0 / fsrResistance) : 0;
    
    if (abs(forceKg - lastForceKg) >= THRESHOLD) {
        Serial.print("FSR Value: ");
        Serial.print(adcValue);
        Serial.print(" | Voltage: ");
        Serial.print(voltage, 3);
        Serial.print("V | Force: ");
        Serial.print(forceKg, 3);
        Serial.println(" kg");
        
        lastForceKg = forceKg;
    } else {
        enterDeepSleep();
    }
    
    delay(500);  // Adjust delay as needed
}
