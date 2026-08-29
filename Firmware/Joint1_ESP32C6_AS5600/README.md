# Joint 1 Firmware — ESP32-C6 Super Mini + AS5600 Encoder

This folder contains all firmware for **Joint 1 (Base Rotation)** of the Multimodal Cycloidal-Actuated Manipulator. The firmware runs on an **ESP32-C6 Super Mini** microcontroller and reads the shaft angle from an **AS5600 12-bit Magnetic Rotary Encoder** over I2C, serving live telemetry via Wi-Fi.

---

## Files

| File | Purpose |
|------|---------|
| `Joint1_ESP32C6_AS5600.ino` | Main Arduino sketch — HTTP server, OTA, telemetry loop |
| `AS5600_Driver.h` | Low-level I2C driver for the AS5600 encoder |
| `adrc.h` | Active Disturbance Rejection Control (ADRC) implementation |
| `pid.h` | PID controller (reference / fallback) |
| `pinout.h` | GPIO pin definitions |
| `web_dashboard.h` | Self-contained dark-mode web dashboard (served from flash) |
| `platformio.ini` | PlatformIO build configuration for ESP32-C6 |

---

## Hardware Wiring

```
       +-----------------------+              +------------------------+
       |   AS5600 Breakout     |              |  ESP32-C6 Super Mini   |
       |                       |              |                        |
       |  [ VCC / 3V3 ] -------+------------>| [ 3V3 ] (3.3V Out)     |
       |  [ GND ] -------------+------------>| [ GND ] (Ground)       |
       |  [ SDA ] -------------+------------>| [ GPIO 6 ] (I2C SDA)   |
       |  [ SCL ] -------------+------------>| [ GPIO 7 ] (I2C SCL)   |
       |  [ DIR ] -------------+------------>| [ GND ] (CW = Angle +) |
       |  [ GPO / OUT ]        | (Unconnected)|                        |
       +-----------------------+              +------------------------+
```

### Pin Mapping

| AS5600 Pin | ESP32-C6 Pin | Function | Notes |
|:-----------|:-------------|:---------|:------|
| **VCC** | **3V3** | Power | **Do NOT connect to 5V!** |
| **GND** | **GND** | Ground | Common system ground |
| **SDA** | **GPIO 6** | I2C Data | Set in `pinout.h` |
| **SCL** | **GPIO 7** | I2C Clock | Set in `pinout.h` |
| **DIR** | **GND** | Rotation direction | GND = Clockwise increases angle |
| **GPO / OUT** | *Not connected* | Analog / PWM | Not used in I2C mode |

---

## Magnet & Mounting Tips

1. **Magnet type:** Use a **diametrically magnetized** disc magnet (north/south split across the diameter, not top-to-bottom).
2. **Air gap:** Position the magnet **0.5 mm to 1.5 mm** directly above the center of the AS5600 IC.
3. **Alignment:** Center the motor shaft with the AS5600 chip for maximum linearity (12-bit = 0.087° per step).

---

## How to Flash

### Option A: Arduino IDE

1. Add board URL in `Settings → Additional Board Manager URLs`:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
2. Install **esp32** board package (v3.0.x or later) via `Tools → Board Manager`.
3. Select:
   - Board: **ESP32C6 Dev Module**
   - USB CDC On Boot: **Enabled**
   - Flash Size: **4MB**
   - Port: Your ESP32-C6 COM port
4. Open `Joint1_ESP32C6_AS5600.ino` and click **Upload**.

### Option B: PlatformIO (VS Code)

1. Open the `Firmware/Joint1_ESP32C6_AS5600/` folder in VS Code with the PlatformIO extension installed.
2. Connect your ESP32-C6 via USB-C.
3. Click the **Upload** (→) button in the bottom status bar.

---

## Serial Monitor Output

Open Serial Monitor at **115200 baud**. After boot and Wi-Fi connection you will see:

```
===========================================
   Joint 1 - ESP32-C6 + AS5600 Encoder
===========================================
Initializing AS5600 I2C (SDA: GPIO 6, SCL: GPIO 7)...
AS5600 encoder initialized successfully.
Magnet Status: Optimal Magnet Alignment
Connecting to WiFi: 1234
.....connected
IP Address: 192.168.1.50
HTTP API URL: http://192.168.1.50/angle
[OTA] Service initialized. Hostname: Joint1-ESP32C6
HTTP server started.
[Joint 1] Current Angle:  142.35 deg | JSON: {"joint1": 142.35}
```

---

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Live dark-mode web dashboard |
| `/angle` | GET | Fast angle read: `{"joint1": 142.35}` |
| `/api/joint1` | GET | Alias for `/angle` |
| `/status` | GET | Full diagnostics (angle, AGC, magnet health, RSSI) |
| `/api/zero` | POST | Reset current position to 0.00° |

### Example `/status` response

```json
{
  "joint1": 142.35,
  "cumulative": 502.35,
  "raw": 1619,
  "agc": 128,
  "magnet_detected": true,
  "magnet_weak": false,
  "magnet_strong": false,
  "rssi": -54,
  "status": "Optimal Magnet Alignment"
}
```

---

## Over-The-Air (OTA) Updates

After the first USB flash, all future updates can be done wirelessly:

- **Arduino IDE:** `Tools → Port → Joint1-ESP32C6 at 192.168.x.x` (under Network Ports).
- **PlatformIO:** Uncomment `upload_protocol = espota` and set `upload_port` in `platformio.ini`.
