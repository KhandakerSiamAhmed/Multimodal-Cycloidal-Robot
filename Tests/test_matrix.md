# Test Matrix — Multimodal Cycloidal-Actuated Manipulator

This document tracks all isolated and integration test results.

## Track A: Hardware Tests

| Test ID | Test Name | Status | Date | Notes |
|---|---|---|---|---|
| `A-MECH-01` | Cycloidal gear backlash | ⬜ NOT RUN | — | Target: < 0.5° |
| `A-MECH-02` | Gear load test (1.5×) | ⬜ NOT RUN | — | Shoulder joint priority |
| `A-MECH-03` | AS5600 encoder accuracy | ⬜ NOT RUN | — | Target: ±0.5° over 360° |
| `A-ELEC-01` | Buck converter regulation | ⬜ NOT RUN | — | Target: 3.3V ±5% |
| `A-ELEC-02` | Motor driver smoke test | ⬜ NOT RUN | — | ±90° on each motor |
| `A-INTEG-01` | Full arm assembly | ⬜ NOT RUN | — | No binding on any joint |

## Track B: Firmware Tests

| Test ID | Test Name | Status | Date | Notes |
|---|---|---|---|---|
| `B-ADRC-01` | Single-joint step response | ⬜ NOT RUN | — | Target: ±0.5° in 500ms |
| `B-ADRC-02` | Disturbance rejection | ⬜ NOT RUN | — | Target: ±1° in 200ms |
| `B-NOW-01` | ESP-NOW round-trip latency | ⬜ NOT RUN | — | Target: < 5ms |
| `B-NOW-02` | ESP-NOW 10K packet stress | ⬜ NOT RUN | — | Target: < 0.1% loss |
| `B-IK-01` | IK solver unit tests | ⬜ NOT RUN | — | Analytical verification |
| `B-SCURVE-01` | S-Curve profile validation | ⬜ NOT RUN | — | Visual + numerical |
| `B-MULTI-01` | 3-joint coordinated move | ⬜ NOT RUN | — | All joints ±1° target |

## Track C: Software Tests

| Test ID | Test Name | Status | Date | Notes |
|---|---|---|---|---|
| `C-VIS-01` | Color detection accuracy | ⬜ NOT RUN | — | Target: ≥95% on 50 images |
| `C-VIS-02` | Coordinate calibration | ⬜ NOT RUN | — | Target: < 3mm error |
| `C-VIS-03` | Pipeline FPS | ⬜ NOT RUN | — | Target: ≥15 FPS |
| `C-NLP-01` | Intent parsing accuracy | ⬜ NOT RUN | — | Target: ≥90% on 30 sentences |
| `C-NLP-02` | End-to-end voice command | ⬜ NOT RUN | — | Target: < 2s latency |
| `C-SER-01` | Serial mock test | ⬜ NOT RUN | — | JSON round-trip |

## Grand Merge Gates

| Gate | Status | Date | Prerequisites |
|---|---|---|---|
| Gate 1: FW on HW | ⬜ NOT PASSED | — | A-INTEG-01 + B-MULTI-01 |
| Gate 2: Serial Link | ⬜ NOT PASSED | — | Gate 1 + C-SER-01 |
| Gate 3: Vision Auto | ⬜ NOT PASSED | — | Gate 2 + C-VIS-02 |
| Gate 4: Full Multimodal | ⬜ NOT PASSED | — | Gate 3 + all states |

### Status Legend
- ⬜ NOT RUN
- ✅ PASS
- ❌ FAIL
- 🔄 IN PROGRESS
