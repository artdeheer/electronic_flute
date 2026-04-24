#include <Arduino.h>
#include <Wire.h>
#include "PressureSensor.h"
#include "CapSensor.h"
#include "IMU.h"

#define MIDI_CHANNEL 1
#define BREATH_CUTOFF 0.4
#define RESET_HOLD_TIME 5000

int lastNote = 0;
unsigned long allPressedStartTime = 0;
bool resetTriggered = false;
uint8_t lastMask = 0;
unsigned long lastDebugTime = 0;

PressureSensor breathSensor(A0, 10000.0, 20000.0, 5.03);
CapSensor fluteKeys(5, 0x29); 
IMU orientation;

int getNote(uint8_t mask) {
    if (mask == 0) return 0;

    // Every bitmask gets a different note pattern.
    // MIDI only supports 0–127, so this wraps into a playable range.
    return 36 + (mask % 60); // notes 36–95
}

void initializeSystem() {
    Wire.begin();
    Wire1.begin();
    breathSensor.begin();
    fluteKeys.begin(&Wire1); 
    orientation.begin();
}

void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("Starting system...");
    initializeSystem();
    Serial.println("System initialized.");
}

void loop() {
    orientation.update();
    float pressure = breathSensor.readKPa();
    if (pressure < 0) pressure = 0;

    // 1. SCAN KEYS
    uint8_t keyMask = 0;
    bool allPressed = true;
    for (uint8_t i = 0; i < 8; i++) {
        if (fluteKeys.isTouched(i)) {
            keyMask |= (1 << i);
        } else {
            allPressed = false;
        }
    }

    // 2. RESET LOGIC
    if (allPressed) {
        if (allPressedStartTime == 0) allPressedStartTime = millis();
        else if (millis() - allPressedStartTime > RESET_HOLD_TIME && !resetTriggered) {
            initializeSystem();
            resetTriggered = true;
        }
    } else {
        allPressedStartTime = 0;
        resetTriggered = false;
    }

    // 3. MIDI LOGIC
    int currentNote = getNote(keyMask);
    int breathVal = constrain(map(pressure * 10, 4, 50, 0, 127), 0, 127);

    usbMIDI.sendControlChange(2, breathVal, MIDI_CHANNEL);
    usbMIDI.sendControlChange(11, breathVal, MIDI_CHANNEL);

    bool blowing = pressure > BREATH_CUTOFF;

    // Update note immediately when the bitmask changes
    if (blowing && keyMask != 0) {
        if (keyMask != lastMask) {
            if (lastNote != 0) {
                usbMIDI.sendNoteOff(lastNote, 0, MIDI_CHANNEL);
            }

            usbMIDI.sendNoteOn(currentNote, breathVal, MIDI_CHANNEL);

            lastNote = currentNote;
            lastMask = keyMask;
        }
    } else {
        if (lastNote != 0) {
            usbMIDI.sendNoteOff(lastNote, 0, MIDI_CHANNEL);
        }

        lastNote = 0;
        lastMask = 0;
    }

    // Debug only 10 times per second so it does not slow MIDI down
    if (millis() - lastDebugTime > 100) {
        lastDebugTime = millis();

        Serial.print("Mask: 0b");
        for (int i = 7; i >= 0; i--) {
            Serial.print((keyMask >> i) & 1);
        }

        Serial.print(" | Decimal: ");
        Serial.print(keyMask);
        Serial.print(" | Note: ");
        Serial.print(currentNote);
        Serial.print(" | Pressure: ");
        Serial.print(pressure);
        Serial.print(" | Breath: ");
        Serial.println(breathVal);
    }

    // 4. PITCH BEND
    int bend = map(constrain(orientation.pitch, -30, 30), -30, 30, 0, 16383);
    usbMIDI.sendPitchBend(bend, MIDI_CHANNEL);

    while (usbMIDI.read());
}