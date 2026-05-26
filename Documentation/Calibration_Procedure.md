# Camera Calibration Procedure

## Vision Pipeline — Pixel-to-Millimeter Coordinate Mapping

> **Status:** Template — To be populated during `C-VIS-02` calibration.

---

## Overview

The overhead camera observes the robot's workspace from a fixed position. A calibration transform maps pixel coordinates `(px, py)` to physical workspace coordinates `(X_mm, Y_mm)` in the robot's reference frame.

## Prerequisites

- Camera mounted in final overhead position
- Calibration grid printed (checkerboard or dot pattern)
- Robot arm in known home position
- `Software/Vision_Pipeline/calibration.py` implemented

## Procedure

### Step 1: Intrinsic Calibration (Lens Distortion)

1. Print a checkerboard pattern (8×6, 25mm squares)
2. Capture 15–20 images of checkerboard at different angles
3. Run OpenCV `calibrateCamera()` to compute:
   - Camera matrix (focal length, principal point)
   - Distortion coefficients
4. Save calibration data to `calibration_data.json`

### Step 2: Extrinsic Calibration (Workspace Mapping)

1. Place calibration markers at known positions in the workspace:
   - Origin (0, 0): Robot base center
   - X-axis reference: (200, 0) mm
   - Y-axis reference: (0, 200) mm
   - Additional points: (100, 100), (150, 50), etc.
2. Capture undistorted camera frame
3. Click on each marker to record pixel coordinates
4. Compute homography matrix `H` using `cv2.findHomography()`
5. Save homography to `calibration_data.json`

### Step 3: Validation

1. Place test objects at 5 known positions
2. Run vision pipeline to detect and map coordinates
3. Compare detected (X, Y) to ground truth
4. Target: < 3mm error across workspace (test `C-VIS-02`)

### Step 4: Z-Axis Estimation

For objects of known height, Z is estimated from the object class or measured height. For general objects:
- Default Z = surface level (configurable)
- Grip height offset applied per object type

---

## Calibration Data

> To be populated after calibration

```json
{
  "camera_matrix": [],
  "distortion_coefficients": [],
  "homography_matrix": [],
  "workspace_bounds_mm": {
    "x_min": -200, "x_max": 200,
    "y_min": -200, "y_max": 200
  },
  "calibration_date": "",
  "error_mm": null
}
```
