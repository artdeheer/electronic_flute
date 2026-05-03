#include <Arduino.h>
#include <Wire.h>
#include "CapSensor.h"
#include "FluteLogic.h"
#include "IMU.h"
#include "PressureSensor.h"

#define MIDI_CHANNEL 1
#define BREATH_THRESHOLD 0.35  // kPa required to trigger a note
#define RESET_HOLD_TIME 5000

CapSensor fluteKeys(5, 0x29); 
FluteLogic fluteLogic;
IMU orientation;
// Pin A0, Resistor R1, Resistor R2, Supply Voltage
PressureSensor breathSensor(A0, 10000.0, 20000.0, 5.03); 

int lastNote = 0;
uint8_t lastMask = 0;
bool isBlowing = false;
unsigned long lastDebugTime = 0;

void setup() {
    Serial.begin(115200);
    while(!Serial && millis() < 3000); 

    Serial.println("--- RECORDER INITIALIZING ---");

    Wire.begin();   // MPU6050 (IMU) usually on Wire
    Wire1.begin();  // CAP1188 on Wire1
    
    fluteKeys.hardwareReset(); 
    
    if (!fluteKeys.begin(&Wire1)) {
        Serial.println("CAP1188 Init Failed!");
    }
    
    if (!orientation.begin()) {
        Serial.println("IMU (MPU6050) Init Failed!");
    }

    breathSensor.begin();

    Serial.println("System Ready.");
}

void loop() {
    orientation.update();
    float pressureKPa = breathSensor.readKPa();
    uint8_t currentMask = fluteKeys.touched();

    int velocity = fluteLogic.getVelocityFromPressure(pressureKPa);
    bool currentBlowing = (pressureKPa > BREATH_THRESHOLD);

    int currentNote = fluteLogic.getNoteFromFingering(currentMask);

    // Case A: You just started blowing
    if (currentBlowing && !isBlowing) {
        if (currentNote > 0) {
            usbMIDI.sendNoteOn(currentNote, velocity, MIDI_CHANNEL);
            lastNote = currentNote;
            lastMask = currentMask;
            isBlowing = true;
        }
    }
    // Case B: You are blowing but changed your fingering (Legato)
    else if (currentBlowing && isBlowing) {
        if (currentMask != lastMask) {
            if (lastNote > 0) usbMIDI.sendNoteOff(lastNote, 0, MIDI_CHANNEL);
            if (currentNote > 0) usbMIDI.sendNoteOn(currentNote, velocity, MIDI_CHANNEL);
            lastNote = currentNote;
            lastMask = currentMask;
        }
        // Continuous Breath Expression (Breath Controller CC#2)
        usbMIDI.sendControlChange(2, velocity, MIDI_CHANNEL);
    }
    // Case C: You stopped blowing
    else if (!currentBlowing && isBlowing) {
        if (lastNote > 0) usbMIDI.sendNoteOff(lastNote, 0, MIDI_CHANNEL);
        isBlowing = false;
        lastNote = 0;
        lastMask = 0;
    }

    // IMU SPECIAL EFFECT ---
    // Use the tilt (pitch) to control an effect like a Wah or Filter (CC 74)
    int tiltEffect = fluteLogic.getExpressionFromTilt(orientation.pitch);
    usbMIDI.sendControlChange(74, tiltEffect, MIDI_CHANNEL); 
    
    // Optional: Use Roll for Pitch Bend or Modulation
    int rollEffect = fluteLogic.getExpressionFromTilt(orientation.roll);
    usbMIDI.sendControlChange(1, rollEffect, MIDI_CHANNEL); // Modulation wheel

    // LOG
    if (millis() - lastDebugTime > 200) {
        lastDebugTime = millis();
        Serial.print("Note: "); Serial.print(currentNote);
        Serial.print(" | Pressure: "); Serial.print(pressureKPa);
        Serial.print(" | Tilt: "); Serial.println(orientation.pitch);
    }

    while (usbMIDI.read());
}