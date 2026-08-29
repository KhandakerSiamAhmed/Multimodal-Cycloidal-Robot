# PROGRESS_STATE.md

> **Read this file at the start of every session. Update it at the end.**

---

## Status

```yaml
PROJECT: Multimodal Cycloidal-Actuated Manipulator
LAST_UPDATED: 2026-06-06
CURRENT_PHASE: Semester 1 — Hardware Design & Basic Actuation
OVERALL_PROGRESS: 5%
```

## What's Done

- Repository initialized and cleaned up
- Hardware specs defined (11:1 cycloidal + 2:1 GT2 belt for shoulder/elbow)
- BOM created
- Timeline set (Semester 1: physical bot + remote control, Semester 2: vision + voice)

## What's Next

- Finalize base joint torque calculation and gear ratio
- Begin cycloidal gearbox CAD design
- Design GT2 pulley system (20T/40T, 6mm belt)
- Order electronic components from BOM

## Open Decisions

| Decision | Status | Impact |
|----------|--------|--------|
| Base joint gear ratio | PENDING — torque calculation needed | Blocks base joint CAD |
| Arm link lengths (DH parameters) | PENDING — awaiting CAD | Blocks IK solver |

## Session Log

```yaml
SESSION_001:
  DATE: 2026-05-26
  WHAT: Initial project planning and repo setup
  DECISIONS:
    - ADRC over PID for joint control
    - ESP-NOW star topology
    - Wi-Fi TCP/IP for PC↔Master comm
    - Google AI Pro (Cloud API) for Voice NLP
    - Bambulab A1 mini as target printer

SESSION_002:
  DATE: 2026-06-06
  WHAT: Major repo cleanup and spec update
  DECISIONS:
    - Drivetrain is 11:1 cycloidal + 2:1 GT2 belt (22:1 total) for shoulder & elbow
    - Base ratio TBD (torque calc pending)
    - GT2 pulleys will be 3D printed (20T driver / 40T driven, 6mm belt)
    - Removed all premature code/template files
    - Simplified repo to essential docs only
    - Timeline restructured to semesters (not months)
  DELETED_FILES:
    - EXECUTION_PLAN.md
    - SYSTEM SPECIFICATION & EXECUTION MANIFEST.md
    - CONTRIBUTING.md
    - TODO.md
    - Documentation/PRODUCT_DOCUMENTATION.tex
    - Documentation/PRODUCT_DOCUMENTATION.md
    - Documentation/Assembly_Instructions.md
    - Documentation/Calibration_Procedure.md
    - Documentation/ADRC_Tuning_Guide.md
```
