#!/usr/bin/env python3
"""
MPU6050 Sensor Data Parser for Raspberry Pi
Reads sensor data from STM32 via serial and parses the CSV format
Format: $AX,AY,AZ,TEMP,GX,GY,GZ\r\n
"""

import sys
import time
from typing import Optional, Tuple

import serial
from serial import Serial, SerialException

# Configuration
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600
TIMEOUT = 2

class MPU6050Parser:
    """Parse MPU6050 sensor data from serial port"""
    
    def __init__(self, port: str = SERIAL_PORT, baudrate: int = BAUD_RATE, timeout: int = TIMEOUT):
        """Initialize serial connection"""
        try:
            self.ser = serial.Serial(port, baudrate, timeout=timeout)
            print(f"✓ Connected to {port} at {baudrate} baud")
        except serial.SerialException as e:
            print(f"✗ Failed to open serial port: {e}")
            sys.exit(1)
    
    def parse_frame(self, line: str) -> Optional[Tuple[float, float, float, float, float, float, float]]:
        """
        Parse a sensor data frame
        
        Args:
            line: Raw data line from serial port
            
        Returns:
            Tuple of (ax, ay, az, temp, gx, gy, gz) or None if parsing failed
        """
        try:
            if not line.startswith('$'):
                return None
            
            # Remove prefix and split by comma
            values = line[1:].split(',')
            
            if len(values) != 7:
                print(f"⚠ Invalid frame length: expected 7 values, got {len(values)}")
                return None
            
            # Convert to float
            ax, ay, az, temp, gx, gy, gz = [float(v) for v in values]
            return (ax, ay, az, temp, gx, gy, gz)
            
        except ValueError as e:
            print(f"⚠ Failed to parse values: {e}")
            return None
        except Exception as e:
            print(f"⚠ Unexpected error parsing frame: {e}")
            return None
    
    def read_data(self, count: Optional[int] = None):
        """
        Read sensor data continuously
        
        Args:
            count: Number of frames to read (None for infinite)
        """
        frame_count = 0
        
        try:
            print("\n" + "="*70)
            print(f"{'Frame':<8} {'Ax':<8} {'Ay':<8} {'Az':<8} {'Temp':<8} {'Gx':<8} {'Gy':<8} {'Gz':<8}")
            print("="*70)
            
            while count is None or frame_count < count:
                try:
                    line = self.ser.readline().decode('utf-8').strip()
                    
                    if not line:
                        continue
                    
                    data = self.parse_frame(line)
                    
                    if data is not None:
                        ax, ay, az, temp, gx, gy, gz = data
                        frame_count += 1
                        
                        # Display formatted output
                        print(f"{frame_count:<8} {ax:>7.2f} {ay:>7.2f} {az:>7.2f} {temp:>7.2f} {gx:>7.2f} {gy:>7.2f} {gz:>7.2f}")
                        
                except UnicodeDecodeError:
                    print("⚠ Unicode decode error, skipping frame")
                    continue
                    
        except KeyboardInterrupt:
            print(f"\n\n✓ Received {frame_count} frames before exit")
        except Exception as e:
            print(f"\n✗ Error during data read: {e}")
        finally:
            self.close()
    
    def close(self):
        """Close serial connection"""
        if self.ser.is_open:
            self.ser.close()
            print("✓ Serial connection closed")

def main():
    """Main entry point"""
    import argparse
    
    parser = argparse.ArgumentParser(description='MPU6050 Sensor Data Parser')
    parser.add_argument('-p', '--port', default=SERIAL_PORT, 
                        help=f'Serial port (default: {SERIAL_PORT})')
    parser.add_argument('-b', '--baud', type=int, default=BAUD_RATE,
                        help=f'Baud rate (default: {BAUD_RATE})')
    parser.add_argument('-t', '--timeout', type=int, default=TIMEOUT,
                        help=f'Serial timeout in seconds (default: {TIMEOUT})')
    parser.add_argument('-c', '--count', type=int,
                        help='Number of frames to read (default: infinite)')
    
    args = parser.parse_args()
    
    # Create parser and read data
    mpu_parser = MPU6050Parser(port=args.port, baudrate=args.baud, timeout=args.timeout)
    mpu_parser.read_data(count=args.count)

if __name__ == '__main__':
    main()
