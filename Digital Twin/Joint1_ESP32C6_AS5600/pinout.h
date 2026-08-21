/**
 * @file pinout.h
 * @brief Pinout configuration for ESP32-C6 Super Mini and AS5600 Magnetic Encoder
 * @details This file defines all hardware pin connections between the ESP32-C6 Super Mini
 *          and the AS5600 12-bit magnetic rotary encoder mounted on Joint 1 (NEMA 17 Stepper).
 */

#ifndef PINOUT_H
#define PINOUT_H

#include <Arduino.h>

// ==============================================================================
// 1. I2C COMMUNICATION PIN DEFINITIONS (ESP32-C6 Super Mini)
// ==============================================================================
// ESP32-C6 supports flexible GPIO multiplexing for I2C.
// The default and recommended pins for the ESP32-C6 Super Mini board:

#define I2C_SDA_PIN         6      // Connect to AS5600 'SDA' pin
#define I2C_SCL_PIN         7      // Connect to AS5600 'SCL' pin
#define I2C_FREQUENCY       400000 // 400 kHz Fast-mode I2C clock

// ==============================================================================
// 2. AS5600 ENCODER I2C REGISTERS & HARDWARE CONSTANTS
// ==============================================================================
#define AS5600_I2C_ADDR     0x36   // Fixed 7-bit I2C address for AS5600

// AS5600 Register Addresses
#define AS5600_REG_STATUS   0x0B   // Magnet status register
#define AS5600_REG_RAW_HI   0x0C   // Raw angle high byte [11:8]
#define AS5600_REG_RAW_LO   0x0D   // Raw angle low byte  [7:0]
#define AS5600_REG_ANG_HI   0x0E   // Filtered angle high byte [11:8]
#define AS5600_REG_ANG_LO   0x0F   // Filtered angle low byte  [7:0]
#define AS5600_REG_AGC      0x1A   // Automatic Gain Control value (0-255)
#define AS5600_REG_MAGN_HI  0x1B   // Magnitude high byte
#define AS5600_REG_MAGN_LO  0x1C   // Magnitude low byte

// Resolution and scaling constants
#define AS5600_RESOLUTION   4096.0f // 12-bit ADC resolution (0 to 4095)
#define AS5600_DEG_PER_LSB  (360.0f / AS5600_RESOLUTION) // ~0.08789 degrees per tick

// ==============================================================================
// 3. TMC2209 STEPPER DRIVER PIN DEFINITIONS (ESP32-C6 Super Mini)
// ==============================================================================
#define TMC_STEP_PIN        4      // Connect to TMC2209 'STEP' pin
#define TMC_DIR_PIN         2      // Connect to TMC2209 'DIR' pin
#define TMC_EN_PIN          3      // Connect to TMC2209 'EN' pin (Active Low)

// ==============================================================================
// 4. ONBOARD LED PIN (OPTIONAL STATUS INDICATOR)
// ==============================================================================
// On ESP32-C6 Super Mini boards, onboard status LED is typically GPIO 8 or GPIO 15
#define ONBOARD_LED_PIN     8      // Set to -1 if not used or active high/low depends on board

// ==============================================================================
// 5. WIRING REFERENCE TABLE
// ==============================================================================
/*
 * +-------------------+-------------------------+-----------------------------------------+
 * | Module Pin        | ESP32-C6 Super Mini Pin | Notes / Description                     |
 * +-------------------+-------------------------+-----------------------------------------+
 * | AS5600 VCC (3V3)  | 3V3                     | 3.3V Power supply (DO NOT USE 5V)       |
 * | AS5600 GND        | GND                     | Ground reference                        |
 * | AS5600 SDA        | GPIO 6                  | I2C Serial Data line                    |
 * | AS5600 SCL        | GPIO 7                  | I2C Serial Clock line                   |
 * | AS5600 DIR        | GND                     | Direction: GND = CW increasing angle    |
 * | TMC2209 VIO       | 3V3                     | Logic Power 3.3V                        |
 * | TMC2209 GND       | GND                     | Logic Ground                            |
 * | TMC2209 STEP      | GPIO 4                  | Step pulse input                        |
 * | TMC2209 DIR       | GPIO 2                  | Direction control                       |
 * | TMC2209 EN        | GPIO 3                  | Enable driver (Active LOW)              |
 * +-------------------+-------------------------+-----------------------------------------+
 */

#endif // PINOUT_H
