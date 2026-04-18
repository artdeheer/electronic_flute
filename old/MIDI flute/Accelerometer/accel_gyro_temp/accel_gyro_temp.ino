#include <Wire.h>

const uint8_t MPU_ADDR = 0x68;  // I2C address of MPU6050 when AD0 = GND

// Raw sensor values
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t tempRaw;

// Conversion factors for default config (+/-2g, +/-250 deg/s)
const float ACCEL_SCALE = 16384.0;  // LSB/g
const float GYRO_SCALE  = 131.0;    // LSB/(deg/s)

void setup() {
  Serial.begin(115200);
  // Give some time for Serial Monitor to open (optional)
  while (!Serial && millis() < 4000) {
    // wait
  }

  Wire.begin();  // Uses SDA=18, SCL=19 by default on Teensy 4.0

  // --- Check MPU6050 is responding ---
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75);  // WHO_AM_I register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)1);
  uint8_t whoAmI = Wire.read();

  Serial.print("MPU6050 WHO_AM_I: 0x");
  Serial.println(whoAmI, HEX);

  if (whoAmI != 0x68) {
    Serial.println("MPU6050 not detected! Check wiring/address.");
  }

  // --- Wake up the MPU6050 (it starts in sleep mode) ---
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0x00);  // set to 0 to wake up
  Wire.endTransmission(true);

  // (Optional) Configure accelerometer and gyro full-scale ranges
  // Here we leave the default settings:
  //   Accel: ±2g  (ACCEL_CONFIG = 0x00)
  //   Gyro : ±250 deg/s (GYRO_CONFIG = 0x00)

  Serial.println("MPU6050 initialized.");
}

void loop() {
  // Start reading at ACCEL_XOUT_H (0x3B), 14 bytes total:
  // ax, ay, az, temp, gx, gy, gz
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)14, (uint8_t)true);

  ax      = (Wire.read() << 8) | Wire.read();
  ay      = (Wire.read() << 8) | Wire.read();
  az      = (Wire.read() << 8) | Wire.read();
  tempRaw = (Wire.read() << 8) | Wire.read();
  gx      = (Wire.read() << 8) | Wire.read();
  gy      = (Wire.read() << 8) | Wire.read();
  gz      = (Wire.read() << 8) | Wire.read();

  // Convert raw values
  float ax_g = ax / ACCEL_SCALE;
  float ay_g = ay / ACCEL_SCALE;
  float az_g = az / ACCEL_SCALE;

  float gx_dps = gx / GYRO_SCALE;
  float gy_dps = gy / GYRO_SCALE;
  float gz_dps = gz / GYRO_SCALE;

  // Temperature in °C (according to datasheet)
  float tempC = (tempRaw / 340.0) + 36.53;

  // Print to Serial Monitor
  Serial.print("Accel (g): ");
  Serial.print(ax_g, 3); Serial.print(", ");
  Serial.print(ay_g, 3); Serial.print(", ");
  Serial.print(az_g, 3);

  Serial.print(" | Gyro (deg/s): ");
  Serial.print(gx_dps, 3); Serial.print(", ");
  Serial.print(gy_dps, 3); Serial.print(", ");
  Serial.print(gz_dps, 3);

  Serial.print(" | Temp: ");
  Serial.print(tempC, 2);
  Serial.println(" C");

  delay(100);  // 10 Hz update
}
