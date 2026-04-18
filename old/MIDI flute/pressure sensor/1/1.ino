// ------- MPXV7002DP pressure sensor --------
const int PRESSURE_PIN = A0;

// Resistor divider values (Ohms)
const float R1_TOP = 10000.0;
const float R2_BOTTOM = 20000.0;

// ADC settings
const float ADC_REF_VOLT = 3.3;
const float ADC_COUNTS   = 1023.0;   // 10-bit

const float MPXV_VSUPPLY = 5.03;

// ===== Smoothing settings =====
const int NUM_SAMPLES = 16;
int samples[NUM_SAMPLES];
int sampleIndex = 0;
long sampleSum = 0;

void setup() {
  Serial.begin(115200);
  delay(500);

  analogReadResolution(10);
  analogReadAveraging(16);   // <-- hardware averaging (very effective)

  Serial.println("kPa mmH2O");
}

void loop() {

  // ---- Read ADC ----
  int raw = analogRead(PRESSURE_PIN);

  // ---- Moving average filter ----
  sampleSum -= samples[sampleIndex];
  samples[sampleIndex] = raw;
  sampleSum += raw;

  sampleIndex++;
  if (sampleIndex >= NUM_SAMPLES) sampleIndex = 0;

  float rawFiltered = (float)sampleSum / NUM_SAMPLES;

  // ---- Convert to voltage ----
  float vTeensy = (rawFiltered * ADC_REF_VOLT) / ADC_COUNTS;

  // ---- Undo divider ----
  float dividerFactor = (R1_TOP + R2_BOTTOM) / R2_BOTTOM;
  float vSensor = vTeensy * dividerFactor;

  // ---- Convert to pressure ----
  float P_kPa = ((vSensor / MPXV_VSUPPLY) - 0.5) / 0.2;
  float P_mmH2O = (P_kPa * 1000.0) / 9.80665;

  Serial.print(P_kPa, 3);
  Serial.print(" ");
  Serial.println(P_mmH2O, 1);

  delay(10);   // optional: limit to ~100 Hz
}
