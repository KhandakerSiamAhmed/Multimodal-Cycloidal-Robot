# Multimodal Cycloidal-Actuated Manipulator — Product Documentation

Version: 0.1-prototype
Date: 2026-05-27
Authors: Project Team

Purpose
-------
This document is the single comprehensive product-level documentation for the
Multimodal Cycloidal-Actuated Manipulator. It explains what the project is,
why it exists, how it works, how to build and test it, and which files cover
which responsibilities. It is written to industry standards so new engineers
and integrators can pick up the project and continue development, testing,
or manufacturing.

Contents
--------
- Executive Summary
- System Overview & Architecture
- Hardware Specification
- Electrical & Wiring
- Firmware (Master & Node)
- Software (Vision, Voice, Serial Bridge)
- Interface Contracts & Data Schemas
- Build & Flash Instructions (When / Where / How)
- Assembly & Calibration Procedures
- Tests, Validation & Gate Criteria
- Project Phases, Schedule & Responsibilities
- File Map: Which file covers what
- BOM & Procurement
- Risks & Mitigations
- Maintenance, Support, and Troubleshooting
- Appendices (quick commands, contacts)

Executive Summary
-----------------
The Multimodal Cycloidal-Actuated Manipulator is a 3-DOF robotic arm (provisioned
for 4-DOF) using 3D-printed cycloidal gear reducers and a decentralized
control architecture. Each joint runs ADRC locally on an ESP32-C6 node; a
central ESP32-S3 master performs IK, motion profiling and acts as an ESP-NOW
hub. Input modalities include Vision, Voice, Teleoperation, and Teach/Repeat.

Goals
- Produce a robust, low-latency manipulator prototype for pick-and-place tasks.
- Demonstrate decentralized ADRC-based control for smooth, disturbance-resistant motion.
- Provide an extensible software stack (vision, voice, serial bridge) with clear
  contracts to enable parallel development by hardware, firmware, and software teams.

System Overview & Architecture
------------------------------
High-level components
- Host PC (Python): Vision, Voice NLP, Serial/TCP bridge.
- ESP32-S3 Master: IK solver, S-Curve profiler, state machine, ESP-NOW hub.
- ESP32-C6 Nodes (3): Local ADRC loops, AS5600 encoder inputs, stepper drivers.
- Mechanical: Cycloidal reducers, NEMA 17/23 motors, fin-ray gripper.

Communication
- Master ↔ Nodes: ESP-NOW (star topology) using packed C structs.
- PC ↔ Master: Newline-delimited JSON over USB Serial (115200) or TCP (8080).

Control Flow
1. Vision/Voice produce target coordinates or intent.
2. PC sends JSON to Master (MOVE_XYZ / MOVE_JOINTS / SET_MODE).
3. Master runs IK → S-Curve → emits MasterToNodePayload over ESP-NOW.
4. Nodes execute ADRC at high rate and report NodeToMasterPayload back.

Hardware Specification
----------------------
Mechanical
- Joints: 3 cycloidal gearboxes (FDM printed), zero-backlash design.
- Motors: NEMA 17 (base & elbow), NEMA 23 (shoulder).
- End effector: Fin-ray compliant gripper driven by MG996R.

Electronics
- Master: ESP32-S3 (USB Serial, PWM for gripper), power distribution.
- Nodes: ESP32-C6 Super Mini, AS5600 I2C encoder, TMC2209/TB6600 drivers.
- Power: 24V main bus, buck converters at each node for 3.3V logic.

Mechanical tolerances & materials
- Print gear parts in PETG or Nylon for wear resistance; target ±0.1mm.
- Recommended print settings: 0.12mm layer height, 100% infill on teeth.

Electrical & Wiring
------------------
Wiring guidelines
- Route high-current motor power separately from encoder and MCU signal
  wiring to minimize noise.
- Use common ground; decouple motor drivers with local bulk and ceramic caps.
- Place buck converters close to the ESP32 modules.

Connectors
- Use keyed JST or Molex connectors for signal/power at each joint to enable
  modular replacement.

Firmware (Master & Node)
------------------------
Master (ESP32-S3)
- Responsibilities: IK solver, S-Curve profiler, state machine, ESP-NOW hub,
  PC serial/TCP bridge.
- Key files (expected): `Firmware/Master_ESP32S3/src/main.cpp`, `ik_solver.*`,
  `s_curve.*`, `espnow_hub.*`, `serial_bridge.*`, `state_machine.*`.

Node (ESP32-C6)
- Responsibilities: ADRC loop, AS5600 reading, motor driver interface,
  peer ESP-NOW handling.
- Key files (expected): `Firmware/Node_ESP32C6/src/main.cpp`, `adrc_controller.*`,
  `as5600_driver.*`, `stepper_driver.*`, `espnow_node.*`.

Build system
- PlatformIO is used for firmware builds. Each platform has a `platformio.ini`.

Software (Vision, Voice, Serial Bridge)
-------------------------------------
Vision Pipeline (Host PC)
- Core responsibilities: camera capture, detection/classification, pixel→mm
  mapping, object list output.
- Key files: `Software/Vision_Pipeline/vision_main.py`, `color_detector.py`,
  `calibration.py`, `coordinate_mapper.py`.

Voice Integration
- Responsibilities: audio capture, intent parsing, mapping intents to target
  coordinates or commands.
- Key files: `Software/Voice_Integration/voice_main.py`, `intent_parser.py`.

Serial Bridge
- Responsibilities: JSON schema parsing/validation, connection to Master over
  USB Serial or TCP.
- Key files: `Software/Serial_Bridge/bridge.py`.

Interface Contracts & Data Schemas
---------------------------------
Firmware Contracts (source-of-truth)
- `Firmware/Contracts/master_to_node_payload.h` — Master→Node payload (packed struct).
- `Firmware/Contracts/node_to_master_payload.h` — Node→Master payload (packed struct).
- `Firmware/Contracts/serial_commands.h` — JSON command schema and command constants.

Design rules
- Always keep contract versions in headers synchronized between Master and Node.
- Serial messages are newline-terminated JSON, maximum 256 bytes.

Build & Flash Instructions (When / Where / How)
-----------------------------------------------
Prerequisites
- Windows host examples below; similar commands work on Linux/macOS.
- Install PlatformIO and Python 3.10+.

PlatformIO firmware build & upload (example)
```powershell
# Install PlatformIO (if not installed)
python -m pip install -U platformio

# Build Master
cd Firmware\Master_ESP32S3
pio run

# Upload (specify COM port)
pio run --target upload --upload-port COM3

# Build & upload Node
cd ..\Node_ESP32C6
pio run
pio run --target upload --upload-port COM4
```

Python software setup
```powershell
cd Software\Vision_Pipeline
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
python vision_main.py

# Voice integration
cd ..\Voice_Integration
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
python voice_main.py
```

Where to flash
- Flash Master (ESP32-S3) via the USB port on the base computer when the
  board is accessible (typically during Phase 1 for bench testing; Phase 2 on
  the physical arm after assembly).
- Flash Nodes via individual USB-to-serial adapters before installation into
  the arm, or by temporarily exposing their USB ports after mounting.

Assembly & Calibration Procedures
---------------------------------
High-level assembly sequence
1. Assemble each gearbox and verify `A-MECH-01` backlash test on bench.
2. Wire motor drivers and verify `A-ELEC-01` voltage rails.
3. Mount encoders, magnets, and validate encoder readings `A-MECH-03`.
4. Install ESP32 nodes and validate basic serial/driver control.

Calibration steps
- Camera calibration and homography: `Documentation/Calibration_Procedure.md`.
- ADRC tuning per `Documentation/ADRC_Tuning_Guide.md`: bench tune `b0`, `ωc`, `ωo`,
  then re-tune on assembled arm.

Tests, Validation & Gate Criteria
--------------------------------
Isolated test lists are maintained in `Tests/test_matrix.md` and summarized in
`PROGRESS_STATE.md`.

Key gates before integration
- Gate 1: Firmware on Physical Arm — pass `B-ADRC-01`, `B-MULTI-01` on physical arm.
- Gate 2: Serial Link — PC → Master JSON commands produce correct responses.
- Gate 3: Vision Auto — Camera-calibrated pick-and-place with ≥80% success.
- Gate 4: Full Multimodal — All six system states functional.

Project Phases, Schedule & Responsibilities
------------------------------------------
The project is organised into a focused 8-month execution schedule split
into two major halves: an intensive hardware-and-actuation first half (Months
1–4) followed by an integration-and-polish second half (Months 5–8).

Phase 0 — Setup (now complete)
- Tasks: repository configuration, PlatformIO skeletons, gather initial DH
  parameters, procurement list and BOM verification.
- Owner: Project lead + repo maintainers.

Phase 1 — Hardware Design & Basic Actuation (Months 1–4)
- Goal: Full-from-scratch mechanical and electrical design; assemble a
  functioning 3-DOF arm and validate basic motion under local node control.
- Track A (Hardware): Design cycloidal reducers, fin-ray gripper, and
  structural components; iterate prints, assemble, and verify backlash.
- Track B (Firmware - Basic): PlatformIO Node project initialization, AS5600
  encoder driver, motor driver bring-up (TMC2209/TB6600), and simple
  local control loops to move the arm safely.
- Track C (Software - Basic): Minimal PC bridge for logging and serial
  connection; basic vision calibration utilities (deferred integration).

Phase 2 — Multimodal Integration & Testing (Months 5–8)
- Goal: Implement master–slave orchestration, full PC multimodal integration
  (vision, voice), and validation for release/demo.
- Firmware (Master): Implement ESP-NOW master hub, inverse kinematics on
  the Master ESP32-S3, S-curve profiler, and coordinated trajectory
  distribution to nodes.
- Software (PC): Full vision pipeline (pixel→mm mapping), voice intent
  parsing, and the Serial/TCP bridge to the Master.
- Integration: End-to-end tests, latency profiling, ADRC tuning on assembled
  hardware, and final documentation for maintainers.

Responsibilities & Handoffs
- Keep the Firmware/Contracts headers (`Firmware/Contracts/*.h`) as the
  authoritative interface for all comms. Update contract versions when
  changing payloads and document the change in the `PROGRESS_STATE.md`.
- Use `PROGRESS_STATE.md` to record gate checks and test outcomes.
- Use `TODO.md` for short-term, actionable steps and check them off as they
  are completed.

File Map: Which file covers what
--------------------------------
- Project root:
  - `README.md` — high-level overview and quick-start.
  - `CONTRIBUTING.md` — branch & commit rules, agent rules.
  - `PROGRESS_STATE.md` — current state, open decisions, and test results.

- Documentation/
  - `Assembly_Instructions.md` — step-by-step mechanical/electrical assembly.
  - `ADRC_Tuning_Guide.md` — ADRC theory and tuning procedure.
  - `Calibration_Procedure.md` — camera calibration and mapping.
  - `BOM.csv` — parts list and procurement status.
  - `PRODUCT_DOCUMENTATION.md` — (this file) consolidated product doc.

- Firmware/Contracts/
  - `master_to_node_payload.h`, `node_to_master_payload.h`, `serial_commands.h`
    — interface schemas (authoritative source-of-truth for comms).

- Firmware/Master_ESP32S3/ — Master firmware (IK, S-curve, hub, serial bridge).
- Firmware/Node_ESP32C6/ — Node firmware (ADRC, drivers, encoder).
- Software/Vision_Pipeline/ — Vision implementation and tests.
- Software/Voice_Integration/ — Voice capture & intent parsing.
- Software/Serial_Bridge/ — PC ↔ Master communication.
- Tests/mocks/ — `mock_esp32_serial.py`, `mock_vision_sender.py` for isolated testing.

BOM & Procurement
-----------------
See `Documentation/BOM.csv` for part numbers and procurement status. Key
components include NEMA 17/23 steppers, ESP32-S3 & ESP32-C6 modules, AS5600
encoders, TMC2209/TB6600 drivers, magnets, and PETG filament for printing.

Risk Assessment & Mitigations
-----------------------------
Risk: Cycloidal gearbox tolerances (High)
- Mitigation: Iterative test prints, ream holes to size, use PETG/Nylon, fallback
  to SLA/MJF for discs.

Risk: ESP-NOW packet loss in enclosure (Medium)
- Mitigation: sequence IDs, heartbeat hold mode, rate limit Master→Node to 50Hz,
  route antennas outside shielding, perform `B-NOW-02` stress tests.

Risk: End-to-end latency (Medium)
- Mitigation: Profile each pipeline stage, reduce image resolution, predictive
  target lead compensation, async pipeline.

Maintenance, Support & Troubleshooting
------------------------------------
Routine checks
- Mechanical: check backlash, fastener torque, and wear on cycloidal pins.
- Electrical: verify buck converter outputs, driver temperatures.
- Firmware: ensure contract versions match across Master and Node.

Troubleshooting tips
- If master reports `IK_NO_SOLUTION`, verify DH parameters in Master IK and
  check workspace limits.
- If encoder faults occur, verify I2C pull-ups, magnet orientation, and wiring.
- For unexplained jitter, reduce Master→Node rate and inspect ESP-NOW packet
  loss logs.

Appendices
---------
Appendix A — Quick Commands
- Run unit tests (firmware): `pio test` inside each PlatformIO project.
- Run Python tests: `pytest` in `Software` subfolders.

Appendix B — Contacts & Ownership
- Repository maintainers: See `CONTRIBUTING.md` for maintainers and code owners.

Appendix C — Next Steps for Integrators
1. Provide precise DH parameters (L1, L2, L3) and joint limits to finalize IK.
2. Confirm PC↔Master comm channel (USB Serial or TCP) for the prototype.
3. Select NLP backend (local Vosk/Whisper or cloud) according to privacy/latency
   needs.

End of Document
