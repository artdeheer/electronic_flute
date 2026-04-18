

## Differential Air Pressure Sensor Wiring (MPXV7002DP + Teensy)

### Overview
This circuit reads a 0–5 V differential pressure sensor (MPXV7002DP) using a 3.3 V Teensy ADC.
A resistor divider and op-amp buffer are used to safely scale and stabilize the signal.

### Power
- **5 V** → MPXV7002DP **Vcc**
- **5 V** → Op-amp **V+** (LM324 / LM358 / CA3140)
- **GND** → Sensor GND, op-amp GND, Teensy GND (all common)

### Voltage Scaling (Divider)
- **R1 = 10 kΩ**: Sensor **Vout → Vscaled**
- **R2 = 20 kΩ**: **Vscaled → GND**

Scales 0–5 V to approximately 0–3.3 V.

### Signal Filtering
- **C1 = 33 nF** between **Vscaled** and **GND**  
  (Optional, reduces noise)

### Op-Amp Buffer (Voltage Follower)
- **+IN** → Vscaled  
- **−IN** → Op-amp OUT (direct feedback)  
- **OUT** → Teensy **A0**  
- **C2 = 33 nF** between op-amp **V+** and **GND** (mandatory decoupling)

The op-amp buffers the scaled signal and prevents ADC loading.

### Teensy Connection
- **A0** → Op-amp OUT
- Optional: **220 Ω – 1 kΩ** series resistor between OUT and A0
### Result
- Safe input for Teensy ADC
- Stable, low-noise pressure readings
- Sensor remains powered at 5 V

---

## Joystick (2-Axis + Button)

Standard analog joystick module with pins **VCC**, **GND**, **VRx**, **VRy**, **SW**.

### Connections (Teensy 4.0)

| Joystick Pin | Teensy 4.0 Pin | Description |
|-------------|---------------|-------------|
| VCC | 3.3V | Power supply |
| GND | GND | Ground |
| VRx | A0 | X-axis analog output |
| VRy | A1 | Y-axis analog output |
| SW | D2 | Push button signal |


---

## Accelerometer / Gyroscope (GY-521 / MPU6050)

The GY-521 communicates using the I²C interface.

### Connections (Teensy 4.0)

| GY-521 Pin | Teensy 4.0 Pin | Description |
|-----------|---------------|-------------|
| VCC | 3.3V | Power supply |
| GND | GND | Ground |
| SDA | Pin 18 | I²C data |
| SCL | Pin 19 | I²C clock |


---

## MPR121 Capacitive Touch Sensor Wiring (Teensy)

### Power
- **MPR121 3.3V** → Teensy **3.3V**
- **MPR121 GND** → Teensy **GND**

### I²C (Wire1)
- **MPR121 SCL** → Teensy **Pin 16 (SCL1)**
- **MPR121 SDA** → Teensy **Pin 17 (SDA1)**

### Address & Interrupt
- **ADDR**: not connected (internally pulled, address = 0x5A)
- **IRQ**: not connected (polling mode)

### Electrodes
- **E0–E11** → wires or aluminum foil touch pads
- Pads must be on insulating material
- Do not connect electrodes to ground or power


![[wiring.png]]
