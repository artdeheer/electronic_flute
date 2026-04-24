#ifndef PRESSURE_SENSOR_H
#define PRESSURE_SENSOR_H

#include <Arduino.h>

class PressureSensor {
private:
    int _pin;
    float _vSupply;
    float _dividerFactor;
    
    // Smoothing/Filtering variables
    static const int NUM_SAMPLES = 16;
    int _samples[NUM_SAMPLES];
    int _sampleIndex = 0;
    long _sampleSum = 0;

    const float ADC_REF_VOLT = 3.3;
    const float ADC_COUNTS = 1023.0;

public:
    // Constructor: Sets up the pin and math constants
    PressureSensor(int pin, float rTop, float rBottom, float vSupply) {
        _pin = pin;
        _vSupply = vSupply;
        _dividerFactor = (rTop + rBottom) / rBottom;
        
        // Initialize array to zero
        for (int i = 0; i < NUM_SAMPLES; i++) _samples[i] = 0;
    }

    void begin() {
        pinMode(_pin, INPUT);
        analogReadResolution(10);
        analogReadAveraging(16);
    }

    // Updates the moving average and returns the latest kPa
    float readKPa() {
        int raw = analogRead(_pin);

        // Moving average logic
        _sampleSum -= _samples[_sampleIndex];
        _samples[_sampleIndex] = raw;
        _sampleSum += raw;
        _sampleIndex = (_sampleIndex + 1) % NUM_SAMPLES;

        float rawFiltered = (float)_sampleSum / NUM_SAMPLES;
        float vTeensy = (rawFiltered * ADC_REF_VOLT) / ADC_COUNTS;
        float vSensor = vTeensy * _dividerFactor;

        // MPXV7002DP Formula: P(kPa) = (Vout/Vs - 0.5) / 0.2
        return ((vSensor / _vSupply) - 0.5) / 0.2;
    }

    // Helper to get mmH2O
    float getmmH2O(float kpa) {
        return (kpa * 1000.0) / 9.80665;
    }
};

#endif