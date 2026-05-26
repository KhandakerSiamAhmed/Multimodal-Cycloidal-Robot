# ?? 8-Month Action Plan & Todo List

This is the complete roadmap modified for your heavy from-scratch design needs in the first half of the project. Check off tasks as we complete them.

## ?? Months 1-4: Hardware Design & Basic Actuation
**Goal:** Design everything from scratch, build the physical arm, and write basic code to move it.

### Track A: Hardware (Design & Build)
- [ ] Design cycloidal gearboxes from scratch.
- [ ] Design the fin-ray compliant gripper.
- [ ] Design the structural arm components (base, shoulder, elbow).
- [ ] 3D print all parts and construct the complete mechanical arm.
- [ ] Complete electrical wiring (ESP32 controllers, stepper motors, drivers, and power).

### Track B: Firmware (Basic Actuation)
- [ ] Initialize PlatformIO project for the ESP32-C6 Nodes.
- [ ] Write the AS5600 Magnetic Encoder driver (I2C) and test feedback.
- [ ] Spin the stepper motors with TMC2209/TB6600 drivers.
- [ ] Implement and test basic local control loops to successfully move the assembled arm.

---

## ?? Months 5-8: Multimodal Integration & Testing
**Goal:** Connect the advanced tools (vision, voice, teleoperation) and polish the total system.

### Integration & Firmware (Master)
- [ ] Establish ESP-NOW communication between Master (S3) and Nodes (C6).
- [ ] Implement Inverse Kinematics (IK) calculation on the Master.
- [ ] Implement Master/Slave teleoperation mode.

### Track C: Software (Vision & Voice)
- [ ] Connect PC Python Serial Bridge to Master ESP32 over serial.
- [ ] Set up Python Vision pipeline and map camera pixels to workspace coordinates.
- [ ] Implement Voice (NLP) intent parsing to send semantic commands to the arm.
- [ ] Perform end-to-end system overall testing and validation.
