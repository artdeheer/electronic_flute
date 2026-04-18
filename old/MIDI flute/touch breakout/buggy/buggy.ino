#include <Wire.h>
#include <Adafruit_MPR121.h>

// MPR121 on Wire1 at 0x5A
Adafruit_MPR121 cap = Adafruit_MPR121();

const uint8_t NUM_ELECTRODES = 12;

// Our own baselines + state
uint16_t baseVals[NUM_ELECTRODES];
bool     isPressed[NUM_ELECTRODES];

// Tune these if needed
const int PRESS_THRESHOLD   = 6;  // how far below baseline counts as "pressed"
const int RELEASE_THRESHOLD = 3;  // hysteresis so it doesn't flicker

void calibrateBaselines() {
  Serial.println("Calibrating baselines, don't touch any pads...");

  // Simple average of several samples
  const uint8_t samples = 40;

  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {
    uint32_t sum = 0;
    for (uint8_t s = 0; s < samples; s++) {
      sum += cap.filteredData(i);
      delay(5);
    }
    baseVals[i] = sum / samples;
  }

  Serial.println("Baselines:");
  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {
    Serial.print("E");
    Serial.print(i);
    Serial.print(" base=");
    Serial.println(baseVals[i]);
  }
  Serial.println("Done. You can touch the pads now.\n");
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {}

  Serial.println("MPR121 custom touch detection on Wire1 @ 0x5A");

  if (!cap.begin(0x5A, &Wire1)) {
    Serial.println("ERROR: MPR121 not found on Wire1 @ 0x5A");
    while (1) { delay(10); }
  }

  // We don't really care about the chip's own thresholds now,
  // but we can still set them to something small.
  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {
    cap.setThresholds(4, 2);
    isPressed[i] = false;
  }

  calibrateBaselines();
}

void loop() {
  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {
    uint16_t filt = cap.filteredData(i);
    int16_t delta = (int16_t)baseVals[i] - (int16_t)filt;
    // On your board: touch -> filt goes LOWER -> delta becomes POSITIVE

    // Detect new press
    if (!isPressed[i] && delta > PRESS_THRESHOLD) {
      isPressed[i] = true;
      Serial.print("Electrode ");
      Serial.print(i);
      Serial.println(" PRESSED");
    }

    // Detect release
    if (isPressed[i] && delta < RELEASE_THRESHOLD) {
      isPressed[i] = false;
      Serial.print("Electrode ");
      Serial.print(i);
      Serial.println(" released");
    }
  }

  delay(20);
}
