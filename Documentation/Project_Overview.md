# 3-DOF Prototype Collaborative Robotic Manipulator (Cobot)

## Project Overview

This project presents a **3-DOF prototype collaborative robotic manipulator (cobot)** developed as a modular platform for automated light-assembly and laboratory tasks. Built around a decentralized wireless control architecture, low-cost off-the-shelf hardware, and 3D-printed cycloidal gearboxes, the system augments human operation through direct multi-modal interaction and real-time disturbance compensation.

---

## Problem Statement & Motivation

* **Prohibitive Cost & Proprietary Barriers:** Closed commercial systems require high capital investment and proprietary supply chains, limiting accessibility for prototyping and research.
* **Centralized Compute Bottlenecks:** Single-processor architectures manage all kinematic and low-level loop computations simultaneously, introducing processing bottlenecks and single points of failure.
* **Joint Backlash:** Conventional low-cost gear trains introduce backlash and mechanical play, causing positional uncertainty at the end effector.
* **Harness Clutter & Latency:** Multi-joint wiring harnesses add mechanical fatigue, while standard networked wireless solutions introduce latency spikes.

---

## Complete System Architecture

```
[ Input Modalities ]              [ Master Coordination ]                [ Distributed Actuation Nodes ]
--------------------              -----------------------                -------------------------------
(1) Teleoperation
    Master Replica Arm  -------->
    + AS5600 Encoders
                                  +-----------------------+   ESP-NOW    +-------------------------------+
(2) Vision / Object Det.          | ESP32-S3 Master Node  |  (Star Topo) | Node 1: Base Joint            |
    Camera -> OpenCV   ---------> | - IK Solver           | -----------> | - ESP32-C6 | ADRC Loop | AS5600|
                                  | - S-Curve Trajectory  |   (< 1 ms)   | - NEMA 17 + TMC2209 Driver    |
(3) Voice Commands                | - State Machine       |              +-------------------------------+
    Cloud NLP          ---------> +-----------+-----------+
                                              |               ESP-NOW    +-------------------------------+
                                              +------------------------> | Node 2: Shoulder Joint        |
                                              |               (< 1 ms)   | - ESP32-C6 | ADRC Loop | AS5600|
                                              |                          | - NEMA 23 + TB6600 Driver     |
                                              |                          +-------------------------------+
                                              |
                                              |               ESP-NOW    +-------------------------------+
                                              +------------------------> | Node 3: Elbow Joint           |
                                                              (< 1 ms)   | - ESP32-C6 | ADRC Loop | AS5600|
                                                                         | - NEMA 17 + TMC2209 Driver    |
                                                                         +-------------------------------+
```

---

## Subsystem Specifications

### 1. Mechanical Arm & Drivetrain

The mechanical structure combines **2020 aluminum extrusion profiles** with 3D-printed brackets and compliant elements. Each joint features an identical **22:1 compound reduction**:

* **Stage 1 (Belt Drive):** GT2 timing belt running a **2:1 ratio** (20T driver to 40T driven pulley, 6 mm belt width).
* **Stage 2 (Cycloidal Gearbox):** Custom 3D-printed cycloidal speed reducer delivering an **11:1 ratio** for near-zero backlash and high shock resistance.
* **End Effector:** Compliant **Fin-Ray mechanical gripper** for adaptive grasping of diverse workpiece geometries.

| Joint Node | Physical Joint | Actuator | Motor Driver | Drivetrain Reduction |
| :--- | :--- | :--- | :--- | :--- |
| **Node 1** | **Base** | NEMA 17 Stepper | TMC2209 (Silent / Micro-stepping) | GT2 (2:1) → Cycloidal (11:1) = **22:1** |
| **Node 2** | **Shoulder** | NEMA 23 Stepper | TB6600 (High-current) | GT2 (2:1) → Cycloidal (11:1) = **22:1** |
| **Node 3** | **Elbow** | NEMA 17 Stepper | TMC2209 (Silent / Micro-stepping) | GT2 (2:1) → Cycloidal (11:1) = **22:1** |

---

### 2. Distributed Control & Electronics Architecture

* **Master Node (ESP32-S3):** Acts as the central supervisor executing inverse kinematics (IK), S-curve trajectory profiling, and unified state machine management.
* **Actuator Nodes (3× ESP32-C6 Super Mini):**
  * **Feedback:** AS5600 12-bit magnetic absolute encoders read real-time joint angles over I2C.
  * **Control Law:** Onboard **Active Disturbance Rejection Control (ADRC)** loop running locally on each ESP32-C6 to actively estimate and cancel nonlinear friction, load disturbances, and dynamic model uncertainties.
* **Wireless Interconnect:** Point-to-point **ESP-NOW star topology** facilitating router-free wireless communication with **< 1 ms latency** directly between the master and joint nodes.

---

### 3. Multi-Modal Input Modalities

* **Teleoperation (Leader-Follower):** Passive unactuated master replica arm fitted with AS5600 magnetic absolute encoders for kinesthetic teaching and direct joint mirroring.
* **Vision / Object Detection:** Overhead camera feeding an **OpenCV pipeline** for coordinate tracking, target identification, and automated spatial localization.
* **Voice Commands:** Hands-free tertiary input processed via a **Cloud NLP** pipeline for state transitions, predefined routine triggers, and emergency stop overrides.

---

## Expected Outcomes & Evolution Path

**Baseline Verification**
* Backlash-free multi-axis synchronized tracking across variable payload profiles.
* Deterministic, sub-millisecond wireless data interchange across all distributed actuator nodes.
* Robust disturbance rejection and vibration suppression via localized ADRC loops.

**Future Enhancements**
* Migration to field-oriented control (FOC) brushless DC (BLDC) actuators for dynamic backdrivability.
* Direct torque sensing for ISO-compliant collaborative power and force limiting (PFL).
* Real-time spatial edge-AI vision overlays for automated obstacle detection and dynamic path replanning.
