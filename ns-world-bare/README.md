# Non-Secure World: Baremetal Runtime Environment

This directory provides the Non-Secure (NS) world runtime environment as a baremetal application. It serves as the attack platform from which M-Step executes and observes Secure world code.

## Overview

The baremetal NS world is the primary environment for running M-Step experiments. It provides:

- Direct hardware access for precise timer control
- Minimal overhead for accurate timing measurements
- Integration with the M-Step framework
- Serial output for trace capture

## Directory Structure

```
ns-world-bare/
├── 0-config.sh             # Configure build system
├── 1-compile.sh            # Compile NS and S images
├── 2-deploy.sh             # Flash to target board
├── 3-monitor.sh            # Capture serial output
└── src/                    # Source code
    ├── CMakeLists.txt      # CMake configuration
    ├── Drivers/            # HAL drivers
    └── NonSecure/          # NS application code
```

## Quick Start

### Complete Workflow

```bash
# Enter the Nix development environment
cd /path/to/m-step
nix develop

# Navigate to this directory
cd ns-world-bare

# Step 1: Configure (sets up CMake for both S and NS)
./0-config.sh

# Step 2: Compile (builds S world, then NS world)
./1-compile.sh

# Step 3: Deploy (flashes images to the board)
./2-deploy.sh

# Step 4: Monitor (captures serial output)
./3-monitor.sh -o trace.txt
```

## Scripts

### `0-config.sh` - Configure Build System

Sets up the CMake build system for both Secure and Non-Secure worlds.

```bash
./0-config.sh           # Configure
./0-config.sh -c        # Clean build directories
./0-config.sh -h        # Show help
```

**What it does:**
1. Calls `copilot.sh -c s` to configure TF-M (Secure world)
2. Runs CMake to configure the NS world build

### `1-compile.sh` - Compile Images

Compiles the Secure and Non-Secure world images.

```bash
./1-compile.sh              # Normal build
./1-compile.sh --test-mode  # Build with test configuration
./1-compile.sh -h           # Show help
```

**What it does:**
1. Builds the Secure world (TF-M)
2. Builds the Non-Secure world application
3. Signs the NS image with MCUboot keys

### `2-deploy.sh` - Deploy to Board

Flashes the compiled images to the target board.

```bash
./2-deploy.sh           # Deploy images
./2-deploy.sh -h        # Show help
```

**What it does:**
1. Runs the TF-M post-build script
2. Flashes the bootloader, S world, and NS world images

### `3-monitor.sh` - Capture Serial Output

Monitors the serial port and captures output to a file.

```bash
./3-monitor.sh -o trace.txt     # Capture to file
./3-monitor.sh -h               # Show help
```

**What it does:**
1. Resets the board
2. Waits for the start marker (`Hello NS World!!`)
3. Captures all output until the end marker (`End NS World!!`)
4. Saves the captured trace to the specified file
