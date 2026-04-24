#ifndef CAP_SENSOR_H
#define CAP_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_CAP1188.h>

class CapSensor {
private:
    Adafruit_CAP1188 _cap;
    int _resetPin;
    uint8_t _address;

public:
    // The resetPin is passed here (e.g., 5)
    CapSensor(int resetPin, uint8_t address = 0x29) 
        : _cap(resetPin), _resetPin(resetPin), _address(address) {}

    // Performs a physical hardware reset on the CAP1188
    void hardwareReset() {
        if (_resetPin != -1) {
            pinMode(_resetPin, OUTPUT);
            digitalWrite(_resetPin, HIGH);
            delay(10);
            digitalWrite(_resetPin, LOW);
            delay(100); // Wait for the chip to reboot and re-calibrate
        }
    }

    void begin(TwoWire *theWire = &Wire1) {
        // Trigger the hardware reset before attempting to communicate
        hardwareReset();
        
        if (!_cap.begin(_address, theWire)) {
            Serial.println("CAP1188 not found!");
            while (1) delay(10);
        }
        
        // Set sensitivity (0x04 is high sensitivity)
        _cap.writeRegister(0x1F, 0x04); 
        Serial.println("CAP1188 Online (Hardware Reset Complete)");
    }

    // Get the raw Delta Count for a channel (0-7)
    int8_t getRawValue(uint8_t channel) {
        if (channel > 7) return 0;
        // Delta counts are 2's complement (signed)
        return (int8_t)_cap.readRegister(0x10 + channel);
    }

    bool isTouched(uint8_t channel) {
        return (_cap.touched() & (1 << channel));
    }
};

#endif