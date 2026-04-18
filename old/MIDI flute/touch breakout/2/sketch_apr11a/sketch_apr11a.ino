#include <Wire.h>
#include <Adafruit_CAP1188.h>

#define CAP1188_RESET  4
Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_RESET);

void setup() {
  Serial.begin(9600);
  while (!Serial); 
  
  Wire1.begin();

  if (!cap.begin(0x29, &Wire1)) {
    Serial.println("CAP1188 not found!");
    while (1);
  }

  // --- ADJUST SENSITIVITY HERE ---
  
  // 1. Global Sensitivity (Optional)
  // 0x00 is most sensitive, 0x0F is least sensitive. Default is 0x02.
  // cap.writeRegister(0x1F, 0x04); 

  // 2. Adjust C1 Threshold specifically
  // Register 0x30 is the threshold for C1.
  // Default is 0x40 (64). 
  // Increase this number (e.g., 0x60 or 0x7F) to make it LESS sensitive.
  // Decrease it (e.g., 0x20) to make it MORE sensitive.
  cap.writeRegister(0x30, 0x20); 

  Serial.println("Sensitivity adjusted for C1!");
}

void loop() {
  uint8_t touched = cap.touched();

  if (touched & (1 << 0)) { // Check specifically for C1 (bit 0)
    Serial.println("C1 Touched!");
  }

  delay(50);
}