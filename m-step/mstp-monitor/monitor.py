#!/usr/bin/env python3
"""
STM32L5 UART Test Logger
Captures serial output between markers and logs to file
"""

import serial
import subprocess
import time
import sys
import argparse
import os
from datetime import datetime

# Configuration
SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 115200
START_MARKER = "Hello NS World!!"
END_MARKER = "End NS World!!"
RESET_COMMAND = "STM32_Programmer_CLI -c port=SWD mode=UR"
TIMEOUT = 60*10  # seconds to wait for end marker

# Global verbosity level
VERBOSITY = 1

def vprint(level, *args, **kwargs):
    """Print only if verbosity level is sufficient"""
    if VERBOSITY >= level:
        print(*args, **kwargs)

def reset_board():
    """Execute the board reset command"""
    vprint(1, f"Resetting board with command: {RESET_COMMAND}")
    try:
        result = subprocess.run(RESET_COMMAND, shell=True, 
                              capture_output=True, text=True, timeout=5)
        if result.returncode != 0:
            print(f"Warning: Reset command returned {result.returncode}")
            print(f"stderr: {result.stderr}")
        time.sleep(0.5)  # Give board time to start resetting
        return True
    except subprocess.TimeoutExpired:
        print("Error: Reset command timed out")
        return False
    except Exception as e:
        print(f"Error executing reset command: {e}")
        return False

def capture_uart_output(log_file):
    """Capture UART output between markers and save to log file"""
    try:
        # Open serial port
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        vprint(1, f"Opened serial port {SERIAL_PORT} at {BAUD_RATE} baud")
        
        # Clear any existing data in buffer
        ser.reset_input_buffer()
        
        # Reset the board
        if not reset_board():
            vprint(1, "Failed to reset board, but continuing anyway...")
        
        # Wait for start marker
        vprint(1, f"Waiting for start marker: '{START_MARKER}'")
        capturing = False
        log_lines = []
        start_time = time.time()
        
        while True:
            if time.time() - start_time > TIMEOUT and not capturing:
                print(f"Timeout waiting for start marker after {TIMEOUT}s")
                return False
            
            try:
                line = ser.readline().decode('utf-8', errors='replace').rstrip()
            except UnicodeDecodeError:
                continue
            
            if not line:
                continue
            
            # Print to console for monitoring
            vprint(1, f"  {line}")
            
            # Check for start marker
            if START_MARKER in line and not capturing:
                vprint(1, f"\n>>> Start marker detected! Beginning capture...")
                capturing = True
                log_lines.append(line)
                start_time = time.time()  # Reset timeout for end marker
                continue
            
            # If capturing, save lines
            if capturing:
                log_lines.append(line)
                
                # Check for end marker
                if END_MARKER in line:
                    vprint(1, f">>> End marker detected! Capture complete.\n")
                    break
            
            # Timeout check while capturing
            if capturing and time.time() - start_time > TIMEOUT:
                print(f"Timeout waiting for end marker after {TIMEOUT}s")
                vprint(1, "Saving partial log...")
                break
        
        # Write to log file
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        with open(log_file, 'a') as f:
            f.write(f"\n{'='*60}\n")
            f.write(f"Test run at {timestamp}\n")
            f.write(f"{'='*60}\n")
            for line in log_lines:
                f.write(line + '\n')
            f.write('\n')
        
        vprint(1, f"Logged {len(log_lines)} lines to {log_file}")
        
        ser.close()
        return True
        
    except serial.SerialException as e:
        print(f"Serial port error: {e}")
        return False
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return False
    except Exception as e:
        print(f"Unexpected error: {e}")
        return False

def parse_arguments():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(
        description='STM32L5 UART Test Logger - Captures serial output between markers'
    )
    parser.add_argument(
        '-o', '--output-dir',
        type=str,
        default='.',
        help='Output directory for log files (default: current directory)'
    )
    parser.add_argument(
        '-v', '--verbosity',
        type=int,
        choices=[0, 1],
        default=1,
        help='Verbosity level: 0 (minimal) or 1 (detailed) (default: 1)'
    )
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_arguments()
    VERBOSITY = args.verbosity
    
    # Determine if output is a file or directory
    output_path = args.output_dir
    
    # Check if it looks like a file (has an extension) or is an existing file
    if os.path.splitext(output_path)[1] or (os.path.exists(output_path) and os.path.isfile(output_path)):
        # User specified a file path
        log_file = output_path
        output_dir = os.path.dirname(output_path) or '.'
    else:
        # User specified a directory path
        output_dir = output_path
        log_file = os.path.join(output_dir, 'trace.log')
    
    # Create output directory if it doesn't exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
        vprint(1, f"Created output directory: {output_dir}")
    
    vprint(1, "STM32L5 UART Test Logger")
    vprint(1, "-" * 40)
    
    success = capture_uart_output(log_file)
    
    if success:
        print("Test capture completed successfully")
        sys.exit(0)
    else:
        print("Test capture failed")
        sys.exit(1)