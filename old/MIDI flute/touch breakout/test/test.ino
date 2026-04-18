#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}

  Wire1.begin();

  Serial.println("Reading ELE_CFG...");
  Wire1.beginTransmission(0x5A);
  Wire1.write(0x5E);
  Wire1.endTransmission(false);
  Wire1.requestFrom(0x5A, (uint8_t)1);

  if (Wire1.available()) {
    uint8_t ele = Wire1.read();
    Serial.print("ELE_CFG = 0x");
    Serial.println(ele, HEX);
  } else {
    Serial.println("No data");
  }
}

void loop() {}
