/**
 * @file AS5600_Driver.h
 * @brief Lightweight I2C driver for the AS5600 12-bit Magnetic Rotary Encoder
 */

#ifndef AS5600_DRIVER_H
#define AS5600_DRIVER_H

#include <Arduino.h>
#include <Wire.h>
#include "pinout.h"

struct AS5600_Status {
  bool magnetDetected;
  bool magnetTooWeak;
  bool magnetTooStrong;
  uint8_t agc;
  uint16_t magnitude;
  String statusText;
};

class AS5600_Encoder {
public:
  // EMA alpha: 1.0 = instant response (no lag).
  static constexpr float EMA_ALPHA = 1.0f;

  AS5600_Encoder()
    : _lastRaw(0), _revolutionCount(0), _zeroOffset(0.0f),
      _initialized(false), _filteredRaw(-1.0f) {}

  /**
   * @brief Initialize I2C bus and verify sensor connection
   * @param sda GPIO pin for SDA (default from pinout.h)
   * @param scl GPIO pin for SCL (default from pinout.h)
   * @param freq I2C clock frequency (default 400kHz)
   * @return true if AS5600 responds on I2C address 0x36, false otherwise
   */
  bool begin(int sda = I2C_SDA_PIN, int scl = I2C_SCL_PIN, uint32_t freq = I2C_FREQUENCY) {
    Wire.begin(sda, scl, freq);
    delay(50); // Sensor power-up settling time

    if (checkConnection()) {
      _initialized = true;
      _lastRaw = readRawAngle();
      return true;
    }
    return false;
  }

  /**
   * @brief Check if device acknowledges at address 0x36
   */
  bool checkConnection() {
    Wire.beginTransmission(AS5600_I2C_ADDR);
    return (Wire.endTransmission() == 0);
  }

  /**
   * @brief Read raw 12-bit angle value directly from registers 0x0C and 0x0D
   * @return 12-bit raw integer [0 .. 4095]
   */
  uint16_t readRawAngle() {
    uint8_t hi = 0, lo = 0;
    Wire.beginTransmission(AS5600_I2C_ADDR);
    Wire.write(AS5600_REG_RAW_HI);
    if (Wire.endTransmission() != 0) return _lastRaw;

    Wire.requestFrom((uint8_t)AS5600_I2C_ADDR, (uint8_t)2);
    if (Wire.available() >= 2) {
      hi = Wire.read();
      lo = Wire.read();
    }
    return ((uint16_t)(hi & 0x0F) << 8) | lo;
  }

  /**
   * @brief Read filtered 12-bit angle value from registers 0x0E and 0x0F
   * @return 12-bit filtered integer [0 .. 4095]
   */
  uint16_t readFilteredAngle() {
    uint8_t hi = 0, lo = 0;
    Wire.beginTransmission(AS5600_I2C_ADDR);
    Wire.write(AS5600_REG_ANG_HI);
    if (Wire.endTransmission() != 0) return _lastRaw;

    Wire.requestFrom((uint8_t)AS5600_I2C_ADDR, (uint8_t)2);
    if (Wire.available() >= 2) {
      hi = Wire.read();
      lo = Wire.read();
    }
    return ((uint16_t)(hi & 0x0F) << 8) | lo;
  }

  /**
   * @brief Perform a single I2C read and update all internal state.
   *        Call this ONCE per control cycle; getDegrees() and getCumulativeDegrees()
   *        will then return consistent values from the same sample.
   */
  void update() {
    uint16_t raw        = readRawAngle();
    uint16_t hwFiltered = readFilteredAngle();
    updateCumulative(raw);   // revolution counting from raw (accurate wrap detection)
    applyEMA(hwFiltered);    // EMA smoothing from chip-filtered value
  }

  /**
   * @brief Get single-turn angle in degrees [0.0 .. 359.91] with zero-offset.
   *        Call update() first each cycle.
   */
  float getDegrees() {
    float deg = _filteredRaw * AS5600_DEG_PER_LSB - _zeroOffset;
    while (deg <    0.0f) deg += 360.0f;
    while (deg >= 360.0f) deg -= 360.0f;
    return deg;
  }

  /**
   * @brief Get multi-turn continuous angle (unbounded, no wrap).
   *        Call update() first each cycle.
   *        Revolution counting uses raw angle; within-revolution uses EMA-filtered value.
   */
  float getCumulativeDegrees() {
    float fr = (_filteredRaw >= 0.0f) ? _filteredRaw : (float)_lastRaw;
    int32_t totalTicks = (int32_t)_revolutionCount * 4096 + (int32_t)fr;
    return ((float)totalTicks * AS5600_DEG_PER_LSB) - _zeroOffset;
  }

  /**
   * @brief Set current mechanical position as 0 degrees
   */
  void setZero() {
    uint16_t raw = readRawAngle();
    _zeroOffset = (float)raw * AS5600_DEG_PER_LSB;
    _revolutionCount = 0;
    _lastRaw    = raw;
    _filteredRaw = (float)raw; // snap filter to current position immediately
  }

  /**
   * @brief Read magnet detection status and diagnostics
   */
  AS5600_Status getStatus() {
    AS5600_Status st;
    st.magnetDetected = false;
    st.magnetTooWeak = false;
    st.magnetTooStrong = false;
    st.agc = 0;
    st.magnitude = 0;
    st.statusText = "Disconnected";

    // Read STATUS register (0x0B)
    Wire.beginTransmission(AS5600_I2C_ADDR);
    Wire.write(AS5600_REG_STATUS);
    if (Wire.endTransmission() == 0) {
      Wire.requestFrom((uint8_t)AS5600_I2C_ADDR, (uint8_t)1);
      if (Wire.available()) {
        uint8_t regVal = Wire.read();
        st.magnetDetected = (regVal & 0x20) != 0; // Bit 5 = MD
        st.magnetTooWeak  = (regVal & 0x10) != 0; // Bit 4 = ML
        st.magnetTooStrong= (regVal & 0x08) != 0; // Bit 3 = MH
      }
    } else {
      return st;
    }

    // Read AGC register (0x1A)
    Wire.beginTransmission(AS5600_I2C_ADDR);
    Wire.write(AS5600_REG_AGC);
    if (Wire.endTransmission() == 0) {
      Wire.requestFrom((uint8_t)AS5600_I2C_ADDR, (uint8_t)1);
      if (Wire.available()) {
        st.agc = Wire.read();
      }
    }

    // Read MAGNITUDE register (0x1B, 0x1C)
    Wire.beginTransmission(AS5600_I2C_ADDR);
    Wire.write(AS5600_REG_MAGN_HI);
    if (Wire.endTransmission() == 0) {
      Wire.requestFrom((uint8_t)AS5600_I2C_ADDR, (uint8_t)2);
      if (Wire.available() >= 2) {
        uint8_t h = Wire.read();
        uint8_t l = Wire.read();
        st.magnitude = ((uint16_t)(h & 0x0F) << 8) | l;
      }
    }

    // Build human-friendly status string
    if (!st.magnetDetected) {
      st.statusText = "No Magnet Detected";
    } else if (st.magnetTooWeak) {
      st.statusText = "Magnet Too Far / Weak";
    } else if (st.magnetTooStrong) {
      st.statusText = "Magnet Too Close / Strong";
    } else {
      st.statusText = "Optimal Magnet Alignment";
    }

    return st;
  }

private:
  uint16_t _lastRaw;
  int32_t  _revolutionCount;
  float    _zeroOffset;
  bool     _initialized;
  float    _filteredRaw; // EMA state in raw ticks; -1.0 = uninitialised

  /**
   * @brief Wrap-safe real-time filter on raw 12-bit ticks.
   *        Provides instant zero-lag response during rotation while suppressing
   *        static single-tick jitter when stationary.
   */
  void applyEMA(uint16_t newRaw) {
    if (_filteredRaw < 0.0f) {
      _filteredRaw = (float)newRaw; // first sample — initialise directly
      return;
    }
    float diff = (float)newRaw - _filteredRaw;
    // Shortest-path across 0/4095 boundary
    if (diff >  2048.0f) diff -= 4096.0f;
    if (diff < -2048.0f) diff += 4096.0f;

    // Zero-lag tracking: if moving (>2 ticks ~ 0.17 deg), track immediately!
    if (fabsf(diff) > 2.0f) {
      _filteredRaw = (float)newRaw;
    } else {
      _filteredRaw += 0.5f * diff; // subtle jitter suppression when resting
    }

    // Keep in [0, 4096)
    if (_filteredRaw >= 4096.0f) _filteredRaw -= 4096.0f;
    if (_filteredRaw <     0.0f) _filteredRaw += 4096.0f;
  }

  void updateCumulative(uint16_t currentRaw) {
    int32_t diff = (int32_t)currentRaw - (int32_t)_lastRaw;
    // Check for rollover at 4096 boundary
    if (diff < -2048) {
      _revolutionCount++; // Clockwise wrap (4095 -> 0)
    } else if (diff > 2048) {
      _revolutionCount--; // Counter-clockwise wrap (0 -> 4095)
    }
    _lastRaw = currentRaw;
  }
};

#endif // AS5600_DRIVER_H
