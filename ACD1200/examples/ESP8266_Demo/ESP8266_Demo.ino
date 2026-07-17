/*
  ACD1200 CO2 Sensor - ESP8266 Standalone Demo
  ---------------------------------------------
  Wiring:
  - VCC -> 5V or 3.3V
  - GND -> GND
  - Sensor TX -> D1 (GPIO5 Software RX)
  - Sensor RX -> D2 (GPIO4 Software TX)
*/

#include <Arduino.h>
#include "ACD1200.h"
#include <SoftwareSerial.h>

// ESP8266: Use SoftwareSerial on D1 & D2
SoftwareSerial swSerial(D1, D2); // RX (D1), TX (D2)
ACD1200 co2Sensor(swSerial);

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    
    Serial.println(F("===================================="));
    Serial.println(F("ACD1200 CO2 Sensor Demo - ESP8266"));
    Serial.println(F("===================================="));

    // Initialize SoftwareSerial
    swSerial.begin(1200);
    
    // Initialize library
    co2Sensor.begin(1200);
    Serial.println(F("Sensor serial initialized (D1/D2). Starting measurements..."));
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
