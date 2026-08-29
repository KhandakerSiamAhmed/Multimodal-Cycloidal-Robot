# ESP32-C6 Super Mini - Technical Documentation

The **ESP32-C6 Super Mini** is an ultra-compact, high-performance IoT development board powered by Espressif's ESP32-C6 SoC. It integrates a single-core 32-bit RISC-V processor with Wi-Fi 6 (802.11ax), Bluetooth 5 (LE), and IEEE 802.15.4 (Zigbee 3.0 / Thread).

---

## 1. Key Specifications

| Feature | Specification |
| :--- | :--- |
| **Processor** | Espressif ESP32-C6FH4 (32-bit RISC-V Single-Core up to 160 MHz) |
| **SRAM** | 512 KB HP SRAM, 16 KB LP SRAM, 320 KB ROM |
| **Flash Memory** | 4 MB Quad-SPI on-chip Flash |
| **Wireless Connectivity** | • 2.4 GHz Wi-Fi 6 (802.11 b/g/n/ax, 20/40 MHz bandwidth)<br>• Bluetooth 5 (LE) & Bluetooth Mesh<br>• IEEE 802.15.4 (Zigbee 3.0 & Thread 1.3) |
| **USB Interface** | USB Type-C (Native USB Serial/JTAG CDC controller) |
| **Operating Voltage** | 3.3V Logic Level (IO pins are **NOT 5V tolerant**) |
| **Input Supply Voltage** | • **USB-C**: 5V DC<br>• **5V Pin**: 4.5V - 6.0V DC (onboard LDO regulator)<br>• **3V3 Pin**: Regulated 3.3V DC input/output |
| **Antenna** | Ceramic chip antenna onboard (or external IPEX connector depending on variant) |
| **Form Factor** | Compact DIP footprint (~22.5 mm × 18 mm), breadboard-compatible |

---

## 2. Pinout & GPIO Reference

```text
                     +-------------------+
                     |   [ USB TYPE-C ]  |
                     +-------------------+
             (GPIO 0) | [0]           [5V] | 5V Power In / VBUS
             (GPIO 1) | [1]          [GND] | Ground
             (GPIO 2) | [2]          [3V3] | 3.3V Power Out
             (GPIO 3) | [3]           [23] | GPIO 23 / ADC
             (GPIO 4) | [4]           [22] | GPIO 22 / ADC
             (GPIO 5) | [5]           [21] | GPIO 21 / UART TX
(I2C SDA)    (GPIO 6) | [6]           [20] | GPIO 20 / UART RX
(I2C SCL)    (GPIO 7) | [7]           [15] | GPIO 15 / Strapping
(Status LED) (GPIO 8) | [8]            [9] | GPIO 9 / Boot Button
                     +-------------------+
                        [RST]     [BOOT]
```

### GPIO Functionality Table

| Pin Name | Primary Function | Alternate Peripherals | Notes |
| :--- | :--- | :--- | :--- |
| **GPIO 0** | Digital I/O | ADC1_CH0, RTC | General purpose I/O |
| **GPIO 1** | Digital I/O | ADC1_CH1, RTC | General purpose I/O |
| **GPIO 2** | Digital I/O | ADC1_CH2, RTC | General purpose I/O |
| **GPIO 3** | Digital I/O | ADC1_CH3, RTC | General purpose I/O |
| **GPIO 4** | Digital I/O | ADC1_CH4, RTC | General purpose I/O |
| **GPIO 5** | Digital I/O | ADC1_CH5, RTC | General purpose I/O |
| **GPIO 6** | Digital I/O | **I2C SDA** (Project Default) | General purpose I/O |
| **GPIO 7** | Digital I/O | **I2C SCL** (Project Default) | General purpose I/O |
| **GPIO 8** | Digital I/O | Onboard Blue LED / Strapping | Active Low or High depending on batch |
| **GPIO 9** | Boot Pin | Onboard BOOT Button | Pulled HIGH internally; LOW on boot enters ROM download mode |
| **GPIO 15** | Strapping | Strapping pin | Ensure no external pull-downs interfere during power-up |
| **GPIO 20** | UART RX / I/O | U0RXD | Default hardware Serial RX |
| **GPIO 21** | UART TX / I/O | U0TXD | Default hardware Serial TX |
| **GPIO 22** | Digital I/O | ADC1_CH6 | General purpose I/O |
| **GPIO 23** | Digital I/O | ADC1_CH7 | General purpose I/O |

---

## 3. Power Supply Guidelines

1. **Power via USB-C**: Powering through the USB-C connector routes 5V into the onboard low-dropout (LDO) regulator, providing a clean 3.3V rail.
2. **Power via External Supply**:
   - Connect **5V DC** to the `5V` pin (recommended when powering from an external step-down buck converter).
   - Alternatively, connect **3.3V DC regulated** directly to the `3V3` pin.
3. **Logic Warning**:
   > [!WARNING]
   > All GPIO pins operate strictly at **3.3V logic levels**. Applying 5V directly to any GPIO pin will permanently damage the MCU.

---

## 4. Flashing & Development Configuration

### PlatformIO Configuration (`platformio.ini`)

```ini
[env:esp32-c6-devkitm-1]
platform = espressif32
board = esp32-c6-devkitm-1
framework = arduino
monitor_speed = 115200
upload_speed = 921600
board_build.flash_mode = qio

build_flags = 
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
```

### Arduino IDE Settings

- **Board**: `ESP32C6 Dev Module`
- **Flash Size**: `4MB (32Mb)`
- **CPU Frequency**: `160MHz`
- **USB CDC On Boot**: `Enabled` *(Crucial for Serial.print() over USB-C)*
- **Upload Speed**: `921600` (or `115200` for recovery)

---

## 5. Boot / Recovery Mode

If the board fails to enter flashing mode automatically:
1. Press and hold the **BOOT** button (GPIO 9).
2. Press and release the **RST** button (or re-plug USB-C).
3. Release the **BOOT** button.
4. Flash the code via PlatformIO or Arduino IDE.
