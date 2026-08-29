<p align="center">
  <img src="Logo.jpg" alt="Multimodal Cycloidal-Actuated Manipulator" width="200"/>
</p>

<h1 align="center">Multimodal Cycloidal-Actuated Manipulator</h1>

<p align="center">
  <strong>3-DOF Collaborative Robotic Arm — Decentralized Wireless Control — Cycloidal Gearboxes — Multi-Modal Input</strong>
</p>

---

## What Is This?

A **3-DOF prototype collaborative robotic arm (cobot)** built for light-assembly and laboratory automation tasks. The arm is fully 3D-printed (structural parts and gearboxes), runs on low-cost off-the-shelf electronics, and uses a **decentralized wireless control architecture** — each joint thinks for itself.

Instead of one central computer controlling everything, each joint has its own microcontroller running a local feedback control loop. They all talk wirelessly to a master controller that handles the high-level math (inverse kinematics, trajectory planning, and input routing).

---

## Why It's Built This Way

| Problem | Solution |
|---------|----------|
| Backlash in standard gears → positional error | 3D-printed **cycloidal gearboxes** (near-zero backlash) |
| Centralized compute bottleneck | **Distributed ESP32-C6 nodes** — one per joint, each with its own ADRC loop |
| Wiring harness clutter & cable fatigue | **ESP-NOW wireless** — no data cables between joints |
| Commercial cobots cost tens of thousands | Entire BOM costs ~$130 USD |

---

## System Architecture

```
[ Input Modalities ]              [ Master Node ]                       [ Joint Nodes ]
--------------------              ---------------                       ---------------
(1) Teleoperation
    Replica Arm -------->
    + AS5600 Encoders
                                  +-----------------------+   ESP-NOW    +-------------------------------+
(2) Computer Vision               | ESP32-S3 Master Node  |  (Star Topo) | Node 1: Base Joint            |
    Camera -> OpenCV   ---------> | - Inverse Kinematics  | -----------> | - ESP32-C6 | ADRC | AS5600    |
                                  | - S-Curve Trajectory  |   (< 1 ms)   | - NEMA 17 + TMC2209           |
(3) Voice Commands                | - State Machine       |              +-------------------------------+
    Cloud NLP          ---------> +-----------+-----------+
                                              |               ESP-NOW    +-------------------------------+
                                              +------------------------> | Node 2: Shoulder Joint        |
                                              |               (< 1 ms)   | - ESP32-C6 | ADRC | AS5600    |
                                              |                          | - NEMA 23 + TB6600            |
                                              |                          +-------------------------------+
                                              |               ESP-NOW    +-------------------------------+
                                              +------------------------> | Node 3: Elbow Joint           |
                                                              (< 1 ms)   | - ESP32-C6 | ADRC | AS5600    |
                                                                         | - NEMA 17 + TMC2209           |
                                                                         +-------------------------------+
```

---

## Hardware

### Drivetrain — Each Joint (Identical)

| Stage | Type | Ratio | Notes |
|-------|------|-------|-------|
| 1 | GT2 timing belt | 2:1 | 20T driver → 40T driven, 6 mm belt width |
| 2 | 3D-printed cycloidal gearbox | 11:1 | FDM-optimized, near-zero backlash |
| **Total** | | **22:1** | |

### Joints

| Joint | Motor | Driver | Role |
|-------|-------|--------|------|
| Base (Yaw) | NEMA 17 | TMC2209 | Rotates the entire arm horizontally |
| Shoulder (Pitch) | NEMA 23 | TB6600 | Lifts — highest cantilevered load |
| Elbow (Pitch) | NEMA 17 | TMC2209 | Extends / retracts the forearm |
| End Effector | MG996R Servo | — | Fin-Ray compliant gripper |

### Electronics

| Component | Part | Qty | Role |
|-----------|------|-----|------|
| Master Node | ESP32-S3 | 1 | IK solver, trajectory planner, state machine |
| Joint Nodes | ESP32-C6 Super Mini | 3 | Local ADRC control loop per joint |
| Encoders | AS5600 (I2C, 12-bit) | 3+ | Absolute position feedback per joint |
| Silent Drivers | TMC2209 | 2 | NEMA 17 joints (micro-stepping) |
| High-current Driver | TB6600 | 1 | NEMA 23 shoulder joint |
| Power | 24 V DC + buck converters | — | Main bus + localized 3.3 V regulation |
| Structure | 2020 aluminum extrusion | — | Arm links |
| Printer | Bambulab A1 Mini | — | 180×180×180 mm, PETG/Nylon gears |

### Communication & Control

- **Joint ↔ Master:** ESP-NOW (star topology, < 1 ms latency, no Wi-Fi router needed)
- **PC ↔ Master:** Wi-Fi TCP/IP (JSON)
- **Joint Control:** ADRC (Active Disturbance Rejection Control) — handles friction, load variation, and vibration without needing an accurate dynamic model
- **Trajectory:** S-curve velocity profiling on the Master to prevent jerk

---

## Repository Structure

```
Multimodal-Cycloidal-Robot/
│
├── README.md                          ← You are here
├── LICENSE
├── Logo.jpg
├── .gitignore
├── .gitattributes
│
├── Documentation/
│   ├── Project_Overview.md            ← Full system documentation
│   ├── BOM.csv                        ← Bill of materials with costs
│   ├── PROGRESS_STATE.md              ← Session log & project status tracker
│   └── Hardware/
│       ├── README.md                  ← Component index & wiring overview
│       ├── as5600_encoder.md          ← AS5600 magnetic encoder datasheet notes
│       ├── esp32c6_super_mini.md      ← ESP32-C6 Super Mini pinout & specs
│       └── tmc2209_stepper_driver.md  ← TMC2209 driver setup & configuration
│
├── Firmware/
│   └── Joint1_ESP32C6_AS5600/
│       ├── README.md                  ← Wiring guide, flash instructions, API docs
│       ├── Joint1_ESP32C6_AS5600.ino  ← Main sketch (HTTP server, OTA, telemetry)
│       ├── AS5600_Driver.h            ← Low-level I2C encoder driver
│       ├── adrc.h                     ← ADRC controller implementation
│       ├── pid.h                      ← PID controller (reference / fallback)
│       ├── pinout.h                   ← GPIO pin definitions
│       ├── web_dashboard.h            ← Self-contained web dashboard (served from flash)
│       └── platformio.ini             ← PlatformIO build config for ESP32-C6
│
├── Digital-Twin/
│   ├── README.md                      ← How to run the viewer and Python client
│   ├── digital_twin.html              ← Browser-based 3D joint viewer (Three.js)
│   ├── nema17.glb                     ← NEMA 17 3D model asset
│   └── fetch_joint_angle.py           ← Python telemetry polling client
│
└── CAD/
    ├── Body.SLDPRT                    ← Motor mount / body (SolidWorks)
    ├── shaft.SLDPRT                   ← Shaft part (SolidWorks)
    ├── nema17.SLDASM                  ← NEMA 17 assembly (SolidWorks)
    └── nema17.STEP                    ← NEMA 17 neutral STEP export
```

---

## Current Status

> **Phase: Semester 1 — Hardware Design & Basic Actuation**

| Milestone | Status |
|-----------|--------|
| Hardware spec finalized (drivetrain, electronics) | ✅ Done |
| Bill of Materials created | ✅ Done |
| Joint 1 firmware (ESP32-C6 + AS5600) | ✅ Done |
| Joint 1 Digital Twin viewer | ✅ Done |
| Cycloidal gearbox CAD design | 🔲 Next |
| GT2 pulley CAD (20T/40T, 6 mm belt) | 🔲 Next |
| Physical build & assembly | 🔲 Next |
| Joints 2 & 3 firmware nodes | 🔲 Next |
| Master ESP32-S3 IK solver | 🔲 Next |
| Full teleoperation pipeline | 🔲 Semester 2 |
| Computer vision integration | 🔲 Semester 2 |
| Voice command integration | 🔲 Semester 2 |

---

## Quick Start

### Flash the Joint 1 Firmware

See [`Firmware/Joint1_ESP32C6_AS5600/README.md`](Firmware/Joint1_ESP32C6_AS5600/README.md) for full wiring diagrams, pin maps, and step-by-step flash instructions for both Arduino IDE and PlatformIO.

### Run the Digital Twin Viewer

See [`Digital-Twin/README.md`](Digital-Twin/README.md) for instructions on opening the 3D viewer in your browser and running the Python telemetry client.

---

## Documentation

| Document | Description |
|----------|-------------|
| [`Documentation/Project_Overview.md`](Documentation/Project_Overview.md) | Full system architecture, problem statement, subsystem specs |
| [`Documentation/BOM.csv`](Documentation/BOM.csv) | Complete bill of materials with supplier info and costs |
| [`Documentation/Hardware/`](Documentation/Hardware/) | Component datasheets: AS5600, ESP32-C6, TMC2209 |
| [`Documentation/PROGRESS_STATE.md`](Documentation/PROGRESS_STATE.md) | Session-by-session project log |

---

## License

[MIT License](LICENSE)
