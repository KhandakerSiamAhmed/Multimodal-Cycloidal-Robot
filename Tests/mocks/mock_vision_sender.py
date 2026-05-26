"""
Mock Vision Data Sender — Isolated Testing Tool

Sends fake vision pipeline coordinate data to the ESP32-S3 Master
(or to the mock_esp32_serial.py) over Serial or TCP.

Used by Track B (Firmware) to test IK solver, S-Curve profiler,
and multi-joint coordination WITHOUT requiring a real camera or OpenCV.

Usage:
  python mock_vision_sender.py --port COM3
  python mock_vision_sender.py --tcp --host 127.0.0.1 --tcpport 8080
"""

import json
import time
import argparse
import serial
import socket

# Pre-defined pick-and-place sequence simulating the Vision Pipeline output
PICK_AND_PLACE_SEQUENCE = [
    # Phase 1: Move to hover above red object
    {"cmd": "SET_MODE", "mode": "VISION_SORT", "seq": 1},
    {"cmd": "MOVE_XYZ", "x": 100.0, "y": 50.0, "z": 80.0, "velocity": 30.0, "seq": 2},
    
    # Phase 2: Lower to object
    {"cmd": "MOVE_XYZ", "x": 100.0, "y": 50.0, "z": 15.0, "velocity": 10.0, "seq": 3},
    
    # Phase 3: Close gripper
    {"cmd": "GRIPPER", "action": "CLOSE", "seq": 4},
    
    # Phase 4: Lift object
    {"cmd": "MOVE_XYZ", "x": 100.0, "y": 50.0, "z": 80.0, "velocity": 20.0, "seq": 5},
    
    # Phase 5: Move to drop zone (red bucket)
    {"cmd": "MOVE_XYZ", "x": -80.0, "y": 100.0, "z": 80.0, "velocity": 30.0, "seq": 6},
    
    # Phase 6: Lower to drop height
    {"cmd": "MOVE_XYZ", "x": -80.0, "y": 100.0, "z": 40.0, "velocity": 15.0, "seq": 7},
    
    # Phase 7: Release
    {"cmd": "GRIPPER", "action": "OPEN", "seq": 8},
    
    # Phase 8: Return home
    {"cmd": "MOVE_XYZ", "x": 0.0, "y": 0.0, "z": 100.0, "velocity": 25.0, "seq": 9},
    {"cmd": "SET_MODE", "mode": "IDLE", "seq": 10},
]

# Joint-space test sequence (bypasses IK)
JOINT_TEST_SEQUENCE = [
    {"cmd": "MOVE_JOINTS", "joints": [0.0, 0.0, 0.0], "velocity": 20.0, "seq": 100},
    {"cmd": "MOVE_JOINTS", "joints": [45.0, 0.0, 0.0], "velocity": 20.0, "seq": 101},
    {"cmd": "MOVE_JOINTS", "joints": [45.0, 30.0, 0.0], "velocity": 20.0, "seq": 102},
    {"cmd": "MOVE_JOINTS", "joints": [45.0, 30.0, -45.0], "velocity": 20.0, "seq": 103},
    {"cmd": "MOVE_JOINTS", "joints": [-45.0, 60.0, -90.0], "velocity": 15.0, "seq": 104},
    {"cmd": "MOVE_JOINTS", "joints": [0.0, 0.0, 0.0], "velocity": 20.0, "seq": 105},
]


def send_sequence(send_fn, receive_fn, sequence: list, delay: float = 2.0):
    """Send a command sequence with delay between commands."""
    for i, cmd in enumerate(sequence):
        cmd_str = json.dumps(cmd)
        print(f"\n[TX {i+1}/{len(sequence)}] {cmd_str}")
        send_fn(cmd_str + '\n')
        
        # Wait for response
        response = receive_fn()
        if response:
            print(f"[RX] {response.strip()}")
            try:
                rsp = json.loads(response)
                if rsp.get("status") == "ERROR":
                    print(f"  ⚠ ERROR: {rsp.get('message', 'unknown')}")
                else:
                    joints = rsp.get("joints", [])
                    print(f"  ✓ Joints: {joints}")
            except json.JSONDecodeError:
                print(f"  ⚠ Invalid JSON response")
        
        time.sleep(delay)


def run_serial(port: str, baud: int, sequence: list, delay: float):
    """Send sequence over serial port."""
    print(f"[SENDER] Connecting to serial port {port} at {baud} baud...")
    ser = serial.Serial(port, baud, timeout=2)
    time.sleep(1)  # Wait for serial to initialize
    
    def send(msg):
        ser.write(msg.encode())
    
    def receive():
        return ser.readline().decode('utf-8', errors='ignore')
    
    try:
        send_sequence(send, receive, sequence, delay)
    finally:
        ser.close()
    print("\n[SENDER] Sequence complete.")


def run_tcp(host: str, port: int, sequence: list, delay: float):
    """Send sequence over TCP."""
    print(f"[SENDER] Connecting to TCP {host}:{port}...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    sock.settimeout(2)
    
    buffer = ""
    
    def send(msg):
        sock.sendall(msg.encode())
    
    def receive():
        nonlocal buffer
        try:
            data = sock.recv(1024).decode('utf-8', errors='ignore')
            buffer += data
            if '\n' in buffer:
                line, buffer = buffer.split('\n', 1)
                return line + '\n'
        except socket.timeout:
            pass
        return ""
    
    try:
        send_sequence(send, receive, sequence, delay)
    finally:
        sock.close()
    print("\n[SENDER] Sequence complete.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Mock Vision Data Sender")
    parser.add_argument("--port", default="COM3", help="Serial port (default: COM3)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate")
    parser.add_argument("--tcp", action="store_true", help="Use TCP instead of serial")
    parser.add_argument("--host", default="127.0.0.1", help="TCP host")
    parser.add_argument("--tcpport", type=int, default=8080, help="TCP port")
    parser.add_argument("--delay", type=float, default=2.0, help="Delay between commands (seconds)")
    parser.add_argument("--mode", choices=["pickplace", "joints"], default="pickplace",
                       help="Test sequence to run (default: pickplace)")
    
    args = parser.parse_args()
    sequence = PICK_AND_PLACE_SEQUENCE if args.mode == "pickplace" else JOINT_TEST_SEQUENCE
    
    if args.tcp:
        run_tcp(args.host, args.tcpport, sequence, args.delay)
    else:
        run_serial(args.port, args.baud, sequence, args.delay)
