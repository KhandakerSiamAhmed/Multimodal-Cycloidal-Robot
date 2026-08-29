# Hardware Components Documentation

This directory contains technical documentation, pinouts, wiring diagrams, configuration guides, and operating parameters for the primary hardware components used in the **Digital Twin Robotic Joint** project.

---

## Component Index

| Component | Description | Documentation File |
| :--- | :--- | :--- |
| **ESP32-C6 Super Mini** | RISC-V 160MHz MCU, Wi-Fi 6, BLE 5, Thread/Zigbee, Native USB-C | [esp32c6_super_mini.md](esp32c6_super_mini.md) |
| **AS5600 Magnetic Encoder** | 12-bit Contactless Rotary Position Sensor (I2C, 4096 counts/rev, 0.087° resolution) | [as5600_encoder.md](as5600_encoder.md) |
| **TMC2209 Stepper Driver** | Ultra-silent Stepper Motor Driver (StealthChop2, SpreadCycle, StallGuard4, MicroPlyer) | [tmc2209_stepper_driver.md](tmc2209_stepper_driver.md) |

---

## System Interconnection Overview

```text
               +-------------------------------------------+
               |            ESP32-C6 Super Mini            |
               |                                           |
               |  (I2C Bus: GPIO 6 / 7)    (Motor Control) |
               +-------------+---------------------+-------+
                             |                     |
                  [I2C SDA / SCL]            [STEP / DIR / EN]
                             |                     |
                             v                     v
               +--------------------+   +---------------------+
               |   AS5600 Encoder   |   |   TMC2209 Driver    |
               | (Joint 1 Feedback) |   | (NEMA 17 Actuator)  |
               +--------------------+   +----------+----------+
                         ^                         |
                 (Magnetic field)           (4-Wire 2-Phase)
                         |                         |
               +---------+-------------------------+---------+
               |            NEMA 17 Stepper Motor            |
               |       (Shaft Magnet + Actuated Joint)       |
               +---------------------------------------------+
```
