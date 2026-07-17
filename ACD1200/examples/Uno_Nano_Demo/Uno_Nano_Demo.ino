/*
  ACD1200 CO2 Sensor - Arduino Uno/Nano Standalone Demo
  -------------------------------------------------------
  Wiring:
  - VCC -> 5V
  - GND -> GND
  - Sensor TX -> Pin 2 (Software RX)
  - Sensor RX -> Pin 3 (Software TX)
*/

#include <Arduino.h>
#include "ACD1200.h"
#include <SoftwareSerial.h>

// Uno/Nano: Use SoftwareSerial on Pin 2 and Pin 3
SoftwareSerial swSerial(2, 3); // RX (Pin 2), TX (Pin 3)
ACD1200 co2Sensor(swSerial);

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    
    Serial.println(F("===================================="));
    Serial.println(F("ACD1200 CO2 Demo - Arduino Uno/Nano"));
    Serial.println(F("===================================="));

    // Initialize SoftwareSerial
    swSerial.begin(1200);
    
    // Initialize library
    co2Sensor.begin(1200);
    Serial.println(F("Sensor serial initialized (Pin 2/3). Starting measurements..."));
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
