#pragma once

/**
 * @brief First-Order Linear Active Disturbance Rejection Control (ADRC)
 * 
 * Suitable for a plant modeled approximately as a first-order system,
 * such as a stepper motor where the input is velocity and output is angle (pure integrator).
 */
class ADRC {
public:
    float b0; // Plant gain (critical gain)
    float wc; // Controller bandwidth
    float wo; // Observer bandwidth
    float dt; // Sample time in seconds
    float max_u; // Maximum control output

    // Observer states
    float z1; // Tracks output (angle)
    float z2; // Tracks total disturbance
    
    float u; // Control effort (velocity)

    ADRC(float _b0 = 1.0f, float _wc = 10.0f, float _wo = 30.0f, float _dt = 0.01f, float _max_u = 1000.0f)
        : b0(_b0), wc(_wc), wo(_wo), dt(_dt), max_u(_max_u), z1(0), z2(0), u(0) {}

    /**
     * @brief Update the ADRC loop
     * @param setpoint Desired target angle
     * @param y Current measured angle
     * @return Control effort (velocity command)
     */
    float update(float setpoint, float y) {
        // 1. Linear Extended State Observer (LESO)
        // Tuning parameters based on bandwidth parameterization
        float beta1 = 2.0f * wo;
        float beta2 = wo * wo;
        
        float e_obs = z1 - y;
        
        // Update observer states (Euler integration)
        z1 += dt * (z2 + u * b0 - beta1 * e_obs);
        z2 += dt * (-beta2 * e_obs);
        
        // 2. Control Law
        float kp = wc;
        float u0 = kp * (setpoint - z1);
        
        // 3. Disturbance Rejection
        if (b0 == 0.0f) b0 = 1.0f;
        u = (u0 - z2) / b0;
        
        // 4. Output saturation
        if (u > max_u) u = max_u;
        if (u < -max_u) u = -max_u;
        
        return u;
    }
    
    void setParams(float _b0, float _wc, float _wo) {
        b0 = _b0;
        wc = _wc;
        wo = _wo;
    }

    void reset(float current_y) {
        z1 = current_y;
        z2 = 0;
        u = 0;
    }
};
