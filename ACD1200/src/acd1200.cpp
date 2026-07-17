// ACD1200.cpp
// ==========================
#include "ACD1200.h"

const uint8_t ACD1200::CMD_READ_CO2[5] = {
    0xFE, 0xA6, 0x00, 0x01, 0xA7
};

ACD1200::ACD1200(Stream &serialPort) {
    _serial = &serialPort;

    _co2 = 0;
    _avgCO2 = 0;

    _sampleIndex = 0;
    _bufferFilled = false;

    memset(_buffer, 0, sizeof(_buffer));
    memset(_rawData, 0, sizeof(_rawData));
}

void ACD1200::begin(uint32_t baud) {
    // Note: Since we use generic Stream, you must call SerialPort.begin(1200) 
    // in your setup() sketch.
}

uint8_t ACD1200::calcChecksum(const uint8_t *data, uint8_t lenWithoutCS) {
    uint16_t sum = 0;

    for (uint8_t i = 1; i < lenWithoutCS; i++) {
        sum += data[i];
    }

    return (uint8_t)(sum & 0xFF);
}

void ACD1200::clearBuffer() {
    while (_serial->available()) {
        _serial->read();
    }
}

void ACD1200::addSample(uint16_t value) {
    _buffer[_sampleIndex] = value;

    _sampleIndex++;

    if (_sampleIndex >= AVG_SAMPLES) {
        _sampleIndex = 0;
        _bufferFilled = true;
    }

    uint32_t sum = 0;
    uint8_t count = _bufferFilled ? AVG_SAMPLES : _sampleIndex;

    for (uint8_t i = 0; i < count; i++) {
        sum += _buffer[i];
    }

    if (count > 0) {
        _avgCO2 = sum / count;
    }
}

bool ACD1200::readCO2() {
    clearBuffer();

    _serial->write(CMD_READ_CO2, sizeof(CMD_READ_CO2));
    _serial->flush();

    uint8_t idx = 0;
    unsigned long startTime = millis();

    while ((millis() - startTime) < 2000 && idx < sizeof(_rawData)) {
        if (_serial->available()) {
            _rawData[idx++] = _serial->read();
        }
    }

    // Must be exactly 9 bytes
    if (idx != 9) {
        return false;
    }

    // Header check
    if (_rawData[0] != 0xFE ||
        _rawData[1] != 0xA6 ||
        _rawData[2] != 0x04 ||
        _rawData[3] != 0x01) {
        return false;
    }

    // Checksum
    uint8_t expectedCS = calcChecksum(_rawData, 8);

    if (expectedCS != _rawData[8]) {
        return false;
    }

    // Parse CO2
    _co2 = ((uint16_t)_rawData[4] << 8) | _rawData[5];

    addSample(_co2);

    return true;
}

uint16_t ACD1200::getCO2() const {
    return _co2;
}

uint16_t ACD1200::getAverageCO2() const {
    return _avgCO2;
}

uint8_t* ACD1200::getRawData() {
    return _rawData;
}

void ACD1200::printRaw(Stream &out) {
    for (uint8_t i = 0; i < sizeof(_rawData); i++) {
        if (_rawData[i] < 0x10) out.print("0");
        out.print(_rawData[i], HEX);
        out.print(" ");
    }
    out.println();
}
