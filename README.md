<p align="center">
  <img src="Logo.jpg" alt="Multimodal Cycloidal-Actuated Manipulator" width="200"/>
</p>

<h1 align="center">Multimodal Cycloidal-Actuated Manipulator</h1>

<p align="center">
  <strong>3-DOF Robotic Arm with Decentralized Edge-Compute Control over ESP-NOW</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/DOF-3%20(4%20Provisioned)-blue" alt="DOF"/>
  <img src="https://img.shields.io/badge/Control-ADRC-green" alt="Control"/>
  <img src="https://img.shields.io/badge/Comm-ESP--NOW-orange" alt="Communication"/>
  <img src="https://img.shields.io/badge/Platform-ESP32--S3%20%7C%20ESP32--C6-red" alt="Platform"/>
  <img src="https://img.shields.io/badge/CAD-FDM%20Optimized-purple" alt="CAD"/>
</p>

---

## Overview

A fully 3D-printed, multimodal robotic manipulator featuring **cycloidal gearboxes** for zero-backlash joint actuation and a **decentralized control architecture** where each joint runs its own Active Disturbance Rejection Control (ADRC) loop on a dedicated ESP32-C6 microcontroller.

The system supports multiple input modalities:
- **Autonomous Vision Sorting** — Overhead camera + OpenCV detects and classifies objects by color, generating Cartesian pick-and-place coordinates.
- **Voice Command** — Natural language commands ("pick up the red block") are parsed via NLP and cross-referenced with the live vision map.
- **Kinesthetic Teleoperation** — A non-actuated replica arm streams joint angles via ESP-NOW for low-latency leader-follower control.
- **Teach & Repeat** — Manually guide the arm, record waypoints, and replay trajectories with S-Curve interpolation.

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        HOST PC (Python)                        │
│  ┌──────────────┐  ┌──────────────┐  ┌───────────────────────┐ │
│  │ Vision       │  │ Voice NLP    │  │ Serial/TCP Bridge     │ │
│  │ Pipeline     │──│ Intent       │──│ JSON commands to      │ │
│  │ (OpenCV)     │  │ Parser       │  │ ESP32-S3 Master       │ │
│  └──────────────┘  └──────────────┘  └───────────┬───────────┘ │
└──────────────────────────────────────────────────┬─────────────┘
                                                   │ USB Serial / Wi-Fi TCP
┌──────────────────────────────────────────────────┼─────────────┐
│                  ESP32-S3 MASTER NODE             │             │
│  ┌──────────┐  ┌──────────┐  ┌──────────────────┐│             │
│  │ IK       │  │ S-Curve  │  │ ESP-NOW Hub      ││             │
│  │ Solver   │──│ Profiler │──│ (Star Topology)  ││             │
│  └──────────┘  └──────────┘  └──┬───────┬───────┘│             │
└─────────────────────────────────┼───────┼────────┘             │
                     ESP-NOW      │       │       ESP-NOW        │
              ┌───────────────────┘       └──────────────────┐   │
              ▼                                              ▼   │
┌─────────────────────────┐  ┌─────────────────────────┐  ┌──────────────────────────┐
│  ESP32-C6 NODE 0 (Base) │  │  ESP32-C6 NODE 1 (Shldr)│  │  ESP32-C6 NODE 2 (Elbow) │
│  ┌────────┐ ┌─────────┐ │  │  ┌────────┐ ┌─────────┐│  │  ┌────────┐ ┌─────────┐  │
│  │ ADRC   │ │ AS5600  │ │  │  │ ADRC   │ │ AS5600  ││  │  │ ADRC   │ │ AS5600  │  │
│  │ Loop   │ │ Encoder │ │  │  │ Loop   │ │ Encoder ││  │  │ Loop   │ │ Encoder │  │
│  └────┬───┘ └─────────┘ │  │  └────┬───┘ └─────────┘│  │  └────┬───┘ └─────────┘  │
│       ▼                  │  │       ▼                 │  │       ▼                   │
│  ┌─────────┐             │  │  ┌─────────┐            │  │  ┌─────────┐              │
│  │ TMC2209 │→ NEMA 17    │  │  │ TB6600  │→ NEMA 23   │  │  │ TMC2209 │→ NEMA 17    │
│  └─────────┘             │  │  └─────────┘            │  │  └─────────┘              │
└─────────────────────────┘  └─────────────────────────┘  └──────────────────────────┘
```

---

## Hardware Specifications

| Component | Joint 0 (Base/Yaw) | Joint 1 (Shoulder/Pitch) | Joint 2 (Elbow/Pitch) | End Effector |
|---|---|---|---|---|
| **Motor** | NEMA 17 Stepper | NEMA 23 Stepper | NEMA 17 Stepper | MG996R Servo |
| **Driver** | TMC2209 | TB6600 | TMC2209 | PWM |
| **Reducer** | Cycloidal (FDM) | Cycloidal (FDM) | Cycloidal (FDM) | — |
| **Encoder** | AS5600 | AS5600 | AS5600 | — |
| **Controller** | ESP32-C6 Super Mini | ESP32-C6 Super Mini | ESP32-C6 Super Mini | ESP32-S3 (PWM) |
| **Gripper** | — | — | — | Fin Ray Compliant |

**Build Volume:** 180 × 180 × 180 mm (FDM optimized)  
**Power:** Main DC bus with localized buck converters per node  
**Expansion:** Chassis and wiring provisioned for a 4th joint (wrist, NEMA 14/17)

---

## Control Architecture

### ADRC (Active Disturbance Rejection Control)

Each joint node runs a local ADRC loop instead of standard PID. An Extended State Observer (ESO) estimates total disturbance (friction, external loads, model uncertainty) in real-time:

```
u(t) = (u0(t) - z3(t)) / b0

Where:
  u0(t) = Ideal control effort (from tracking differentiator)
  z3(t) = Total estimated disturbance (from ESO)
  b0    = Estimated system gain
```

### Communication Protocol

- **Protocol:** ESP-NOW (connectionless, low-latency)
- **Topology:** Star (ESP32-S3 Master → ESP32-C6 Nodes)
- **Master → Node:** `{joint_id, target_angle, velocity_limit, acceleration, seq_id}`
- **Node → Master:** `{joint_id, current_angle, error_state, seq_id}`

---

## System States

| State | Trigger | Description |
|---|---|---|
| `IDLE` | Power On | Hold positions via ADRC. Await command. |
| `VISION_SORT` | Web App / PC | Camera maps objects → IK → autonomous pick & place |
| `VOICE_SORT` | Microphone | NLP parses intent → coordinates from vision → execute |
| `TELEOP` | Replica Arm | Leader-follower via ESP-NOW joint angle streaming |
| `TEACH_MODE` | UI Switch | Manual guidance, record waypoints to memory |
| `REPEAT_MODE` | UI Switch | Replay recorded waypoints with S-Curve interpolation |

---

## Repository Structure

```
Multimodal-Cycloidal-Robot/
├── Hardware/
│   ├── Mechanical_CAD/
│   │   ├── SolidWorks_Source/         # Native .sldprt, .sldasm (Git LFS)
│   │   └── Manufacturing_Exports/     # .STEP and .STL (Git LFS)
│   └── Electronics/
│       ├── Schematics/                # Wiring diagrams
│       └── PCB_Gerbers/               # Board manufacturing files
├── Firmware/
│   ├── Master_ESP32S3/                # IK, S-Curve, ESP-NOW hub, state machine
│   ├── Node_ESP32C6/                  # ADRC, AS5600, stepper drivers
│   └── Teleop_Controller/            # Kinesthetic sampling logic
├── Software/
│   ├── Vision_Pipeline/               # OpenCV object detection & tracking
│   ├── Voice_Integration/            # NLP intent parsing
│   └── Serial_Bridge/                # PC ↔ ESP32 communication layer
├── Tests/
│   ├── mocks/                         # Mock interfaces for isolated testing
│   └── integration/                  # Cross-track integration tests
└── Documentation/
    ├── BOM.csv                        # Bill of Materials
    ├── Assembly_Instructions.md
    ├── ADRC_Tuning_Guide.md
    └── Calibration_Procedure.md
```

---

## Getting Started

### Prerequisites

| Tool | Version | Purpose |
|---|---|---|
| [PlatformIO](https://platformio.org/) | ≥ 6.x | Firmware build & flash (ESP32-S3, ESP32-C6) |
| [Python](https://python.org/) | ≥ 3.10 | Vision pipeline, voice integration, serial bridge |
| [OpenCV](https://opencv.org/) | ≥ 4.8 | Computer vision processing |
| [Git LFS](https://git-lfs.com/) | ≥ 3.x | Large file support for CAD binaries |
| [SolidWorks](https://www.solidworks.com/) | 2023+ | CAD editing (optional — STEP exports provided) |

### 1. Clone Repository

```bash
git lfs install
git clone https://github.com/<your-username>/Multimodal-Cycloidal-Robot.git
cd Multimodal-Cycloidal-Robot
```

### 2. Firmware Setup

```bash
# Install PlatformIO CLI (if not using VS Code extension)
pip install platformio

# Build & flash Master Node
cd Firmware/Master_ESP32S3
pio run --target upload --upload-port <COM_PORT>

# Build & flash each Actuator Node (repeat for each ESP32-C6)
cd ../Node_ESP32C6
pio run --target upload --upload-port <COM_PORT>
```

### 3. Software Setup

```bash
# Create Python virtual environment
cd Software/Vision_Pipeline
python -m venv venv
venv\Scripts\activate        # Windows
pip install -r requirements.txt

# Run vision pipeline
python vision_main.py

# In a separate terminal — Voice integration
cd ../Voice_Integration
pip install -r requirements.txt
python voice_main.py
```

### 4. Hardware Assembly

See [Assembly_Instructions.md](Documentation/Assembly_Instructions.md) for step-by-step mechanical and electrical assembly guide.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for commit message conventions, branching strategy, and testing requirements.

---

## License

This project is open-source. License TBD.

---

## Acknowledgments

- Cycloidal drive design adapted from open-source parametric generators
- ADRC theory based on Han Jingqing's foundational work
- ESP-NOW protocol documentation from Espressif Systems
