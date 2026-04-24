#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Wire.h>

class IMU {
private:
    const uint8_t MPU_ADDR = 0x68;
    const float ACCEL_SCALE = 16384.0;
    const float GYRO_SCALE  = 131.0;
    const float ALPHA       = 0.98;

    unsigned long lastTime;
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

public:
    // Filtered Orientation
    float pitch, roll, yaw;
    // Real-time Acceleration (in g)
    float axg, ayg, azg;

    IMU() : pitch(0), roll(0), yaw(0), lastTime(0), axg(0), ayg(0), azg(0) {}

    bool begin() {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x6B);
        Wire.write(0x00); 
        if (Wire.endTransmission() != 0) return false;

        lastTime = micros();
        return true;
    }

    void update() {
        unsigned long now = micros();
        float dt = (now - lastTime) / 1000000.0;
        lastTime = now;
        if (dt <= 0 || dt > 0.5) dt = 0.01;

        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x3B);
        Wire.endTransmission(false);
        Wire.requestFrom(MPU_ADDR, (uint8_t)14);

        if (Wire.available() < 14) return;

        ax = (Wire.read() << 8) | Wire.read();
        ay = (Wire.read() << 8) | Wire.read();
        az = (Wire.read() << 8) | Wire.read();
        Wire.read(); Wire.read(); // Skip temp
        gx = (Wire.read() << 8) | Wire.read();
        gy = (Wire.read() << 8) | Wire.read();
        gz = (Wire.read() << 8) | Wire.read();

        // Convert raw to Gs
        axg = ax / ACCEL_SCALE;
        ayg = ay / ACCEL_SCALE;
        azg = az / ACCEL_SCALE;

        float gx_dps = gx / GYRO_SCALE;
        float gy_dps = gy / GYRO_SCALE;
        float gz_dps = gz / GYRO_SCALE;

        // Angles
        float accelPitch = atan2(ayg, sqrt(axg * axg + azg * azg)) * 180.0 / PI;
        float accelRoll  = atan2(-axg, azg) * 180.0 / PI;

        pitch = ALPHA * (pitch + gx_dps * dt) + (1.0 - ALPHA) * accelPitch;
        roll  = ALPHA * (roll + gy_dps * dt) + (1.0 - ALPHA) * accelRoll;
        yaw   += gz_dps * dt;

        if (yaw > 180)  yaw -= 360;
        if (yaw < -180) yaw += 360;
    }
};

#endif