#!/usr/bin/env python3
"""
Mock test for MPU6050 parser - simulates sensor data for testing
"""

import sys
import time

# Mock data samples
MOCK_DATA = [
    "$0.00,0.00,1.00,36.53,0.00,0.00,0.00",
    "$0.05,0.10,0.95,36.54,0.10,-0.05,0.15",
    "$0.10,0.15,0.90,36.55,0.20,-0.10,0.30",
    "$0.15,0.20,0.85,36.56,0.30,-0.15,0.45",
    "$0.20,0.25,0.80,36.57,0.40,-0.20,0.60",
    "$-0.05,-0.10,1.05,36.52,-0.10,0.05,-0.15",
    "$-0.10,-0.15,1.10,36.51,-0.20,0.10,-0.30",
    "$-0.15,-0.20,1.15,36.50,-0.30,0.15,-0.45",
    "$0.00,0.00,1.00,36.53,0.00,0.00,0.00",
    "$0.05,0.10,0.95,36.54,0.10,-0.05,0.15",
]

def main():
    """Simulate sensor data output"""
    print("Mock MPU6050 Sensor Data Generator")
    print("=" * 70)
    print("Sending mock data frames (press Ctrl+C to stop)...\n")
    
    frame_count = 0
    try:
        while True:
            for data in MOCK_DATA:
                frame_count += 1
                print(data)
                sys.stdout.flush()
                time.sleep(0.05)  # 50ms like the firmware
    except KeyboardInterrupt:
        print(f"\n\n✓ Sent {frame_count} frames")

if __name__ == '__main__':
    main()
