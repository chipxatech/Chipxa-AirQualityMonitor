/*
  ACD1200 CO2 Sensor - ESP32 Standalone Demo
  -------------------------------------------
  Wiring:
  - VCC -> 5V or 3.3V
  - GND -> GND
  - Sensor TX -> GPIO16 (UART2 RX)
  - Sensor RX -> GPIO17 (UART2 TX)
*/

#include <Arduino.h>
#include "ACD1200.h"

// ESP32: Use HardwareSerial UART2 with Custom Pins
HardwareSerial SensorSerial(2);
ACD1200 co2Sensor(SensorSerial);

const int RX_PIN = 16;
const int TX_PIN = 17;

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    
    Serial.println(F("===================================="));
    Serial.println(F("ACD1200 CO2 Sensor Demo - ESP32"));
    Serial.println(F("===================================="));

    // Initialize HardwareSerial for ESP32 with Custom Pins
    SensorSerial.begin(1200, SERIAL_8N1, RX_PIN, TX_PIN);
    
    // Initialize library
    co2Sensor.begin(1200);
    Serial.println(F("Sensor serial initialized (UART2). Starting measurements..."));
}

void loop() {
    static unsigned long lastReadTime = 0;
    
    if (millis() - lastReadTime >= 2000) {
        lastReadTime = millis();
        
        Serial.println(F("------------------------------------"));
        if (co2Sensor.readCO2()) {
            Serial.print(F("Current CO2: "));
            Serial.print(co2Sensor.getCO2());
            Serial.println(F(" ppm"));
            
            Serial.print(F("Average CO2: "));
            Serial.print(co2Sensor.getAverageCO2());
            Serial.println(F(" ppm"));
            
            Serial.print(F("Raw Packet:  "));
            co2Sensor.printRaw(Serial);
        } else {
            Serial.println(F("Error: Failed to read from ACD1200 sensor."));
        }
    }
}
