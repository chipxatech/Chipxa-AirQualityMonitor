/*
  ACD1200 CO2 Sensor - STM32 Standalone Demo
  -------------------------------------------
  Wiring:
  - VCC -> 5V or 3.3V
  - GND -> GND
  - Sensor TX -> PA3 (USART2 RX)
  - Sensor RX -> PA2 (USART2 TX)
*/

#include <Arduino.h>
#include "ACD1200.h"

// STM32 (e.g. BluePill): Use HardwareSerial USART2
HardwareSerial SensorSerial(USART2);
ACD1200 co2Sensor(SensorSerial);

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    
    Serial.println(F("===================================="));
    Serial.println(F("ACD1200 CO2 Sensor Demo - STM32"));
    Serial.println(F("===================================="));

    // Initialize HardwareSerial for sensor
    SensorSerial.begin(1200);
    
    // Initialize library
    co2Sensor.begin(1200);
    Serial.println(F("Sensor serial initialized (USART2). Starting measurements..."));
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
