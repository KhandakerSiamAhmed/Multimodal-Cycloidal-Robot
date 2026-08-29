# Digital Twin — Joint 1 Telemetry Subsystem

**Joint 1 Telemetry Subsystem (ESP32-C6 + AS5600 Magnetic Encoder)**

---

## Overview

This folder contains the browser-based 3D Digital Twin viewer and a Python telemetry bridge client for the Multimodal Cycloidal-Actuated Manipulator.

The initial milestone — **Joint 1 (Base Rotation) Angular Telemetry** — is complete. It features:
- Real-time angle streaming from the ESP32-C6 firmware via Wi-Fi
- A self-hosted, interactive **3D Digital Twin viewer** in the browser (`digital_twin.html`)
- A Python polling client (`fetch_joint_angle.py`) for integration with desktop pipelines or ROS

```
+-----------------------------------------------------------------------------------+
|                            PHYSICAL ROBOTIC ARM                                   |
|                                                                                   |
|  [ NEMA 17 Motor ] === [ Diametric Magnet ]                                       |
|                               | (0.5 - 1.5mm air gap)                             |
|                               v                                                   |
|                      [ AS5600 12-Bit Encoder ]                                    |
|                               |                                                   |
|                               | I2C (SDA: GPIO 6, SCL: GPIO 7)                    |
|                               v                                                   |
|                   [ ESP32-C6 Super Mini MCU ]                                     |
+-------------------------------|---------------------------------------------------+
                                | Wi-Fi 6 / 802.11 b/g/n (mDNS: joint1.local)
                                v
+-----------------------------------------------------------------------------------+
|                        CYBER / DIGITAL TWIN LAYER                                 |
|                                                                                   |
|  +------------------------+  +------------------------+  +---------------------+  |
|  |   Built-in Dark Web    |  |   Python Data Stream   |  |  3D Digital Twin    |  |
|  | Dashboard (HTTP Port80)|  |   (fetch_joint_angle)  |  | (digital_twin.html) |  |
|  +------------------------+  +------------------------+  +---------------------+  |
+-----------------------------------------------------------------------------------+
```

---

## Files

| File | Purpose |
|------|---------|
| `digital_twin.html` | Browser-based 3D Digital Twin viewer (Three.js / WebGL) |
| `nema17.glb` | NEMA 17 motor 3D model asset used by the viewer |
| `fetch_joint_angle.py` | Python client — polls the ESP32's `/angle` endpoint and prints live data |

---

## How to Run the Digital Twin Viewer

1. Make sure the ESP32-C6 firmware is flashed and running (see `Firmware/Joint1_ESP32C6_AS5600/README.md`).
2. Find the ESP32's IP address from the Serial Monitor (e.g. `192.168.1.50`).
3. Open `digital_twin.html` in your browser — it will connect to the ESP32 and animate the 3D joint in real time.

> If your browser blocks local file access, serve the folder with a simple HTTP server:
> ```bash
> # Python (run from inside the Digital-Twin/ folder)
> python -m http.server 8080
> ```
> Then open `http://localhost:8080/digital_twin.html`

---

## How to Run the Python Client

```bash
# Install requests if needed
pip install requests

# Run the polling client (replace with your ESP32's IP)
python fetch_joint_angle.py
```

Expected output:
```
Joint 1 Angle: 142.35°
Joint 1 Angle: 145.10°
...
```

---

## API Endpoints (ESP32 Firmware)

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/angle` | GET | `{"joint1": 142.35}` — fast angle read |
| `/status` | GET | Full diagnostics (angle, AGC, magnet health, RSSI) |
| `/api/zero` | POST | Reset position to 0.00° |
| `/` | GET | Live dark-mode web dashboard |

---

## Development Roadmap

```
Phase 1: Joint 1 Subsystem       [COMPLETE]
    ↓
Phase 2: Multi-Joint Expansion   (Joints 2 & 3)
    ↓
Phase 3: WebSocket / MQTT        (Low-latency real-time stream)
    ↓
Phase 4: Full 3D Twin Viewer     (All joints animated)
    ↓
Phase 5: Closed-Loop Feedback    (Missed-step & anomaly detection)
```
