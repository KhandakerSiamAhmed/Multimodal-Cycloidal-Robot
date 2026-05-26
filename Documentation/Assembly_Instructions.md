# Assembly Instructions

## Multimodal Cycloidal-Actuated Manipulator

> **Status:** Template — To be populated during Track A execution.

---

## Prerequisites

- All parts from [BOM.csv](BOM.csv) procured
- All cycloidal gearboxes printed and passed `A-MECH-01` backlash test
- All structural components printed
- All electronics tested on bench (`A-ELEC-01`, `A-ELEC-02`)

## Assembly Order

### Step 1: Base Joint (Yaw)
- [ ] Mount NEMA 17 stepper to base plate
- [ ] Install cycloidal gearbox
- [ ] Attach AS5600 encoder with diametric magnet
- [ ] Wire TMC2209 driver
- [ ] Mount ESP32-C6 Node 0

### Step 2: Shoulder Joint (Pitch)
- [ ] Mount NEMA 23 stepper to shoulder bracket
- [ ] Install cycloidal gearbox (high-load rated)
- [ ] Attach AS5600 encoder with diametric magnet
- [ ] Wire TB6600 driver
- [ ] Mount ESP32-C6 Node 1

### Step 3: Elbow Joint (Pitch)
- [ ] Mount NEMA 17 stepper to elbow bracket
- [ ] Install cycloidal gearbox
- [ ] Attach AS5600 encoder with diametric magnet
- [ ] Wire TMC2209 driver
- [ ] Mount ESP32-C6 Node 2

### Step 4: End Effector
- [ ] Mount fin ray gripper mechanism
- [ ] Wire MG996R servo
- [ ] Connect servo to ESP32-S3 PWM output

### Step 5: Power Distribution
- [ ] Route main DC bus through arm
- [ ] Install buck converters at each node
- [ ] Verify all voltage rails (`A-ELEC-01`)

### Step 6: Master Node
- [ ] Mount ESP32-S3 at base
- [ ] Wire USB Serial connection to external port
- [ ] Verify ESP-NOW connectivity to all nodes

### Step 7: Mechanical Verification
- [ ] Run `A-INTEG-01`: Verify all joints move freely
- [ ] Check for cable pinching at all joint positions
- [ ] Verify encoder readings through full range of motion

---

## Photos

> Add assembly photos here during construction.
