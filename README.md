<p align="center">
  <img src="Logo.jpg" alt="Multimodal Cycloidal-Actuated Manipulator" width="200"/>
</p>

<h1 align="center">Multimodal Cycloidal-Actuated Manipulator</h1>

<p align="center">
  <strong>3-DOF Robotic Arm with Decentralized Edge-Compute Control over ESP-NOW</strong>
</p>

---

## Overview

A 3-DOF fully 3D-printed robotic manipulator using **cycloidal gearboxes** paired with **GT2 timing belt** reduction for smooth, high-torque, zero-backlash movement. The system is controlled by a network of ESP32 microcontrollers communicating over ESP-NOW in a decentralized architecture — each joint runs its own ADRC control loop locally.

---

## Hardware Setup

### Drivetrain

Each joint uses a **two-stage reduction**:

| Stage | Type | Ratio | Notes |
|-------|------|-------|-------|
| 1 | 3D-printed cycloidal gearbox | 11:1 | Zero-backlash, FDM optimized |
| 2 | GT2 timing belt pulley | 2:1 | 20T driver → 40T driven, 6mm belt |
| **Total** | | **22:1** | |

- **Shoulder & Elbow:** 11:1 cycloidal + 2:1 GT2 belt (22:1 total) — confirmed.
- **Base (Yaw):** Ratio TBD — torque calculation pending.

#### GT2 Pulley Specs (3D Printed)

| Part | Teeth | Pitch Dia | Notes |
|------|-------|-----------|-------|
| Driver pulley (motor side) | 20T | ~12.7 mm | GT2 2mm pitch, 6mm belt width |
| Driven pulley (output side) | 40T | ~25.5 mm | GT2 2mm pitch, 6mm belt width |
| Belt | — | — | GT2 6mm wide, closed loop (length depends on center-to-center distance) |

### Actuators

| Joint | Motor | Driver | Reduction |
|-------|-------|--------|-----------|
| Base (Yaw) | NEMA 17 | TMC2209 | TBD |
| Shoulder (Pitch) | NEMA 23 | TB6600 | 22:1 (11:1 cycloidal + 2:1 GT2) |
| Elbow (Pitch) | NEMA 17 | TMC2209 | 22:1 (11:1 cycloidal + 2:1 GT2) |
| End Effector | MG996R servo | — | Fin-ray compliant gripper |

### Electronics

| Component | Part | Quantity | Role |
|-----------|------|----------|------|
| Master Node | ESP32-S3 | 1 | IK solver, ESP-NOW hub, state machine |
| Joint Nodes | ESP32-C6 Super Mini | 3 | Local ADRC control loops (one per joint) |
| Encoders | AS5600 (I2C) | 3+ | Absolute position feedback per joint |
| NEMA 17 Drivers | TMC2209 | 2 | Silent micro-stepping (Base + Elbow) |
| NEMA 23 Driver | TB6600 | 1 | High-current (Shoulder) |
| Power | 24V DC supply + buck converters | — | Main bus with localized 3.3V regulation |

### Communication

- **Master ↔ Nodes:** ESP-NOW (star topology, packed C structs)
- **PC ↔ Master:** Wi-Fi TCP/IP (JSON)

### Control

- **ADRC** (Active Disturbance Rejection Control) on each joint node — replaces traditional PID.
- **Inverse Kinematics** on the Master ESP32-S3 for Cartesian positioning.
- **S-Curve** velocity profiling on the Master to prevent jerk.

---

## Repository Structure

```
Multimodal-Cycloidal-Robot/
├── README.md                 ← You are here
├── PROGRESS_STATE.md         ← Project status tracker
├── LICENSE
├── Logo.jpg
├── .gitignore
├── .gitattributes
└── Documentation/
    └── BOM.csv               ← Bill of materials
```

Code (firmware & software) will be added as it is written.

---

## Printer

**Bambulab A1 mini** — 180×180×180mm build volume. All mechanical parts are FDM-optimized for this printer. Gear parts printed in PETG or Nylon for wear resistance.
