#pragma once

/**
 * @brief Basic PID Controller with anti-windup and derivative filter.
 *
 * Output is a velocity command (degrees/sec) used to drive the stepper motor.
 * Derivative is computed on the measurement (not the error) to avoid
 * derivative kick on setpoint changes.
 */
class PID {
public:
    float kp;      // Proportional gain
    float ki;      // Integral gain
    float kd;      // Derivative gain
    float dt;         // Sample time (seconds)
    float max_u;      // Output saturation limit (deg/s)
    float filterN;    // Derivative filter coefficient (e.g. 5–20) — named filterN to avoid ESP32 macro conflict

private:
    float _integral;
    float _prevMeasurement;  // For derivative-on-measurement
    float _derivFiltered;    // Low-pass filtered derivative

public:
    // All defaults on one line — Arduino preprocessor can't handle multi-line default args
    PID(float _kp = 5.0f, float _ki = 0.5f, float _kd = 0.05f, float _dt = 0.01f, float _max_u = 2000.0f, float _filterN = 10.0f)
        : kp(_kp), ki(_ki), kd(_kd), dt(_dt), max_u(_max_u), filterN(_filterN),
          _integral(0), _prevMeasurement(0), _derivFiltered(0) {}

    /**
     * @brief Compute PID output.
     * @param setpoint Desired angle (degrees)
     * @param measurement Current measured angle (degrees)
     * @return Control output (velocity command, degrees/sec)
     */
    float update(float setpoint, float measurement) {
        float error = setpoint - measurement;

        // ── Proportional ────────────────────────────────────────────
        float p_term = kp * error;

        // ── Integral with anti-windup (clamping) ────────────────────
        _integral += error * dt;
        float i_term = ki * _integral;

        // ── Derivative on measurement (avoids kick on setpoint step) 
        float d_raw = -(measurement - _prevMeasurement) / dt;
        // First-order low-pass filter: alpha = filterN*dt / (1 + filterN*dt)
        float alpha = filterN * dt / (1.0f + filterN * dt);
        _derivFiltered = alpha * d_raw + (1.0f - alpha) * _derivFiltered;
        float d_term = kd * _derivFiltered;
        _prevMeasurement = measurement;

        // ── Raw output ──────────────────────────────────────────────
        float output = p_term + i_term + d_term;

        // ── Anti-windup: back-calculate if saturated ─────────────────
        if (output > max_u) {
            _integral -= (output - max_u) / (ki > 0 ? ki : 1.0f);
            output = max_u;
        } else if (output < -max_u) {
            _integral -= (output + max_u) / (ki > 0 ? ki : 1.0f);
            output = -max_u;
        }

        return output;
    }

    void setParams(float _kp, float _ki, float _kd) {
        kp = _kp; ki = _ki; kd = _kd;
    }

    void reset(float current_measurement) {
        _integral       = 0;
        _prevMeasurement = current_measurement;
        _derivFiltered  = 0;
    }
};
