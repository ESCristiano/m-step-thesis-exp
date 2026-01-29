# M-Step Debug Tools

Debugging utilities for M-Step development, including GDB scripts, OpenOCD configurations, and Renode emulator setup.

## Overview

This directory contains tools for:

- **GDB-Py**: Python-enhanced GDB scripts for debugging
- **OpenOCD**: JTAG/SWD debugging configurations for STM32L5
- **Renode**: MCU emulator for trace replay and post-mortem analysis

## Directory Structure

```
mstp-debug/
├── README.md           # This file
├── gdb-py/             # GDB Python scripts
│   └── *.py            # Custom GDB commands
├── openocd/            # OpenOCD configurations
│   └── *.cfg           # Board/target configs
└── renode/             # Renode emulator setup
    ├── 1-renode.sh     # Renode launcher script
    ├── boards/         # Board definitions
    ├── setups/         # Emulation configurations
    ├── shell.nix       # Nix environment
    └── src/            # Custom Renode peripherals
```

## GDB Python Scripts (`gdb-py/`)

Custom GDB commands for M-Step debugging:

```bash
# Start GDB with Python scripts loaded
arm-none-eabi-gdb -x gdb-py/init.py tfm_s.elf
```

### Available Commands

| Command | Description |
|---------|-------------|
| `mstp-state` | Print current M-Step state machine state |
| `mstp-ctx` | Dump M-Step runtime context |
| `mstp-trace` | Show recent trace entries |

## OpenOCD (`openocd/`)

Configuration files for hardware debugging:

```bash
# Start OpenOCD for STM32L5
openocd -f openocd/stm32l5.cfg

# In another terminal, connect GDB
arm-none-eabi-gdb -ex "target remote :3333" tfm_s.elf
```

### Configuration Files

| File | Purpose |
|------|---------|
| `stm32l5.cfg` | STM32L5 target configuration |
| `nucleo-l552ze-q.cfg` | NUCLEO board configuration |

## Renode Emulator (`renode/`)

Renode allows emulating the STM32L5 for:

- Post-mortem trace analysis
- Debugging without hardware
- Automated testing

### Quick Start

```bash
cd renode

# Enter Nix environment (provides Renode)
nix-shell shell.nix

# Launch Renode with M-Step setup
./1-renode.sh
```

### Renode Scripts

The `1-renode.sh` script:

1. Loads the STM32L5 platform definition
2. Loads the Secure and Non-Secure world binaries
3. Configures peripherals (UART, timers)
4. Starts emulation

### Board Definitions (`boards/`)

Platform definitions for Renode:

- Memory map configuration
- Peripheral instantiation
- Interrupt routing

### Custom Peripherals (`src/`)

Custom Renode peripheral models for accurate STM32L5 emulation.

## Debugging Workflow

### Hardware Debugging

1. Connect the NUCLEO board
2. Start OpenOCD:
   ```bash
   openocd -f openocd/nucleo-l552ze-q.cfg
   ```
3. Connect GDB:
   ```bash
   arm-none-eabi-gdb -x gdb-py/init.py \
     ../copilot/build/nucleo_l552ze_q_mstp_s/bin/tfm_s.elf
   ```
4. In GDB:
   ```
   (gdb) target remote :3333
   (gdb) monitor reset halt
   (gdb) load
   (gdb) continue
   ```

### Emulation Debugging

1. Start Renode:
   ```bash
   cd renode && ./1-renode.sh
   ```
2. In Renode console:
   ```
   (renode) start
   (renode) pause
   ```
3. Connect GDB to Renode:
   ```bash
   arm-none-eabi-gdb -ex "target remote :3333" tfm_s.elf
   ```

## Troubleshooting

### Renode Emulation Fails

Make sure to comment the uart configuration on the `ns-world-bare/src/NonSecure/Core/Src/main.c`

```C
// We have to comment MX_LPUART1_UART_Init to run on RENODE
MX_LPUART1_UART_Init();
```


