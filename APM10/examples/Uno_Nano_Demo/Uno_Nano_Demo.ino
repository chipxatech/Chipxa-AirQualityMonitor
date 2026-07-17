/*
  APM10 Particulate Matter Sensor - Uno/Nano Standalone Demo
  ---------------------------------------------------------
  Supports both I2C and UART interfaces.

  Wiring (I2C Mode):
  - SDA -> A4
  - SCL -> A5

  Wiring (UART Mode):
  - Sensor TX -> Pin 2 (Software RX)
  - Sensor RX -> Pin 3 (Software TX)
*/

#include <Arduino.h>
#include "APM10.h"

// Set USE_I2C to 1 for I2C mode, or 0 for UART mode
#define USE_I2C 1

APM10 pmSensor;

#if !USE_I2C
#include <SoftwareSerial.h>
// Uno/Nano: Use SoftwareSerial on Pin 2 & 3
SoftwareSerial swSerial(2, 3); // RX (Pin 2), TX (Pin 3)
#endif

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    
    Serial.println(F("===================================="));
    Serial.println(F("APM10 PM Sensor Demo - Uno/Nano"));
    Serial.print(F("Mode: "));
    Serial.println(USE_I2C ? F("I2C") : F("UART (SoftwareSerial Pin 2/3)"));
    Serial.println(F("===================================="));

#if USE_I2C
    Wire.begin();

    if (pmSensor.beginI2C(APM10_ADDRESS, Wire)) {
        Serial.println(F("APM10 initialized via I2C."));
        pmSensor.startMeasurement();
    } else {
        Serial.println(F("Error: APM10 I2C connection failed."));
        while (1);
    }
#else
    swSerial.begin(1200);
    if (pmSensor.beginUART(swSerial)) {
        Serial.println(F("APM10 initialized via SoftwareSerial."));
    } else {
        Serial.println(F("Error: APM10 UART connection failed."));
        while (1);
    }
#endif
    Serial.println(F("Starting data collection..."));
}

void loop() {
    static unsigned long lastReadTime = 0;
    
    if (millis() - lastReadTime >= 2000) {
        lastReadTime = millis();
        
        APM10::pmData data;
        Serial.println(F("------------------------------------"));
        
        if (pmSensor.collectData(data)) {
            Serial.print(F("PM1.0: ")); Serial.print(data._PM1_0); Serial.println(F(" ug/m3"));
            Serial.print(F("PM2.5: ")); Serial.print(data._PM2_5); Serial.println(F(" ug/m3"));
            Serial.print(F("PM10 : ")); Serial.print(data._PM10); Serial.println(F(" ug/m3"));
            
            int aqi25 = pmSensor.PM2_5_TO_AQI(data._PM2_5);
            Serial.print(F("AQI (PM2.5): ")); Serial.print(aqi25);
            if (aqi25 <= 50) Serial.println(F(" (GOOD)"));
            else if (aqi25 <= 100) Serial.println(F(" (MODERATE)"));
            else if (aqi25 <= 150) Serial.println(F(" (UNHEALTHY FOR SENSITIVE)"));
            else Serial.println(F(" (DANGER/UNHEALTHY)"));
        } else {
            Serial.println(F("Error: Failed to read from APM10 sensor."));
        }
    }
}
