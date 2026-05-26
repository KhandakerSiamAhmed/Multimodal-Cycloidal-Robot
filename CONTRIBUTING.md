# Contributing to Multimodal Cycloidal-Actuated Manipulator

## Branching Strategy

| Branch Pattern | Purpose | Example |
|---|---|---|
| `main` | Stable, tested releases only | — |
| `develop` | Integration branch for merged features | — |
| `feature/<track>-<name>` | Isolated feature work | `feature/adrc-node`, `feature/vision-pipeline` |
| `fix/<track>-<name>` | Bug fixes | `fix/firmware-encoder-drift` |
| `integration/<phase>` | Grand Merge phases | `integration/grand-merge` |
| `hardware/<component>` | CAD file updates | `hardware/cycloidal-v2` |

## Commit Message Format

Use [Conventional Commits](https://www.conventionalcommits.org/) with test IDs:

```
<type>(<scope>): <description> [TEST-IDs]

Examples:
feat(adrc): implement Extended State Observer for single joint [B-ADRC-01 ✓]
fix(espnow): reduce packet loss by lowering TX rate to 50Hz [B-NOW-02 ✓]
feat(vision): add HSV-based color detector [C-VIS-01 ✓]
docs(readme): add system architecture diagram
test(ik): add unit tests for edge-case joint configurations [B-IK-01 ✓]
hardware(cad): update shoulder gearbox clearances +0.05mm [A-MECH-01 ✓]
```

**Types:** `feat`, `fix`, `test`, `docs`, `refactor`, `hardware`, `chore`  
**Scopes:** `adrc`, `espnow`, `ik`, `scurve`, `vision`, `voice`, `serial`, `cad`, `electronics`

## Merge Rules

1. All merges to `develop` require passing the relevant isolated test suite
2. All merges use **squash merge** to keep history clean
3. Every merge commit references test IDs that passed
4. `main` only receives merges from `integration/` branches after system acceptance

## Testing Requirements

Before merging any feature branch:

- **Firmware:** All PlatformIO unit tests pass (`pio test`)
- **Software:** All Python tests pass (`pytest`)
- **Hardware:** Physical test results documented in commit message or PR description

## Agent-Specific Rules

If you are an AI agent working on this project:

1. **READ** `PROGRESS_STATE.md` before starting any work
2. **UPDATE** `PROGRESS_STATE.md` at the end of every session
3. **DO NOT** modify files outside your assigned track without explicit instruction
4. **DO NOT** merge branches — request human review for all merges
5. **REFERENCE** test IDs in all commits related to testable deliverables
6. **LOG** all parameter changes (ADRC tuning, calibration values) in `PROGRESS_STATE.md`

## Code Style

### C++ (Firmware)
- PlatformIO with Arduino framework
- 4-space indentation
- Header guards: `#ifndef MODULE_NAME_H`
- All public functions documented with `/** @brief */` comments

### Python (Software)
- Python 3.10+
- Type hints on all function signatures
- Docstrings on all public functions
- Format with `black`, lint with `ruff`

## File Size Limits

- CAD files (`.sldprt`, `.sldasm`, `.STEP`, `.STL`): Git LFS required
- No single source file should exceed 500 lines — refactor if needed
- Test datasets (images): Store in `Tests/` directory, Git LFS for files > 1MB
