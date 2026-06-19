# Project Overview

## Multimodal Cycloidal-Actuated Manipulator
### A Collaborative Surgical Robotic Arm with Decentralised Edge-Compute Control

---

## Objective

This project presents a **three-degree-of-freedom collaborative robotic manipulator** designed for remote surgical assistance through master-slave teleoperation. The arm is built around custom 3D-printed cycloidal gearboxes and a wireless decentralised control architecture.

It rotates at the base, lifts at the shoulder, and bends at the elbow. A compliant fin-ray gripper is mounted at the end effector for conformable grasping of surgical instruments and delicate objects. The system is classified as a **cobot** — a collaborative robot that augments the surgeon's capability rather than replacing it.

Multiple input modalities are supported and managed through a unified state machine on the master controller:
*   **Real-time teleoperation**
*   **Computer vision-based object detection**
*   **Voice commands**

---

## Problem Statement

Conventional robotic surgical platforms are prohibitively expensive, placing them out of reach for most hospitals and research institutions. Beyond cost, they suffer from:
*   **Centralised Control Bottlenecks:** Standard platforms rely on centralised control architectures where a single processor handles all joint computations, creating communication bottlenecks and single points of failure.
*   **Backlash Issues:** Backlash in standard gear trains introduces positional uncertainty at the end effector — an unacceptable condition in surgical manipulation.
*   **Command Latency:** Latency in the control loop, even in the order of milliseconds, can compromise the safety of a remotely operated procedure.

### The Solution

While the present work is a prototype, the platform is architected around low-cost off-the-shelf components and 3D-printed mechanical parts. A production-grade version built for actual surgical deployment would remain significantly more affordable than existing commercial alternatives. 

The reliance on widely available standard components also ensures easy maintenance and repairability — failed parts can be sourced and replaced without proprietary supply chains or specialised tooling.

This project addresses these constraints by:
1.  **Distributing control intelligence** across the joints.
2.  Employing **cycloidal gearboxes** that deliver near-zero backlash and high shock tolerance.
3.  Utilising a **wireless communication layer** that eliminates the wiring complexity typical of multi-axis robotic systems while maintaining sub-millisecond command latency.

---

## System Architecture & Design

### Drivetrain & Actuation
*   **Two-Stage Speed Reduction:**
    *   **First Stage:** GT2 timing belt system running a 2:1 ratio (20-tooth driver pulley, 40-tooth driven pulley, 6mm-wide closed-loop belt).
    *   **Second Stage:** Custom 3D-printed cycloidal gearbox with an 11:1 ratio, designed for FDM fabrication constraints.
    *   **Combined Reduction:** 22:1 per joint.
*   **Joint Motors & Drivers:**
    *   **Shoulder Joint:** Carries the heaviest cantilevered load and is driven by a NEMA 23 stepper motor with a TB6600 high-current driver.
    *   **Base & Elbow Joints:** Driven by NEMA 17 stepper motors paired with TMC2209 silent drivers configured for micro-stepping.
*   **Structural Links:** 2020 aluminium extrusion profiles and standard off-the-shelf components (bearings, fasteners, timing belts) to maximise reliability and reduce fabrication overhead.

### Electronics & Control Architecture
The electronic architecture follows a distributed node topology:
*   **Master Node (ESP32-S3):** Runs the inverse kinematics solver, S-curve trajectory generation, and system state machine.
*   **Actuator Nodes (ESP32-C6 Super Mini):** Three boards serving as joint-level actuator nodes. Each node reads an AS5600 magnetic encoder over I2C and executes an **Active Disturbance Rejection Control (ADRC)** loop locally. 
    *   *ADRC* estimates and compensates for friction, payload variation, and vibration in real time without requiring an accurate dynamic model.
*   **Inter-Node Communication:** Uses **ESP-NOW** in a star topology, delivering sub-millisecond latency without a Wi-Fi router.

### Input Modalities
1.  **Teleoperation:** Provided through an unactuated replica arm fitted with its own AS5600 encoders and ESP32 node, mirroring the surgeon's hand movements in real time.
2.  **Computer Vision:** An overhead camera feeds an OpenCV pipeline for object detection and spatial localisation.
3.  **Voice Control:** Voice commands are parsed through cloud-based natural language processing as a hands-free tertiary channel.

---

## Expected Outcomes

The expected outcome is a fully functional collaborative surgical robotic arm capable of precise, low-latency remote operation through master-slave teleoperation. The system is expected to demonstrate:
*   Smooth, backlash-free joint motion under varying loads.
*   Seamless wireless coordination between distributed joint controllers.
*   Reliable multi-modal input switching between teleoperation, vision-assisted, and voice-commanded modes.

---

## Future Scope

The current vision pipeline is limited to object detection and basic spatial localisation. A natural extension is the integration of real-time AI models for advanced surgical assistance, including:
*   Instrument tracking
*   Tissue boundary detection
*   Procedural guidance overlays

Such a system would function as an intelligent co-pilot to the chief surgeon during live procedures. This capability is identified as a high-impact addition but is kept outside the present scope to maintain focus on the core mechanical, electronic, and control objectives of the platform.
