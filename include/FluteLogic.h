#ifndef FLUTE_LOGIC_H
#define FLUTE_LOGIC_H

#include <Arduino.h>
#include "FingeringMap.h"

class FluteLogic {
public:

    int getNoteFromFingering(uint8_t keyMask) {
        return RECORDER_MAP[keyMask];
    }

    int getExpressionFromTilt(float pitch) {
        // Map -45 to 45 degrees to 0-127 MIDI value
        int expression = map(constrain(pitch, -45, 45), -45, 45, 0, 127);
        return expression;
    }

    int getVelocityFromPressure(float pressureKPa) {
        // Assume 0.0 to 3.0 kPa is our blowing range
        int velocity = map(constrain(pressureKPa * 10, 0, 30), 0, 30, 0, 127);
        return velocity;
    }
};

#endif