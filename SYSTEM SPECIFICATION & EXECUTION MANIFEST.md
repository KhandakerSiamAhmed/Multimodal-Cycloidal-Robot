# SYSTEM SPECIFICATION & EXECUTION MANIFEST
**Project:** Multimodal Cycloidal-Actuated Manipulator
**Target Build Volume:** 180x180x180mm (FDM Optimized)
**Primary Architecture:** Decentralized Edge-Compute over ESP-NOW
**Kinematic Scope:** 3-DOF (Provisioned for 4-DOF expansion)

---

## 1. HARDWARE TOPOLOGY

### 1.1 Mechanical Subsystems
* **Reducers:** 3D-printed cycloidal gearboxes (zero-backlash configuration).
* **Actuators:**
  * Base (Yaw): NEMA 17 Stepper
  * Joint 1 (Shoulder Pitch): NEMA 23 Stepper (High torque for cantilevered load)
  * Joint 2 (Elbow Pitch): NEMA 17 Stepper
  * End Effector: Compliant fin ray gripper driven by MG996R servo
* **Expansion:** Chassis/wiring provisioned for future NEMA 14/17 wrist joint.

### 1.2 Electrical & Node Subsystems
* **Power Distribution:** Main DC supply bus routed through arm; localized buck converters at each node isolate logic from motor back-EMF.
* **Master Node (Central Controller):**
  * MCU: ESP32-S3
  * Role: Spatial calculations, Inverse Kinematics (IK), Path planning, ESP-NOW Hub.
* **Actuator Nodes (Distributed Controllers):**
  * MCU: ESP32-C6 Super Mini (1 per stepper motor)
  * Feedback: AS5600 magnetic rotary encoders (Absolute positioning)
  * Drivers: TMC2209 (for NEMA 17, micro-stepped/silent), TB6600 (for NEMA 23, high current)
  * Role: Localized Active Disturbance Rejection Control (ADRC) loops.

---

## 2. FIRMWARE & CONTROL LOGIC

### 2.1 Communication Protocol (ESP-NOW)
* **Topology:** Star Network (ESP32-S3 acts as Master, ESP32-C6 units are Slaves).
* **Master Payload to Nodes:** Target Joint Angle (float), Velocity limit (float), Acceleration (float).
* **Node Payload to Master:** Current Angle (float), Error State (boolean).

### 2.2 Local Node Control (ADRC)
* Standard PID is bypassed in favor of Active Disturbance Rejection Control.
* An Extended State Observer (ESO) runs on each ESP32-C6.
* **Control Law representation:** `u(t) = (u0(t) - z3(t)) / b0`
  * `u0(t)` = Ideal calculated control effort
  * `z3(t)` = Total estimated disturbance (internal friction + external load)
  * `b0` = System gain parameter

### 2.3 Master Kinematics & Profiling
* **Inverse Kinematics (IK):** ESP32-S3 translates global (X, Y, Z) targets into local (Theta_1, Theta_2, Theta_3) joint targets.
* **Motion Profiling:** ESP32-S3 calculates an S-Curve velocity profile to prevent mechanical jerk and gear skipping during trajectory execution.

---

## 3. MULTIMODAL SOFTWARE PIPELINES

### 3.1 Autonomous Vision Pipeline
* **Environment:** Host PC (Laptop) running Python/OpenCV.
* **Input:** Global overhead USB/IP Camera.
* **Execution:** Detects object contours, categorizes color limits, generates physical Cartesian coordinates (X_obj, Y_obj, Z_obj) and target bucket coordinates.
* **Output:** Transmits coordinate arrays to ESP32-S3 via Serial or Wi-Fi TCP/IP.

### 3.2 Voice Command Integration
* **Environment:** Host PC / Natural Language Processing (NLP) API.
* **Execution:** Parses spoken intent -> Extracts [Target Object Color] and [Destination].
* **Logic:** Cross-references extracted entities with the active Vision Pipeline coordinate dictionary -> Dispatches execution command to ESP32-S3.

### 3.3 Kinesthetic Teleoperation (Leader-Follower)
* **Hardware:** Non-actuated 1:1 scale replica of the main arm.
* **Sensors:** AS5600 encoders on replica joints.
* **Execution:** Embedded ESP32 samples replica joint angles at high frequency -> Broadcasts directly to slave ESP32-S3 via ESP-NOW for low-latency mimicry.

---

## 4. SYSTEM STATE MACHINE

| State | Trigger | Active Subsystems | Execution Routine |
| :--- | :--- | :--- | :--- |
| **IDLE** | Power On | All | Await instruction. Hold current motor positions using local ADRC. |
| **VISION_SORT** | Web App / PC | Camera, PC, Master, Nodes | PC maps (X,Y,Z). Master calculates IK and S-Curve. Nodes execute move. |
| **VOICE_SORT** | Audio Input | NLP API, PC, Master, Nodes | Parse intent, lookup coordinates from Vision logic, execute IK. |
| **TELEOP** | UI Rotary Encoder | Replica Arm, Master, Nodes | Stream raw angle data from Replica to Master. Master bypasses IK, sends direct targets to Nodes. |
| **TEACH_MODE** | UI Rotary Encoder | Master, Nodes | User manually moves arm. Master logs AS5600 coordinates to memory array. |
| **REPEAT_MODE** | UI Rotary Encoder | Master, Nodes | Master loops over saved memory array with S-Curve interpolation. |

---

## 5. REPOSITORY ARCHITECTURE

```text
Multimodal-Cycloidal-Robot/
├── README.md                          # High-level overview & quick-start
├── CONTRIBUTING.md                    # Guidelines for future researchers
├── Hardware/                          
│   ├── Mechanical_CAD/                
│   │   ├── SolidWorks_Source/         # Native .sldprt, .sldasm
│   │   └── Manufacturing_Exports/     # .STEP (Universal) and .STL (Sliced for FDM)
│   └── Electronics/                   
│       ├── Schematics/                # Wiring diagrams, DC bus routing
│       └── PCB_Gerbers/               # Custom board manufacturing files
├── Firmware/                          
│   ├── Master_ESP32S3/                # IK solver, S-Curve engine, ESP-NOW hub
│   ├── Node_ESP32C6/                  # ADRC algorithms, AS5600 read, TMC2209/TB6600 step generation
│   └── Teleop_Controller/             # Kinesthetic sampling logic
├── Software/                          
│   ├── Vision_Pipeline/               # OpenCV scripts, object tracking
│   └── Voice_Integration/             # NLP API integration scripts
└── Documentation/                     
    ├── BOM.csv                        # Part numbers, suppliers, costs
    └── Assembly_Instructions.md       # Integration guide