# TMC2209 Stepper Motor Driver - Technical Documentation

The **TMC2209** is an ultra-silent, two-phase bipolar stepper motor driver IC manufactured by Trinamic (Analog Devices). It delivers smooth, quiet, and precise motion for 3D printers, CNC systems, and robotic joints (e.g., NEMA 17 stepper motors).

---

## 1. Key Specifications

| Parameter | Specification |
| :--- | :--- |
| **Motor Supply Voltage ($V_{MOT}$)** | **4.75V to 28V DC** (12V or 24V typical) |
| **Logic Supply Voltage ($V_{IO}$)** | **3.3V to 5.0V DC** (Directly compatible with ESP32-C6 3.3V logic) |
| **Continuous Current Rating** | Up to **2.0A RMS** (2.8A Peak) with proper heatsink and active airflow |
| **Default Sense Resistors ($R_{SENSE}$)** | **0.11 $\Omega$** (Marked `R110` on standard BTT / MKS modules) |
| **Native Microstepping** | $1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64$ up to $1/256$ microstepping |
| **Interpolation** | **MicroPlyer™** interpolates 1/16 input steps smoothly to 1/256 steps |
| **Control Interfaces** | • **STEP / DIR** (Legacy pulse/direction control)<br>• **Single-Wire UART** (Digital software configuration) |

---

## 2. Proprietary Trinamic Technologies

- **StealthChop2™**: Voltage-regulated chopper mode for whisper-quiet motor operation at low to medium velocities (eliminates motor whistling/humming).
- **SpreadCycle™**: Current-controlled chopper mode for high dynamic performance, higher torque, and resonance suppression at high speeds.
- **StallGuard4™**: Sensorless load measurement and stall detection; triggers the `DIAG` pin on mechanical blockages (enables sensorless homing).
- **CoolStep™**: Automatically reduces motor current by up to 75% during low-load intervals, minimizing motor heating.

---

## 3. Standard StepStick Pinout

```text
                           +-------------------+
             (Enable)   EN | [ ]           [ ] | VMOT  (Motor Power 12-24V)
             (Mode 1)  MS1 | [ ]           [ ] | GND   (Power Ground)
             (Mode 2)  MS2 | [ ]           [ ] | 2B    (Motor Coil B2)
         (UART / PDN) UART | [ ]           [ ] | 2A    (Motor Coil B1)
             (CLK In)  CLK | [ ]           [ ] | 1A    (Motor Coil A1)
             (Step In) STP | [ ]           [ ] | 1B    (Motor Coil A2)
        (Direction In) DIR | [ ]           [ ] | VIO   (Logic Power 3.3V)
             (Diag/SG) DCO | [ ]           [ ] | GND   (Logic Ground)
                           +-------------------+
```

### Pin Description

| Pin | Type | Function |
| :--- | :--- | :--- |
| **VMOT** | Power | Motor DC Power Supply (4.75V – 28V). **Must have $\ge 47\mu\text{F}$ capacitor to GND**. |
| **GND** | Power | Power & Logic Ground (Common Ground). |
| **VIO / VDD** | Power | Logic Power (Connect to **3.3V** rail of ESP32-C6). |
| **1A / 1B** | Output | Stepper Motor Phase A coil connections. |
| **2A / 2B** | Output | Stepper Motor Phase B coil connections. |
| **STEP** | Input | Step pulse input (Each pulse advances the motor by one microstep). |
| **DIR** | Input | Direction control input (HIGH = Clockwise, LOW = Counter-Clockwise). |
| **EN (ENABLE)**| Input | Enable driver stage (Active LOW: `LOW` = Driver ON, `HIGH` = Driver OFF). |
| **PDN / UART** | I/O | Single-Wire UART communication for software configuration. |
| **MS1 / MS2** | Input | Hardware microstep selection pins (Used in Standalone mode). |
| **DIAG** | Output | StallGuard output pulse / sensorless homing trigger. |

---

## 4. Hardware Microstepping Configuration (Standalone Mode)

When UART is not connected, microstep resolution is selected using MS1 and MS2 pins:

| MS1 | MS2 | Microstep Setting | Interpolation |
| :---: | :---: | :---: | :---: |
| **GND** | **GND** | **1/8** | Interpolated to 1/256 with MicroPlyer™ |
| **VIO (3.3V)** | **GND** | **1/2** | None |
| **GND** | **VIO (3.3V)** | **1/4** | None |
| **VIO (3.3V)** | **VIO (3.3V)** | **1/16** | Interpolated to 1/256 with MicroPlyer™ *(Standard)* |

---

## 5. Current Limit ($V_{REF}$) Tuning & Calculation

Setting the correct reference voltage ($V_{REF}$) on the onboard trimmer potentiometer ensures the stepper receives optimal torque without overheating.

### Mathematical Formula (for $R_{SENSE} = 0.11\ \Omega$)

$$I_{RMS} = \frac{V_{REF}}{1.414 \times 8 \times (R_{SENSE} + 0.02)} \approx \frac{V_{REF}}{1.47}$$

$$V_{REF} = I_{RMS} \times 1.47$$

### Quick Reference Table for NEMA 17 Steppers ($R_{SENSE} = 0.11\ \Omega$)

| Motor Rated Current ($I_{MAX}$) | Recommended $I_{RMS}$ (~70-80% rated) | Target $V_{REF}$ Setting |
| :---: | :---: | :---: |
| **1.0 A** | 0.70 A | **~1.03 V** |
| **1.2 A** | 0.85 A | **~1.25 V** |
| **1.5 A** | 1.05 A | **~1.54 V** |
| **1.7 A** | 1.20 A | **~1.76 V** |

### How to Measure & Adjust $V_{REF}$:
1. Connect $V_{IO}$ (3.3V) and GND to power the driver logic. (Motor supply $V_{MOT}$ is optional for $V_{REF}$ tuning on most modern modules).
2. Set your digital multimeter to **DC Voltage** mode.
3. Put the **Black probe** on a Ground pin (`GND`).
4. Put the **Red probe** on the metal top of the small potentiometer screw or the dedicated $V_{REF}$ test pad.
5. Use a ceramic or insulated screwdriver to gently turn the potentiometer until the multimeter reads your target $V_{REF}$.
   - **Clockwise**: Increases $V_{REF}$ / Current.
   - **Counter-Clockwise**: Decreases $V_{REF}$ / Current.

---

## 6. Wiring with ESP32-C6 & NEMA 17

```text
       +-----------------------+              +------------------------+
       |   ESP32-C6 Super Mini |              |    TMC2209 Driver      |
       |                       |              |                        |
       |  [ 3V3 ] -------------+------------->| [ VIO ] (Logic 3.3V)   |
       |  [ GND ] -------------+------------->| [ GND ] (Logic Ground) |
       |  [ GPIO 4 ] (Step) ---+------------->| [ STEP ]               |
       |  [ GPIO 5 ] (Dir) ----+------------->| [ DIR ]                |
       |  [ GPIO 3 ] (Enable) -+------------->| [ EN ] (Active LOW)    |
       +-----------------------+              +------------------------+
                                              | [ VMOT ] <--- +12V/24V Power
                                              | [ GND ]  <--- 12V/24V Ground
                                              |   || (100uF Cap across VMOT/GND)
                                              | [ 1A, 1B ] -> Motor Phase A
                                              | [ 2A, 2B ] -> Motor Phase B
                                              +------------------------+
```

---

## 7. Critical Safety & Operating Rules

> [!CAUTION]
> **NEVER connect or disconnect a stepper motor while the driver is powered ($V_{MOT}$ active)**. The resulting inductive kickback voltage spike will instantly destroy the driver output MOSFETs.

> [!IMPORTANT]
> **Electrolytic Buffer Capacitor**: Always place a **$47\mu\text{F} \text{ to } 100\mu\text{F}$ low-ESR electrolytic capacitor** directly between $V_{MOT}$ and $GND$ close to the driver module to suppress power supply transients.

> [!TIP]
> **Thermal Management**: Install the included aluminum heatsink on the top of the TMC2209 IC without touching the exposed pin headers. For currents exceeding $1.0\text{A RMS}$, active fan airflow is strongly recommended.
