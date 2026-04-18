#include <Arduino.h>          // Required for PIO C++ projects
#include <Wire.h>
#include <Adafruit_CAP1188.h>

// Function Prototypes (Optional for this simple script, but good practice in C++)
void setup();
void loop();

#define CAP1188_RESET  4
Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_RESET);

void setup() {
    Serial.begin(9600);
    
    // Wait for Serial to be ready (important for boards like Teensy or ESP32-S3)
    while (!Serial) {
        delay(10); 
    }
  
    // Initialize the secondary I2C bus
    Wire1.begin();

    // Initialize CAP1188 with address 0x29 on Wire1
    if (!cap.begin(0x29, &Wire1)) {
        Serial.println("CAP1188 not found!");
        while (1) {
            delay(100); // Prevents watchdog timer resets on some chips
        }
    }

    // --- ADJUST SENSITIVITY HERE ---
    // Register 0x30 is the threshold for C1.
    // Lower values (0x20) = More sensitive.
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