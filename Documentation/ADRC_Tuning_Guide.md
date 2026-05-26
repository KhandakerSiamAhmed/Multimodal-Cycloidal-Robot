# ADRC Tuning Guide

## Active Disturbance Rejection Control — Per-Joint Tuning Procedure

> **Status:** Template — To be populated during `B-ADRC-01` and `B-ADRC-02` tuning sessions.

---

## Theory Summary

ADRC replaces traditional PID with an Extended State Observer (ESO) that estimates total disturbance in real-time:

```
u(t) = (u0(t) - z3(t)) / b0

Where:
  u0 = Tracking differentiator output (desired control effort)
  z3 = ESO estimate of total disturbance
  b0 = System gain parameter
```

### Key Tuning Parameters

| Parameter | Symbol | Description | Typical Range |
|---|---|---|---|
| System gain | `b0` | Motor torque-to-angle gain | 50 – 500 |
| Observer bandwidth | `ωo` | How fast ESO tracks disturbance | 5× – 10× controller BW |
| Controller bandwidth | `ωc` | Closed-loop response speed | 10 – 100 rad/s |

---

## Tuning Procedure (Per Joint)

### Step 1: Determine `b0`

1. Apply known constant voltage/step count to motor
2. Measure steady-state angular velocity
3. Calculate `b0 = Δω / Δu` (velocity change per control input change)
4. Use 50-80% of measured value for robustness

### Step 2: Set Controller Bandwidth `ωc`

1. Start with `ωc = 20 rad/s` (conservative)
2. Send step command (e.g., 0° → 90°)
3. Observe response on serial plotter
4. Increase `ωc` until:
   - Rise time meets spec (< 500ms for ±0.5°)
   - No sustained oscillation
   - Overshoot < 5%

### Step 3: Set Observer Bandwidth `ωo`

1. Start with `ωo = 5 × ωc`
2. Apply hand-load disturbance during motion
3. Observe rejection speed
4. Increase `ωo` until:
   - Disturbance rejected within spec (< 200ms for ±1°)
   - No noise amplification (high-frequency chatter)
5. Rule of thumb: `ωo ∈ [3ωc, 10ωc]`

### Step 4: Re-tune on Physical Arm

After bench tuning, repeat Steps 1-3 on the fully assembled arm. The real arm has different inertia, friction, and coupling effects. Document parameter deltas.

---

## Tuning Log

### Joint 0 (Base / Yaw)

| Parameter | Bench Value | Arm Value | Date |
|---|---|---|---|
| `b0` | — | — | — |
| `ωo` | — | — | — |
| `ωc` | — | — | — |

### Joint 1 (Shoulder / Pitch)

| Parameter | Bench Value | Arm Value | Date |
|---|---|---|---|
| `b0` | — | — | — |
| `ωo` | — | — | — |
| `ωc` | — | — | — |

### Joint 2 (Elbow / Pitch)

| Parameter | Bench Value | Arm Value | Date |
|---|---|---|---|
| `b0` | — | — | — |
| `ωo` | — | — | — |
| `ωc` | — | — | — |
