#define CAP_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_CAP1188.h>

class CapSensor {
private:
    int _resetPin;
    uint8_t _address;
    TwoWire* _wire;

public:
    CapSensor(int resetPin, uint8_t address = 0x29) 
        : _resetPin(resetPin), _address(address) {}

    void hardwareReset() {
        if (_resetPin != -1) {
            pinMode(_resetPin, OUTPUT);
            digitalWrite(_resetPin, HIGH);
            delay(20);
            digitalWrite(_resetPin, LOW);
            delay(150); 
        }
    }

    bool begin(TwoWire *theWire = &Wire1) {
        _wire = theWire;
        
        _wire->beginTransmission(_address);
        if (_wire->endTransmission() != 0) return false;

        // 1. Set sensitivity
        writeRegister(0x1F, 0x04); 

        // 2. ENABLE THE LEDS: Link all 8 sensors to their corresponding LEDs
        writeRegister(0x72, 0xFF); 

        // 3. Optional: Set LED behavior to "Direct" (instantly on/off)
        writeRegister(0x81, 0x00); 

        return true;
    }

    void writeRegister(uint8_t reg, uint8_t value) {
        _wire->beginTransmission(_address);
        _wire->write(reg);
        _wire->write(value);
        _wire->endTransmission();
    }

    uint8_t touched() {
        _wire->beginTransmission(_address);
        _wire->write(0x03); 
        _wire->endTransmission(false);
        _wire->requestFrom(_address, (uint8_t)1);
        return _wire->available() ? _wire->read() : 0;
    }
};