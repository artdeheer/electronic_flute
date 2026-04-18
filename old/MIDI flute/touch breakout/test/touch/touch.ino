#include <Wire.h>
#include <Adafruit_MPR121.h>

Adafruit_MPR121 cap;

const uint8_t NUM_ELECTRODES = 12;

// --------- SETTINGS ----------
const int PRESS_THRESHOLD   = 30;
const int RELEASE_THRESHOLD = 15;

const uint8_t BASELINE_ALPHA = 32;

const bool TOUCH_MAKES_FILT_GO_DOWN = true;

// --------- MODES ----------
const bool DEBUG_MODE = true;     // TRUE = print detailed electrode values continuously
const bool PLOT_DELTA = true;     // if DEBUG_MODE false but plotting enabled, choose delta or raw
const uint16_t OUTPUT_INTERVAL_MS = 100; // debug print interval
// ----------------------------------------

uint16_t baseVals[NUM_ELECTRODES];
bool isPressed[NUM_ELECTRODES];

static void mpr121SoftReset() {
  Wire1.beginTransmission(0x5A);
  Wire1.write(0x80);
  Wire1.write(0x63);
  Wire1.endTransmission();
  delay(10);
}

static inline int16_t computeDelta(uint16_t base, uint16_t filt) {
  if (TOUCH_MAKES_FILT_GO_DOWN)
    return (int16_t)base - (int16_t)filt;
  else
    return (int16_t)filt - (int16_t)base;
}

void calibrateBaselines() {
  Serial.println("Calibrating baselines (DON'T touch pads)...");
  const uint8_t throwaway = 10;
  const uint8_t samples = 80;

  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {
    for (uint8_t t = 0; t < throwaway; t++) {
      (void)cap.filteredData(i);
      delay(5);
    }

    uint32_t sum = 0;
    for (uint8_t s = 0; s < samples; s++) {
      sum += cap.filteredData(i);
      delay(5);
    }

    baseVals[i] = sum / samples;
  }

  Serial.println("Baseline calibration complete.\n");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire1.begin();
  Wire1.setClock(100000);
  delay(50);

  Serial.println("MPR121 init on Wire1 @ 0x5A");

  mpr121SoftReset();

  if (!cap.begin(0x5A, &Wire1)) {
    Serial.println("ERROR: MPR121 begin() failed");
    while (1) delay(10);
  }

  cap.setThresholds(12, 6);

  for (uint8_t i = 0; i < NUM_ELECTRODES; i++)
    isPressed[i] = false;

  delay(2000);
  calibrateBaselines();

  Serial.print("Delta sign: touch makes filt go ");
  Serial.println(TOUCH_MAKES_FILT_GO_DOWN ? "DOWN" : "UP");

  Serial.print("DEBUG_MODE: ");
  Serial.println(DEBUG_MODE ? "ON" : "OFF");
  Serial.println();
}

void loop() {

  static uint32_t lastOutput = 0;

  uint32_t now = millis();
  if (now - lastOutput < OUTPUT_INTERVAL_MS) return;
  lastOutput = now;

  for (uint8_t i = 0; i < NUM_ELECTRODES; i++) {

    uint16_t filt = cap.filteredData(i);
    int16_t delta = computeDelta(baseVals[i], filt);

    // Press logic
    if (!isPressed[i] && delta > PRESS_THRESHOLD) {
      isPressed[i] = true;
    } 
    else if (isPressed[i] && delta < RELEASE_THRESHOLD) {
      isPressed[i] = false;
    }

    // Baseline drift correction
    if (!isPressed[i]) {
      baseVals[i] = (uint16_t)(
        ((uint32_t)baseVals[i] * (BASELINE_ALPHA - 1) + filt) / BASELINE_ALPHA
      );
    }

    // -------- DEBUG OUTPUT --------
    if (DEBUG_MODE) {
      Serial.print("E"); Serial.print(i);
      Serial.print(" | Filt: "); Serial.print(filt);
      Serial.print(" | Base: "); Serial.print(baseVals[i]);
      Serial.print(" | Delta: "); Serial.print(delta);
      Serial.print(" | Pressed: ");
      Serial.print(isPressed[i] ? "YES" : "NO");
      Serial.print("   ");
    }
  }

  if (DEBUG_MODE) {
    Serial.println();
  }
}
