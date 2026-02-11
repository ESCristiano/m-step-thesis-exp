# Copilot: Build and Deployment Automation

This directory contains the build automation scripts and TF-M configuration files for the M-Step framework.

## Overview

The `copilot.sh` script automates the entire workflow for configuring, building, and deploying both the Secure world (TF-M) and Non-Secure world applications to the target board.

## Directory Structure

```
copilot/
├── copilot.sh              # Main automation script
├── README.md               # This file
└── tfm-configs/            # TF-M configuration files
    ├── nucleo_l552ze_q-bare-config.cmake    # Minimal configuration
    ├── nucleo_l552ze_q-crypto-config.cmake  # Crypto services enabled
    ├── nucleo_l552ze_q-mstp-config.cmake    # Full M-Step configuration
    └── profile/            # Additional profile configurations
```

## Usage

```bash
./copilot.sh [OPTIONS]
```

## Options

| Option | Arguments | Description |
|--------|-----------|-------------|
| `-c, --config` | `s` \| `ns` | Configure the build system for Secure or Non-Secure world |
| `-b, --build` | `s` \| `ns`  | Build the specified image |
| `-p, --profile` | `bare` \| `crypto` \| `mstp` | Select build profile (default: `bare`) |
| `-t, --target` | `STM32L5` | Specify target board (default: `STM32L5`) |
| `-d, --deploy` | - | Deploy built images to the target board |
| `-m, --monitor` | `<output_file>` | Start serial monitor and save output |
| `-vi, --visualizer` | `<trace> <elf> <out_dir>` | Launch GTKWave trace visualizer |

## Build Profiles

### `bare` - Minimal Configuration
- Minimal TF-M with basic services
- Suitable for testing basic functionality
- Smallest memory footprint

### `crypto` - Crypto Services
- TF-M with cryptographic services enabled
- Includes Mbed TLS integration
- Required for crypto-related experiments

### `mstp` - Full M-Step Setup
- Complete M-Step evaluation configuration
- Includes all M-Step plugins and test infrastructure
- Custom Trusted Applications for victim code
- Required for running the paper's evaluation

## Workflow Examples

### Basic Build and Deploy

```bash
# 1. Configure the Secure world
./copilot.sh -c s -p mstp

# 2. Build the Secure world
./copilot.sh -b s -p mstp

# 3. Configure the Non-Secure world
./copilot.sh -c ns -p mstp

# 4. Build the Non-Secure world
./copilot.sh -b ns -p mstp

# 5. Deploy to target board
./copilot.sh -d -p mstp
```

### Monitor and Capture Traces

```bash
# Start monitoring and save output to file
./copilot.sh -m trace_output.txt
```

### Visualize Traces

```bash
# Generate VCD and open in GTKWave
./copilot.sh -vi trace.txt tfm_s.elf ./visualization_output/
```

## Build Order

The build process must follow this order:

1. **Configure Secure World** (`-c s`): Sets up CMake for TF-M
2. **Build Secure World** (`-b s`): Compiles TF-M and generates SPE API
3. **Configure Non-Secure World** (`-c ns`): Sets up CMake for NS app (requires S world built)
4. **Build Non-Secure World** (`-b ns`): Compiles NS application
5. **Deploy** (`-d`): Flashes both images to the board

> **Note**: The NS world configuration depends on the Secure world build output, so always build the Secure world first.

## First-Time Setup

On the first deployment, you need to configure the board's option bytes for TrustZone:

```bash
# After building, run the regression script (only needed once)
./build/nucleo_l552ze_q_mstp_s/api_ns/regression.sh
```

Alternatively, uncomment the `regression.sh` line in `copilot.sh` for automatic execution.

## TF-M Configuration Files

The `tfm-configs/` directory contains CMake configuration files that customize TF-M for different use cases:

| File | Purpose |
|------|---------|
| `*-bare-config.cmake` | Minimal TF-M, no crypto services |
| `*-crypto-config.cmake` | TF-M with Mbed TLS crypto services |
| `*-mstp-config.cmake` | Full M-Step setup with custom TAs |

### Custom Trusted Applications

The M-Step configuration includes custom Trusted Applications (TAs) for evaluation:

- **appRoT**: Application Root of Trust partition for victim code
- **PRoT_crypto_attack**: Platform Root of Trust partition for crypto attacks

These are located in `m-step/mstp-victims/TAs/`.

## Troubleshooting

### Build Fails with "SPE API not found"

```bash
# Ensure the Secure world is built first
./copilot.sh -b s -p mstp
```

### Deployment Fails

```bash
# Check if board is connected
STM32_Programmer_CLI -l

# Verify udev rules are configured (see main README)
```

### Missing mbedtls_config.h

This is a known TF-M issue when building Mbed TLS out of tree. The script automatically copies the missing file, but if issues persist:

```bash
cp ../s-world/mbedtls/include/mbedtls/mbedtls_config.h \
   ./build/nucleo_l552ze_q_mstp_s/api_ns/interface/include/mbedtls/
```
