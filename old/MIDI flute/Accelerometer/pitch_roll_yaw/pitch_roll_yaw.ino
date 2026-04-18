#include <Wire.h>

const uint8_t MPU_ADDR = 0x68;

// Raw values
int16_t ax, ay, az;
int16_t gx, gy, gz;

// Filtered orientation
float pitch = 0.0;
float roll  = 0.0;
float yaw   = 0.0;

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);

  Wire.begin(); // SDA=18, SCL=19 on Teensy 4.0

  // Wake MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Serial.println("MPU6050 Running... Showing Pitch / Roll / Yaw only.");
}

void loop() {
  // --- TIME DELTA ---
  unsigned long now = micros();
  float dt = (now - lastTime) / 1e6;
  lastTime = now;
  if (dt <= 0 || dt > 1) dt = 0.01;

  // --- READ SENSOR DATA ---
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();
  Wire.read(); Wire.read(); // temperature not used
  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();

  // Convert to g and °/s
  float axg = ax / 16384.0;
  float ayg = ay / 16384.0;
  float azg = az / 16384.0;

  float gx_dps = gx / 131.0;
  float gy_dps = gy / 131.0;
  float gz_dps = gz / 131.0;

  // --- ACCELEROMETER ANGLES ---
  float accelPitch = atan2(ayg, sqrt(axg * axg + azg * azg)) * 180 / PI;
  float accelRoll  = atan2(-axg, azg) * 180 / PI;

  // --- COMPLEMENTARY FILTER ---
  const float alpha = 0.98; // gyro weight

  pitch = alpha * (pitch + gx_dps * dt) + (1 - alpha) * accelPitch;
  roll  = alpha * (roll  + gy_dps * dt) + (1 - alpha) * accelRoll;
  yaw   += gz_dps * dt;  // No accel reference → gyro only

  // Keep yaw in -180..180
  if (yaw > 180) yaw -= 360;
  if (yaw < -180) yaw += 360;

  // --- PRINT ONLY PITCH / ROLL / YAW ---
  Serial.print("Pitch: ");
  Serial.print(pitch, 2);
  Serial.print(" | Roll: ");
  Serial.print(roll, 2);
  Serial.print(" | Yaw: ");
  Serial.println(yaw, 2);

  delay(20);
}
