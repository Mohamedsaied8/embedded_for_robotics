# VSCode GUI Debugging Guide

## Quick Start

### 1. Install Required Extensions

When you open this project in VSCode, you'll see a notification to install recommended extensions. Click **Install All** or install them manually:

- **Cortex-Debug** (marus25.cortex-debug) - Essential for STM32 debugging with GUI
- **C/C++ Extension Pack** (ms-vscode.cpptools) - IntelliSense and code navigation
- **CMake Tools** (ms-vscode.cmake-tools) - CMake integration
- **CMake** (twxs.cmake) - CMake language support

### 2. Build the Project

**Option 1: Using GUI**
- Press `Ctrl+Shift+B` (or `Cmd+Shift+B` on Mac)
- Select **Build** from the task list

**Option 2: Using Command Palette**
- Press `Ctrl+Shift+P`
- Type "Tasks: Run Task"
- Select **Build**

**Option 3: Using Terminal**
```bash
./build.sh
```

### 3. Start Debugging

**Method 1: Using Run/Debug Button (Recommended)**
1. Click the **Run and Debug** icon in the left sidebar (or press `Ctrl+Shift+D`)
2. Select a debug configuration from the dropdown:
   - **Cortex Debug (OpenOCD)** - Recommended for most users
   - **Cortex Debug (ST-Link)** - Direct ST-Link connection
   - **Debug (GDB - OpenOCD)** - Fallback option
3. Click the **green play button** (▶️) or press `F5`

**Method 2: Using Menu**
- Go to **Run → Start Debugging** (F5)

**Method 3: Using Keyboard**
- Press `F5` to start debugging

## Debug Configurations

### 🔧 Cortex Debug (OpenOCD) - Recommended

Best for most users. Provides:
- ✅ Full GUI debugging with peripheral registers
- ✅ Memory viewer
- ✅ SWO (Serial Wire Output) support
- ✅ Real-time variable watching
- ✅ Automatic build before debug

**Requirements:**
- OpenOCD installed: `sudo apt install openocd`
- STLink v2 connected

### 🔧 Cortex Debug (ST-Link)

Direct ST-Link connection without OpenOCD:
- ✅ Faster connection
- ✅ Same GUI features as OpenOCD
- ✅ No OpenOCD server needed

**Requirements:**
- st-util installed: `sudo apt install stlink-tools`
- STLink v2 connected

### 🔧 Debug (GDB - OpenOCD)

Fallback option using standard GDB:
- Basic debugging without Cortex-Debug extension
- Requires OpenOCD server running separately

## GUI Features

### Breakpoints
- Click in the left margin (gutter) next to line numbers
- Red dots indicate active breakpoints
- Right-click for conditional breakpoints

### Debug Controls (Top Bar)
- **Continue** (F5) - Resume execution
- **Step Over** (F10) - Execute current line
- **Step Into** (F11) - Step into function
- **Step Out** (Shift+F11) - Step out of function
- **Restart** (Ctrl+Shift+F5) - Restart debugging
- **Stop** (Shift+F5) - Stop debugging

### Variables Panel
- View local variables and their values
- Expand structures and arrays
- Watch expressions in real-time

### Call Stack
- See function call hierarchy
- Click to navigate to different stack frames

### Peripheral Registers (Cortex-Debug Only)
- View STM32 peripheral registers (GPIO, RCC, etc.)
- Real-time register updates
- Edit register values during debugging

### Memory Viewer (Cortex-Debug Only)
- View memory at specific addresses
- Hex and ASCII representation
- Search and navigate memory

### SWO Output (Cortex-Debug Only)
- View ITM console output
- Real-time printf debugging
- Configure in launch.json

## Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| Start Debugging | `F5` |
| Stop Debugging | `Shift+F5` |
| Restart | `Ctrl+Shift+F5` |
| Step Over | `F10` |
| Step Into | `F11` |
| Step Out | `Shift+F11` |
| Toggle Breakpoint | `F9` |
| Build | `Ctrl+Shift+B` |
| Run Task | `Ctrl+Shift+P` → "Tasks: Run Task" |

## Available Tasks

Access via `Ctrl+Shift+B` or Command Palette:

1. **Build** - Compile the project
2. **Rebuild** - Clean and rebuild
3. **Clean** - Remove build artifacts
4. **Flash** - Build and flash to STM32

## Troubleshooting

### Extension Not Installed
**Error**: "The debug type is not recognized"

**Solution**: Install Cortex-Debug extension:
1. Press `Ctrl+Shift+X`
2. Search for "Cortex-Debug"
3. Click **Install**

### OpenOCD Connection Failed
**Error**: "Could not connect to OpenOCD"

**Solution**:
```bash
# Check if OpenOCD is installed
openocd --version

# Test connection manually
openocd -f scripts/openocd.cfg
```

### STLink Not Found
**Error**: "Error: libusb_open() failed with LIBUSB_ERROR_ACCESS"

**Solution**: Add udev rules
```bash
sudo cp /usr/share/stlink/49-stlinkv*.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
# Replug STLink
```

### Build Failed
**Error**: Build task fails

**Solution**:
```bash
# Check ARM GCC installation
arm-none-eabi-gcc --version

# Try manual build
./build.sh rebuild
```

### GDB Path Not Found
**Error**: "Unable to find GDB"

**Solution**: Update `.vscode/settings.json`:
```json
{
    "cortex-debug.gdbPath": "/usr/bin/arm-none-eabi-gdb"
}
```

## Tips & Tricks

### 1. Quick Flash Without Debug
- Press `Ctrl+Shift+P`
- Type "Tasks: Run Task"
- Select **Flash**

### 2. Watch Variables
- Right-click on a variable
- Select "Add to Watch"
- View in Watch panel during debugging

### 3. Conditional Breakpoints
- Right-click on breakpoint
- Select "Edit Breakpoint"
- Add condition (e.g., `i == 10`)

### 4. Logpoints
- Right-click in gutter
- Select "Add Logpoint"
- Enter message to log (no code changes needed!)

### 5. View Peripheral Registers
- During debug, open **Cortex Peripherals** panel
- Expand GPIO, RCC, or other peripherals
- See real-time register values

### 6. Memory Inspection
- During debug, press `Ctrl+Shift+P`
- Type "Cortex-Debug: View Memory"
- Enter address (e.g., `0x20000000` for RAM)

## Advanced Configuration

### Enable SWO Output (Printf Debugging)

1. Add to your code:
```c
// In main.c
#include "stdio.h"

// Retarget printf to SWO
int _write(int file, char *ptr, int len) {
    for(int i = 0; i < len; i++) {
        ITM_SendChar((*ptr++));
    }
    return len;
}
```

2. Update launch.json:
```json
"swoConfig": {
    "enabled": true,
    "cpuFrequency": 72000000,
    "swoFrequency": 2000000,
    "source": "probe",
    "decoders": [
        {
            "type": "console",
            "label": "ITM",
            "port": 0
        }
    ]
}
```

3. Use printf in your code:
```c
printf("Counter: %d\n", counter);
```

### Custom Debug Commands

Add to launch.json `preLaunchCommands`:
```json
"preLaunchCommands": [
    "set print pretty on",
    "set pagination off"
]
```

## Summary

✅ **Install Cortex-Debug extension** for best experience
✅ **Press F5** to start debugging with GUI
✅ **Use breakpoints** by clicking in the gutter
✅ **View peripherals** in Cortex Peripherals panel
✅ **Watch variables** in real-time
✅ **Step through code** with F10/F11

Happy debugging! 🚀
