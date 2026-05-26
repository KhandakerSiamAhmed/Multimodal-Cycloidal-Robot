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

This project is a 3-DOF fully 3D-printed robotic manipulator. It uses **cycloidal gearboxes** for smooth, zero-backlash movement. The entire system is controlled by a network of ESP32 microcontrollers over ESP-NOW, making it completely decentralized. Each joint handles its own advanced control loops locally.

You can control the arm via:
- **Vision:** An overhead camera detects objects and sends XYZ coordinates.
- **Voice:** Say "pick up the red block" and the arm will find it.
- **Teleoperation:** Move a small replica arm to pilot the main arm intuitively.

## ?? 8-Month Roadmap

We have an 8-month timeline to bring this from full scratch design to a polished, multimodal robot. The schedule is divided into two major halves:

- **Months 1-4: Hardware Design & Basic Actuation**
  - Full CAD design from scratch (cycloidal gearboxes, fin ray gripper, and arm structure).
  - Complete electrical wiring and mechanical assembly of the 3-DOF arm.
  - Writing basic ESP32 firmware to successfully run the physical arm (motor driving, encoder reading).

- **Months 5-8: Multimodal Integration & Testing**
  - Master-slave teleoperation setup.
  - Camera (Vision) and Voice (NLP) integration via PC bridge.
  - Complete end-to-end system testing, tuning, and demonstration.

## ?? Where to look next

- **Ready to start building?** Check [TODO.md](TODO.md) for the exact step-by-step checklist.
- **Want the full engineering details and architecture?** Read the [Product Documentation](Documentation/PRODUCT_DOCUMENTATION.md).
- **Want to see our current status and test results?** Check [PROGRESS_STATE.md](PROGRESS_STATE.md).
