#ifndef FLUTE_LOGIC_H
#define FLUTE_LOGIC_H

#include <Arduino.h>

class FluteLogic {
public:
    // This function takes the bitmask of touched keys and returns a MIDI note
    // A bitmask is a single number where each bit represents one of your 8 keys
    int getNoteFromFingering(uint8_t keyMask) {
        // Example Fingering Chart (Binary representation of 8 keys)
        // B7 B6 B5 B4 B3 B2 B1 B0
        
        switch (keyMask) {
            case 0b11111111: return 60; // All keys down = Middle C
            case 0b01111111: return 62; // Top key up = D
            case 0b00111111: return 64; // Two keys up = E
            case 0b00000000: return 0;  // No keys = No note
            default: return -1;         // Undefined fingering
        }
    }

    // This converts your IMU pitch into a MIDI Pitch Bend or Expression
    int getExpressionFromTilt(float pitch) {
        // Map -45 to 45 degrees to 0-127 MIDI value
        int expression = map(constrain(pitch, -45, 45), -45, 45, 0, 127);
        return expression;
    }

    // This converts pressure (kPa) into Velocity (how hard you blow)
    int getVelocityFromPressure(float pressureKPa) {
        // Assume 0.0 to 3.0 kPa is our blowing range
        int velocity = map(constrain(pressureKPa * 10, 0, 30), 0, 30, 0, 127);
        return velocity;
    }
};

#endif