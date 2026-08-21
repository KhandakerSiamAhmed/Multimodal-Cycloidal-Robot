# ESP32-C6 Super Mini + AS5600 Encoder Pinout & Setup Guide

This guide provides the complete hardware wiring, pinout specifications, and software instructions for reading Joint 1 (NEMA 17 Stepper Motor shaft) using the **AS5600 12-Bit Magnetic Rotary Encoder** and **ESP32-C6 Super Mini**.

---

## 1. Hardware Pinout & Wiring Diagram

```
       +-----------------------+              +------------------------+
       |   AS5600 Breakout     |              |  ESP32-C6 Super Mini   |
       |                       |              |                        |
       |  [ VCC / 3V3 ] -------+------------->| [ 3V3 ] (3.3V Out)     |
       |  [ GND ] -------------+------------->| [ GND ] (Ground)       |
       |  [ SDA ] -------------+------------->| [ GPIO 6 ] (I2C SDA)   |
       |  [ SCL ] -------------+------------->| [ GPIO 7 ] (I2C SCL)   |
       |  [ DIR ] -------------+------------->| [ GND ] (CW = Angle +) |
       |  [ GPO / OUT ]        | (Unconnected)|                        |
       +-----------------------+              +------------------------+
```

### Complete Pin Mapping Table

| AS5600 Pin | ESP32-C6 Super Mini Pin | Wire Function | Notes |
| :--- | :--- | :--- | :--- |
| **VCC** (or 3V3) | **3V3** | 3.3V Power | **Do NOT connect to 5V!** Connect to ESP32 3V3 pin. |
| **GND** | **GND** | Ground | Common system ground |
| **SDA** | **GPIO 6** | I2C Data | Configured in `pinout.h` (`I2C_SDA_PIN`) |
| **SCL** | **GPIO 7** | I2C Clock | Configured in `pinout.h` (`I2C_SCL_PIN`) |
| **DIR** | **GND** | Rotation Direction | `GND` = Clockwise angle increases (0° $\rightarrow$ 360°)<br>`3V3` = Counter-clockwise increases |
| **GPO / OUT** | *NC (No Connection)* | Analog / PWM | Not needed for I2C communication |

---

## 2. Magnet & 3D Printed Mount Best Practices

1. **Magnet Type**: The AS5600 requires a **diametrically magnetized** magnet (split north/south across the circle diameter, not top-to-bottom axial).
2. **Air Gap**: Position the magnet **0.5 mm to 1.5 mm** directly above the center of the AS5600 IC.
3. **Alignment**: Ensure the center of the motor shaft is centered with the AS5600 chip for best linearity (12-bit precision = 0.087° per step).

---

## 3. How to Flash the Firmware

### Option A: Using Arduino IDE
1. Install **ESP32 Board Package** in Arduino IDE:
   - Go to `Settings` $\rightarrow$ Additional Board Manager URLs $\rightarrow$ Add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
   - Open `Tools` $\rightarrow$ `Board Manager`, search for `esp32` and install version **3.0.x** (or latest).
2. Select Board:
   - Board: **ESP32C6 Dev Module**
   - USB CDC On Boot: **Enabled**
   - Flash Size: **4MB**
   - Port: Select your ESP32-C6 COM port.
3. Open `Joint1_ESP32C6_AS5600/Joint1_ESP32C6_AS5600.ino`.
4. Click **Upload**.

### Option B: Using PlatformIO (VS Code)
1. Open the project root folder in VS Code with PlatformIO installed.
2. Connect your ESP32-C6 via USB-C.
3. Click the **PlatformIO: Upload** arrow icon in the bottom status bar.

---

## 4. Testing & Operation

### 1. Serial Monitor Output
Open the Serial Monitor at **115200 Baud**. Upon boot and connecting to WiFi (`SSID: 1233`), you will see:

```text
===========================================
   Joint 1 - ESP32-C6 + AS5600 Encoder     
===========================================
Initializing AS5600 I2C (SDA: GPIO 6, SCL: GPIO 7)...
AS5600 encoder initialized successfully.
Magnet Status: Optimal Magnet Alignment
Connecting to WiFi: 1234
.....
connected
IP Address: 192.168.1.50
HTTP API URL: http://192.168.1.50/angle
[OTA] Service initialized. Hostname: Joint1-ESP32C6
HTTP server started.
[Joint 1] Current Angle:  142.35 deg | JSON: {"joint1": 142.35}
[Joint 1] Current Angle:  145.10 deg | JSON: {"joint1": 145.10}
```

### 2. Live Web Dashboard (Browser)
Open your browser and navigate to:
```text
http://<ESP32_IP_ADDRESS>/
```
or (if mDNS supported on your PC/Phone):
```text
http://joint1.local/
```
You will see a real-time dark-mode gauge, angle readout, raw 12-bit ticks, and magnet health indicators.

### 3. Direct JSON Endpoint
Any application (Python, ROS, Node.js, Digital Twin Web app) can fetch the current angle via:
```bash
curl http://<ESP32_IP_ADDRESS>/angle
```
**Response:**
```json
{"joint1": 142.35}
```

---

## 5. Over-The-Air (OTA) Updates
Once flashed over USB the first time, you can update code wirelessly:
- **Arduino IDE**: Select `Tools` $\rightarrow$ `Port` $\rightarrow$ `Joint1-ESP32C6 at 192.168.x.x` (under Network Ports).
- **PlatformIO**: Add `upload_protocol = espota` and `upload_port = <ESP32_IP>` in `platformio.ini` and run upload.
