"""
read_joint1.py
A simple Python script to poll and display real-time Joint 1 angle from ESP32-C6.
Usage:
    python read_joint1.py <ESP32_IP_OR_HOSTNAME>
Example:
    python read_joint1.py 192.168.1.50
    python read_joint1.py joint1.local
"""

import sys
import time
import urllib.request
import json

def stream_joint1(host="joint1.local"):
    url = f"http://{host}/angle"
    print(f"Connecting to Joint 1 telemetry feed at {url}...")
    print("Press Ctrl+C to exit.\n")

    while True:
        try:
            with urllib.request.urlopen(url, timeout=2.0) as response:
                if response.status == 200:
                    data = json.loads(response.read().decode())
                    angle = data.get("joint1", 0.0)
                    print(f"\r[Joint 1 Telemetry] Angle: {angle:6.2f}° | Raw JSON: {json.dumps(data)}", end="", flush=True)
        except Exception as e:
            print(f"\r[Joint 1] Waiting for ESP32 ({e})...", end="", flush=True)
        time.sleep(0.05) # 20 Hz polling

if __name__ == "__main__":
    host_arg = sys.argv[1] if len(sys.argv) > 1 else "joint1.local"
    try:
        stream_joint1(host_arg)
    except KeyboardInterrupt:
        print("\nExiting stream.")
