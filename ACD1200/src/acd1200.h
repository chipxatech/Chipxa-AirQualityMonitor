// ============================================================================
// ACD1200.h
// Thư viện giao tiếp cảm biến CO2 ACD1200
// Phát triển bởi Công ty TNHH Công nghệ Chipxa (Chipxa Technology Co., Ltd.)
// Website: https://chipxa.com | Email: contact@chipxa.com
// ============================================================================
#ifndef ACD1200_H
#define ACD1200_H

#include <Arduino.h>

class ACD1200 {
public:
    // Constructor
    ACD1200(Stream &serialPort);

    // Initialize sensor
    void begin(uint32_t baud = 1200);

    // Read CO2 value (ppm)
    bool readCO2();

    // Get latest CO2 value
    uint16_t getCO2() const;

    // Get moving average
    uint16_t getAverageCO2() const;

    // Get last raw packet
    uint8_t* getRawData();

    // Debug print raw packet
    void printRaw(Stream &out = Serial);

private:
    Stream* _serial;

    static const uint8_t CMD_READ_CO2[5];

    uint16_t _co2;
    uint16_t _avgCO2;

    uint8_t _rawData[9];

    static const uint8_t AVG_SAMPLES = 5;
    uint16_t _buffer[AVG_SAMPLES];
    uint8_t _sampleIndex;
    bool _bufferFilled;

    uint8_t calcChecksum(const uint8_t *data, uint8_t lenWithoutCS);
    void clearBuffer();
    void addSample(uint16_t value);
};

#endif
