# AS5600 Magnetic Rotary Encoder - Technical Documentation

The **AS5600** is a high-resolution, 12-bit contactless magnetic rotary position sensor (Hall-effect encoder IC) designed by ams OSRAM. It measures the absolute angle of a diametrically magnetized on-axis magnet over a full 360° rotation.

---

## 1. Key Specifications

| Feature | Specification |
| :--- | :--- |
| **Resolution** | 12-bit (4096 positions per 360° rotation) |
| **Angular Precision** | ~0.08789° per LSB ($360^\circ / 4096$) |
| **Communication Interface** | I2C (Standard Mode 100 kHz, Fast Mode 400 kHz) |
| **I2C Slave Address** | `0x36` (Fixed 7-bit address) |
| **Output Modes** | • I2C digital read (recommended)<br>• Analog ratiometric voltage ($0 \text{ V} \rightarrow \text{VCC}$)<br>• PWM output (115 Hz to 920 Hz) |
| **Operating Voltage** | **3.3V** or **5.0V** (Selectable on breakout board) |
| **Supply Current** | ~6.5 mA (Active mode), < 1.5 mA (Low-power mode) |
| **Non-Linearity Error** | < ±1° (assuming proper magnet alignment and air gap) |

---

## 2. Pinout & Wiring Diagram

```text
       +-----------------------+              +------------------------+
       |    AS5600 Breakout    |              |  ESP32-C6 Super Mini   |
       |                       |              |                        |
       |  [ VCC / 3V3 ] -------+------------->| [ 3V3 ] (3.3V Power)   |
       |  [ GND ] -------------+------------->| [ GND ] (Ground)       |
       |  [ SDA ] -------------+------------->| [ GPIO 6 ] (I2C SDA)   |
       |  [ SCL ] -------------+------------->| [ GPIO 7 ] (I2C SCL)   |
       |  [ DIR ] -------------+------------->| [ GND ] (CW = Angle +) |
       |  [ GPO / OUT ]        | (Unconnected)|                        |
       +-----------------------+              +------------------------+
```

### Pin Description

| Pin | Description | Connection in Project |
| :--- | :--- | :--- |
| **VCC / 3V3** | Power supply input (3.3V) | Connect to ESP32-C6 **3V3** rail |
| **GND** | Ground reference | Connect to ESP32-C6 **GND** |
| **SDA** | I2C Serial Data (Bidirectional) | Connect to ESP32-C6 **GPIO 6** |
| **SCL** | I2C Serial Clock | Connect to ESP32-C6 **GPIO 7** |
| **DIR** | Rotation Direction configuration | Connect to **GND** (Clockwise increases angle)<br>Connect to **3V3** (CCW increases angle) |
| **GPO / OUT** | Analog Output / PWM Output | Leave disconnected when using I2C |

> [!IMPORTANT]
> When interfacing with a 3.3V microcontroller (ESP32-C6), power the AS5600 module from the **3.3V** pin to prevent signal level mismatches on the I2C bus.

---

## 3. Magnet Requirements & Mechanical Alignment

```text
         [ Diametrically Magnetized Magnet ]
                  +-------+-------+
                  |   N   |   S   |  <- Diametrical (Pole split across diameter)
                  +-------+-------+
                         |
                   Air Gap (0.5mm - 1.5mm)
                         |
                  +---------------+
                  |  AS5600 IC    |  <- Centered directly on motor shaft axis
                  +---------------+
```

1. **Magnet Type**: Must be a **diametrically magnetized** cylinder or disc magnet (typically Ø6 mm × 2.5 mm, NdFeB N35/N52). Standard axial magnets (poles on top/bottom faces) will **NOT** work.
2. **Air Gap**: Maintain a vertical clearance of **0.5 mm to 1.5 mm** between the chip package surface and the magnet face.
3. **Axial Alignment**: The magnet must be centered over the AS5600 IC within ±0.25 mm to avoid angular distortion.

---

## 4. Key I2C Registers

| Register Address | Register Name | Bits | Description |
| :--- | :--- | :--- | :--- |
| `0x0B` | `STATUS` | `[5:3]` | Magnet status flags:<br>• Bit 5 (`MD`): Magnet Detected (1 = OK)<br>• Bit 4 (`ML`): Magnet too weak / too far (1 = Low)<br>• Bit 3 (`MH`): Magnet too strong / too close (1 = High) |
| `0x0C` | `RAW ANGLE (Hi)` | `[11:8]` | Unmodified raw angle high byte |
| `0x0D` | `RAW ANGLE (Lo)` | `[7:0]` | Unmodified raw angle low byte |
| `0x0E` | `ANGLE (Hi)` | `[11:8]` | Filtered angle high byte |
| `0x0F` | `ANGLE (Lo)` | `[7:0]` | Filtered angle low byte |
| `0x1A` | `AGC` | `[7:0]` | Automatic Gain Control ($0 \rightarrow 255$). Ideal range: $80 \rightarrow 170$ (nominal ~128) |
| `0x1B` - `0x1C` | `MAGNITUDE` | `[11:0]` | Internal CORDIC vector magnitude of magnetic field |

---

## 5. Reading Angle via Software (Formula)

To calculate the angle in degrees from the raw 12-bit register reading:

$$\text{Raw Value} = (\text{Reg}[0x0C] \ll 8) \mid \text{Reg}[0x0D]$$

$$\text{Angle (Degrees)} = \text{Raw Value} \times \frac{360.0^\circ}{4096} = \text{Raw Value} \times 0.087890625^\circ$$

---

## 6. Diagnostic & Troubleshooting Checklist

- **Angle stuck at 0° or 4095°**: Check `STATUS` register (`0x0B`). If `MD == 0`, the magnet is missing or not diametrically polarized.
- **`ML` bit set (Magnet too Low)**: Move the magnet closer to the chip (reduce air gap).
- **`MH` bit set (Magnet too High)**: Increase the air gap between magnet and sensor.
- **I2C communication failure (`0x36` not found)**: Ensure pull-up resistors (typically 4.7kΩ) are present on SDA and SCL, and verify DIR pin is tied to GND or VCC (do not leave floating).
