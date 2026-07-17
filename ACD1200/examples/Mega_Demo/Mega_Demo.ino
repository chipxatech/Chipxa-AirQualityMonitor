/*
  ACD1200 CO2 Sensor - Arduino Mega 2560 Standalone Demo
  -------------------------------------------------------
  Wiring:
  - VCC -> 5V
  - GND -> GND
  - Sensor TX -> Pin 19 (Serial1 RX1)
  - Sensor RX -> Pin 18 (Serial1 TX1)
*/

#include <Arduino.h>
#include "ACD1200.h"

// Mega: Use HardwareSerial Serial1
ACD1200 co2Sensor(Serial1);

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    
    Serial.println(F("===================================="));
    Serial.println(F("ACD1200 CO2 Demo - Arduino Mega 2560"));
    Serial.println(F("===================================="));

    // Initialize HardwareSerial Serial1
    Serial1.begin(1200);
    
    // Initialize library
    co2Sensor.begin(1200);
    Serial.println(F("Sensor serial initialized (Serial1). Starting measurements..."));
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
