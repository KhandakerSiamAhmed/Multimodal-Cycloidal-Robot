"""
Mock ESP32 Serial Interface — Isolated Testing Tool

Simulates the ESP32-S3 Master node over a serial port.
Used by Track C (Software) to test Vision Pipeline, Voice Integration,
and Serial Bridge WITHOUT requiring any physical ESP32 hardware.

Setup Options:
  1. Virtual Serial Port (Windows): Install com0com, create a pair (e.g., COM10 <-> COM11).
     Run this script on COM10, connect your Python app to COM11.
  2. Virtual Serial Port (Linux): Use socat:
     socat -d -d pty,raw,echo=0 pty,raw,echo=0
  3. TCP Mode: Use --tcp flag to listen on localhost:8080 instead.

Usage:
  python mock_esp32_serial.py --port COM10
  python mock_esp32_serial.py --tcp --host 127.0.0.1 --tcpport 8080
"""

import serial
import json
import time
import argparse
import socket
import math
import sys

# Simulated arm state
arm_state = {
    "joints": [0.0, 0.0, 0.0],
    "mode": "IDLE",
    "errors": [],
    "gripper": "OPEN"
}

# Simulated DH parameters (approximate — update with real values)
L1 = 100.0  # mm — base to shoulder
L2 = 120.0  # mm — shoulder to elbow
L3 = 80.0   # mm — elbow to end effector


def mock_ik(x: float, y: float, z: float) -> list[float] | None:
    """
    Simplified 3-DOF IK mock.
    Returns [theta1, theta2, theta3] in degrees or None if unreachable.
    """
    theta1 = math.degrees(math.atan2(y, x))
    r = math.sqrt(x**2 + y**2)
    s = z - L1
    d = math.sqrt(r**2 + s**2)
    
    if d > (L2 + L3) or d < abs(L2 - L3):
        return None  # Unreachable
    
    cos_theta3 = (d**2 - L2**2 - L3**2) / (2 * L2 * L3)
    cos_theta3 = max(-1.0, min(1.0, cos_theta3))
    theta3 = math.degrees(math.acos(cos_theta3))
    
    alpha = math.atan2(s, r)
    beta = math.atan2(L3 * math.sin(math.radians(theta3)), 
                       L2 + L3 * math.cos(math.radians(theta3)))
    theta2 = math.degrees(alpha + beta)
    
    return [round(theta1, 2), round(theta2, 2), round(theta3, 2)]


def process_command(cmd_str: str) -> str:
    """Process a JSON command and return a JSON response."""
    try:
        cmd = json.loads(cmd_str.strip())
    except json.JSONDecodeError:
        return json.dumps({"status": "ERROR", "message": "INVALID_JSON", "seq": 0})
    
    seq = cmd.get("seq", 0)
    command = cmd.get("cmd", "")
    
    if command == "MOVE_XYZ":
        x, y, z = cmd.get("x", 0), cmd.get("y", 0), cmd.get("z", 0)
        joints = mock_ik(x, y, z)
        if joints is None:
            return json.dumps({"status": "ERROR", "message": "IK_NO_SOLUTION", "seq": seq})
        arm_state["joints"] = joints
        return json.dumps({"status": "OK", "joints": joints, "mode": arm_state["mode"], 
                          "errors": [], "seq": seq})
    
    elif command == "MOVE_JOINTS":
        joints = cmd.get("joints", [0, 0, 0])
        arm_state["joints"] = joints
        # Simulate execution delay
        time.sleep(0.05)
        return json.dumps({"status": "OK", "joints": joints, "mode": arm_state["mode"],
                          "errors": [], "seq": seq})
    
    elif command == "GRIPPER":
        action = cmd.get("action", "OPEN")
        arm_state["gripper"] = action
        return json.dumps({"status": "OK", "joints": arm_state["joints"], 
                          "mode": arm_state["mode"], "errors": [], "seq": seq})
    
    elif command == "SET_MODE":
        mode = cmd.get("mode", "IDLE")
        valid_modes = ["IDLE", "VISION_SORT", "VOICE_SORT", "TELEOP", "TEACH_MODE", "REPEAT_MODE"]
        if mode not in valid_modes:
            return json.dumps({"status": "ERROR", "message": "INVALID_MODE", "seq": seq})
        arm_state["mode"] = mode
        return json.dumps({"status": "OK", "joints": arm_state["joints"],
                          "mode": mode, "errors": [], "seq": seq})
    
    elif command == "QUERY_STATE":
        return json.dumps({"status": "OK", "joints": arm_state["joints"],
                          "mode": arm_state["mode"], "gripper": arm_state["gripper"],
                          "errors": arm_state["errors"], "seq": seq})
    
    elif command == "ESTOP":
        arm_state["mode"] = "IDLE"
        return json.dumps({"status": "OK", "joints": arm_state["joints"],
                          "mode": "IDLE", "errors": [], "seq": seq})
    
    else:
        return json.dumps({"status": "ERROR", "message": "INVALID_CMD", "seq": seq})


def run_serial_mode(port: str, baud: int = 115200):
    """Run mock ESP32 over serial port."""
    print(f"[MOCK ESP32] Listening on serial port {port} at {baud} baud...")
    ser = serial.Serial(port, baud, timeout=1)
    
    while True:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                print(f"[RX] {line}")
                response = process_command(line)
                ser.write((response + '\n').encode())
                print(f"[TX] {response}")
        except KeyboardInterrupt:
            print("\n[MOCK ESP32] Shutting down.")
            ser.close()
            break
        except Exception as e:
            print(f"[ERROR] {e}")


def run_tcp_mode(host: str = "127.0.0.1", port: int = 8080):
    """Run mock ESP32 over TCP socket."""
    print(f"[MOCK ESP32] Listening on TCP {host}:{port}...")
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind((host, port))
    server.listen(1)
    
    while True:
        try:
            conn, addr = server.accept()
            print(f"[MOCK ESP32] Client connected from {addr}")
            buffer = ""
            
            while True:
                data = conn.recv(1024).decode('utf-8', errors='ignore')
                if not data:
                    break
                buffer += data
                
                while '\n' in buffer:
                    line, buffer = buffer.split('\n', 1)
                    line = line.strip()
                    if line:
                        print(f"[RX] {line}")
                        response = process_command(line)
                        conn.sendall((response + '\n').encode())
                        print(f"[TX] {response}")
            
            conn.close()
            print("[MOCK ESP32] Client disconnected.")
        except KeyboardInterrupt:
            print("\n[MOCK ESP32] Shutting down.")
            server.close()
            break


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Mock ESP32-S3 Master Node")
    parser.add_argument("--port", default="COM10", help="Serial port (default: COM10)")
    parser.add_argument("--baud", type=int, default=115200, help="Baud rate (default: 115200)")
    parser.add_argument("--tcp", action="store_true", help="Use TCP instead of serial")
    parser.add_argument("--host", default="127.0.0.1", help="TCP host (default: 127.0.0.1)")
    parser.add_argument("--tcpport", type=int, default=8080, help="TCP port (default: 8080)")
    
    args = parser.parse_args()
    
    if args.tcp:
        run_tcp_mode(args.host, args.tcpport)
    else:
        run_serial_mode(args.port, args.baud)
