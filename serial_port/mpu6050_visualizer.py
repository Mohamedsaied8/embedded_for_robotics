#!/usr/bin/env python3
"""
Real-time MPU6050 Sensor Data Visualization
Plots accelerometer, gyroscope, and temperature data in real-time
"""

import sys
import time
from typing import Optional, Tuple, List
from collections import deque
from datetime import datetime

import serial
from serial import Serial, SerialException

import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.figure import Figure
import numpy as np

# Configuration
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600
TIMEOUT = 2
BUFFER_SIZE = 500  # Number of samples to keep in memory

class MPU6050Visualizer:
    """Real-time visualization of MPU6050 sensor data"""
    
    def __init__(self, port: str = SERIAL_PORT, baudrate: int = BAUD_RATE, 
                 timeout: int = TIMEOUT, buffer_size: int = BUFFER_SIZE):
        """Initialize serial connection and data buffers"""
        try:
            self.ser = Serial(port, baudrate, timeout=timeout)
            print(f"✓ Connected to {port} at {baudrate} baud")
        except SerialException as e:
            print(f"✗ Failed to open serial port: {e}")
            sys.exit(1)
        
        self.buffer_size = buffer_size
        self.timestamps = deque(maxlen=buffer_size)
        
        # Accelerometer buffers (g)
        self.ax_data = deque(maxlen=buffer_size)
        self.ay_data = deque(maxlen=buffer_size)
        self.az_data = deque(maxlen=buffer_size)
        
        # Temperature buffer (°C)
        self.temp_data = deque(maxlen=buffer_size)
        
        # Gyroscope buffers (deg/s)
        self.gx_data = deque(maxlen=buffer_size)
        self.gy_data = deque(maxlen=buffer_size)
        self.gz_data = deque(maxlen=buffer_size)
        
        self.frame_count = 0
        self.start_time = time.time()
    
    def parse_frame(self, line: str) -> Optional[Tuple[float, ...]]:
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
            
            values = line[1:].split(',')
            if len(values) != 7:
                return None
            
            ax, ay, az, temp, gx, gy, gz = [float(v) for v in values]
            return (ax, ay, az, temp, gx, gy, gz)
            
        except (ValueError, IndexError):
            return None
    
    def read_frame(self) -> bool:
        """Read and parse one frame from serial"""
        try:
            line = self.ser.readline().decode('utf-8').strip()
            
            if not line:
                return False
            
            data = self.parse_frame(line)
            
            if data is not None:
                ax, ay, az, temp, gx, gy, gz = data
                
                # Add timestamp (relative to start)
                timestamp = time.time() - self.start_time
                self.timestamps.append(timestamp)
                
                # Add data to buffers
                self.ax_data.append(ax)
                self.ay_data.append(ay)
                self.az_data.append(az)
                self.temp_data.append(temp)
                self.gx_data.append(gx)
                self.gy_data.append(gy)
                self.gz_data.append(gz)
                
                self.frame_count += 1
                return True
            
            return False
            
        except (UnicodeDecodeError, SerialException):
            return False
    
    def get_data_arrays(self):
        """Get current data as numpy arrays for plotting"""
        timestamps = np.array(list(self.timestamps)) if self.timestamps else np.array([])
        
        return {
            'timestamps': timestamps,
            'ax': np.array(list(self.ax_data)),
            'ay': np.array(list(self.ay_data)),
            'az': np.array(list(self.az_data)),
            'temp': np.array(list(self.temp_data)),
            'gx': np.array(list(self.gx_data)),
            'gy': np.array(list(self.gy_data)),
            'gz': np.array(list(self.gz_data)),
        }
    
    def close(self):
        """Close serial connection"""
        if self.ser.is_open:
            self.ser.close()
            print("✓ Serial connection closed")


def create_visualization(visualizer: MPU6050Visualizer):
    """Create and display real-time plots"""
    
    # Create figure with 3 subplots (accelerometer, gyroscope, temperature)
    fig, axes = plt.subplots(3, 1, figsize=(14, 10))
    fig.suptitle('MPU6050 Real-Time Sensor Data', fontsize=16, fontweight='bold')
    
    # Accelerometer plot
    ax_accel = axes[0]
    ax_accel.set_ylabel('Acceleration (g)', fontsize=11, fontweight='bold')
    ax_accel.set_ylim(-2.5, 2.5)
    ax_accel.grid(True, alpha=0.3)
    ax_accel.legend(['Ax', 'Ay', 'Az'], loc='upper left', fontsize=10)
    lines_accel = [ax_accel.plot([], [], label=label, linewidth=2)[0] 
                   for label in ['Ax', 'Ay', 'Az']]
    
    # Gyroscope plot
    ax_gyro = axes[1]
    ax_gyro.set_ylabel('Angular Velocity (deg/s)', fontsize=11, fontweight='bold')
    ax_gyro.set_ylim(-300, 300)
    ax_gyro.grid(True, alpha=0.3)
    ax_gyro.legend(['Gx', 'Gy', 'Gz'], loc='upper left', fontsize=10)
    lines_gyro = [ax_gyro.plot([], [], label=label, linewidth=2)[0] 
                  for label in ['Gx', 'Gy', 'Gz']]
    
    # Temperature plot
    ax_temp = axes[2]
    ax_temp.set_xlabel('Time (seconds)', fontsize=11, fontweight='bold')
    ax_temp.set_ylabel('Temperature (°C)', fontsize=11, fontweight='bold')
    ax_temp.set_ylim(20, 50)
    ax_temp.grid(True, alpha=0.3)
    ax_temp.legend(['Temp'], loc='upper left', fontsize=10)
    lines_temp = ax_temp.plot([], [], 'r-', label='Temp', linewidth=2)
    
    # Status text
    status_text = fig.text(0.5, 0.02, '', ha='center', fontsize=11, 
                          bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    # Animation update function
    def update_plot(frame):
        """Update plots with new data"""
        # Try to read new data
        for _ in range(10):  # Try to read up to 10 frames per animation update
            if not visualizer.read_frame():
                break
        
        # Get current data
        data = visualizer.get_data_arrays()
        ts = data['timestamps']
        
        if len(ts) == 0:
            return lines_accel + lines_gyro + lines_temp + [status_text]
        
        # Update accelerometer plot
        lines_accel[0].set_data(ts, data['ax'])
        lines_accel[1].set_data(ts, data['ay'])
        lines_accel[2].set_data(ts, data['az'])
        ax_accel.set_xlim(ts[0] if len(ts) > 0 else 0, ts[-1] if len(ts) > 0 else 1)
        
        # Update gyroscope plot
        lines_gyro[0].set_data(ts, data['gx'])
        lines_gyro[1].set_data(ts, data['gy'])
        lines_gyro[2].set_data(ts, data['gz'])
        ax_gyro.set_xlim(ts[0] if len(ts) > 0 else 0, ts[-1] if len(ts) > 0 else 1)
        
        # Update temperature plot
        lines_temp[0].set_data(ts, data['temp'])
        ax_temp.set_xlim(ts[0] if len(ts) > 0 else 0, ts[-1] if len(ts) > 0 else 1)
        
        # Update status text
        if len(data['ax']) > 0:
            status_text.set_text(
                f"Frames: {visualizer.frame_count} | "
                f"Accel: ({data['ax'][-1]:.2f}, {data['ay'][-1]:.2f}, {data['az'][-1]:.2f}) g | "
                f"Temp: {data['temp'][-1]:.1f}°C | "
                f"Gyro: ({data['gx'][-1]:.1f}, {data['gy'][-1]:.1f}, {data['gz'][-1]:.1f}) deg/s"
            )
        
        return lines_accel + lines_gyro + lines_temp + [status_text]
    
    # Create animation
    ani = animation.FuncAnimation(fig, update_plot, interval=100, blit=True,
                                cache_frame_data=False)
    
    plt.tight_layout(rect=[0, 0.03, 1, 0.96])
    plt.show()
    
    visualizer.close()


def main():
    """Main entry point"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Real-time MPU6050 Sensor Data Visualization',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Default settings (port: /dev/ttyUSB0, baud: 9600)
  python3 mpu6050_visualizer.py
  
  # Custom port and baud rate
  python3 mpu6050_visualizer.py -p /dev/ttyACM0 -b 115200
  
  # Larger buffer for longer history
  python3 mpu6050_visualizer.py --buffer 1000
        """
    )
    
    parser.add_argument('-p', '--port', default=SERIAL_PORT,
                        help=f'Serial port (default: {SERIAL_PORT})')
    parser.add_argument('-b', '--baud', type=int, default=BAUD_RATE,
                        help=f'Baud rate (default: {BAUD_RATE})')
    parser.add_argument('-t', '--timeout', type=int, default=TIMEOUT,
                        help=f'Serial timeout in seconds (default: {TIMEOUT})')
    parser.add_argument('--buffer', type=int, default=BUFFER_SIZE,
                        help=f'Data buffer size (default: {BUFFER_SIZE})')
    
    args = parser.parse_args()
    
    print("\n" + "="*70)
    print("MPU6050 Real-Time Sensor Data Visualization")
    print("="*70)
    print(f"Port: {args.port}")
    print(f"Baud Rate: {args.baud}")
    print(f"Buffer Size: {args.buffer}")
    print("\nPress Ctrl+C to exit")
    print("="*70 + "\n")
    
    # Create visualizer
    visualizer = MPU6050Visualizer(
        port=args.port,
        baudrate=args.baud,
        timeout=args.timeout,
        buffer_size=args.buffer
    )
    
    try:
        create_visualization(visualizer)
    except KeyboardInterrupt:
        print("\n✓ Visualization stopped")
        visualizer.close()
    except Exception as e:
        print(f"\n✗ Error: {e}")
        visualizer.close()
        sys.exit(1)


if __name__ == '__main__':
    main()
