# M-Step Framework: Core Implementation

This directory contains the source code for the M-Step single-stepping framework, including the core mechanisms, evaluation infrastructure, debugging tools, and analysis utilities.

## Overview

M-Step is a software-based single-stepping attack framework that exploits timer interrupts to achieve instruction-level observation of Secure world code execution from the Non-Secure world on ARM TrustZone-M microcontrollers.

## Core Components

### Core Single-Stepping Implementation (`mstp/`)

The heart of the M-Step framework. Implements the timer-based interrupt injection mechanism.

**Key Files:**
- `mstp.h`: Configuration parameters and data structures
- `mstp.c`: Main single-stepping algorithm and state machine

**Key Parameters (in `mstp.h`):**

| Parameter | Default | Description |
|-----------|---------|-------------|
| `STREAK_THRESHOLD` | 10 | Consecutive zero-steps before recovery |
| `ICI_STREAK_THRESHOLD` | 4 | Max ICI instructions before restart |
| `PREEMPTIONS_THRESHOLD` | 5 | Max preemptions before clock adjustment |
| `BASE_CLK` | 5 | Base timer value for interrupt injection |
| `BASE_ISR_TIME` | 29/47 | Interrupt handler overhead (NS/S) |

**State Machine States:**
- `ATOMIC_INST`: Normal single-step execution
- `ICI_RESUME`: Resuming interrupted-continuable instruction
- `TRY_SINGLE_STEP`: Attempting to single-step
- `ICI_RESTART`: Restarting ICI after overflow
- `PREEMPTION`: Handling preemption recovery


<!-- ### Mstp-eval

Provides infrastructure for running experiments and tests.

**See [mstp-eval/README.md](mstp-eval/README.md) for detailed instructions on:**
- Adding new tests
- Configuring experiments
- Customizing M-Step parameters

### Mstp-victims

Contains the vulnerable code used in experiments:

- **`s/`**: Secure world victim functions (assembly and C)
- **`TAs/`**: Trusted Applications integrated with TF-M
  - `appRoT/`: Application Root of Trust partition
  - `PRoT_crypto_attack/`: Crypto attack partition

### Mstp-visualizer

Converts M-Step traces to VCD format for analysis in GTKWave.

```bash
# Generate VCD from trace
./1-gen-vcd.sh -t trace.txt -elf tfm_s.elf -o ./outputs

# Open in GTKWave
./2-gtkwave.sh -i ./outputs/trace.vcd
```

### Mstp-debug

- **gdb-py/**: Python-enhanced GDB scripts for debugging
- **openocd/**: OpenOCD configuration files for JTAG debugging
- **renode/**: Renode emulator setup for trace replay and analysis

### Mstp-monitor 

Python utility for capturing UART output from the target board.

```bash
python3 monitor.py -o trace.txt
``` -->

## M-Step Configuration

### Runtime Configuration Structure

```c
typedef struct {
    uint32_t base_ISR_time;         // Interrupt handler overhead
    uint32_t base_clk;              // Base timer value
    uint32_t base_clk_2_collide;    // Timer value for contention
    uint32_t streak_threshold;      // Zero-step detection threshold
    uint32_t ici_streak_threshold;  // ICI detection threshold
    uint32_t preemptions_threshold; // Preemption recovery threshold
    uint32_t base_inst_time;        // Base instruction time
    uint8_t  debug;                 // Debug output enable
    uint8_t  trace_enable;          // Trace output enable
    uint8_t  print_iri_latency;     // Print IRI latency
    uint8_t  print_ici_latency;     // Print ICI latency
    uint64_t start_trace_window;    // Trace start instruction
    uint64_t end_trace_window;      // Trace end instruction
    uint8_t  mstp_cache_enabled;    // Enable cache timing
    uint8_t  mstp_busted_enabled;   // Enable bus contention
} mstp_conf_t;
```

### Enabling/Disabling Features

```c
// In your test configuration
mstp_conf_t my_config = {
    .base_ISR_time          = BASE_ISR_TIME_S,
    .base_clk               = BASE_CLK_S,
    .debug                  = DEBUG_ON,
    .trace_enable           = ENABLE_TRACE,
    .mstp_cache_enabled     = ENABLE_MSTP_CACHE,
    .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
    // ...
};
```

## Building

The M-Step framework is built as part of the Non-Secure world application:

```bash
# From the repository root
cd ns-world-bare
./0-config.sh
./1-compile.sh
```

Or using copilot:

```bash
cd copilot
./copilot.sh -b ns_costum -p mstp
```

## Sample Traces

The `traces/` directory contains pre-captured traces for analysis:

| Trace | Description |
|-------|-------------|
| `mbedtls-mpi-cmp-profile-001/` | MPI comparison profiling |
| `tfm-inv-mod-001/` | Modular inversion trace |
| `tfm-inv-mod-rsa-001/` | RSA modular inversion |
| `tfm-rsa-dec-001/` | RSA decryption trace |

These can be visualized using:

```bash
cd mstp-visualizer
./1-gen-vcd.sh -t ../traces/tfm-inv-mod-001/trace.txt \
               -elf ../traces/tfm-inv-mod-001/tfm_s.elf \
               -o ./outputs
./2-gtkwave.sh -i ./outputs/trace.vcd
```

## API Reference

### Start single-stepping a victim function

```c
void trace(void (*victim)());
```

### Get current M-Step context per interrupt

```c
typedef struct {
        uint32_t streak; 
        uint32_t streak_mul_clks; 
        int32_t inst_time;
        int32_t inst_time_prev;
        uint32_t partial_step;
        uint32_t clk_it;
        uint32_t clk_2_collide;
        uint32_t stack_clk_offset;
        uint32_t ici_restart_clk_it;
        uint32_t ici_resume_pc_clk_it;
        uint32_t secure_stack_prev;
        uint32_t streak_overflow;
        uint32_t mul_clk_streak_overflow;
        uint32_t partial_step_saved;
        uint32_t ici_streak;
        uint32_t lazy_stacking_en;
        uint32_t preemptions;
        uint64_t interrupts_counter;
        state_t  state_n;
        state_t  state_p;
        lr_t    lr_exc_ret;
} mstp_ctx_t;
```

### Trace Output Format

Mstp-debug outputs traces in the following format (PC Cycle):

```C
0x0C001234 5
0x0C001238 3
0x0C00123C 8
...
```

## Extending M-Step

### Adding a New Plugin

1. Create a new directory `mstp-<plugin>/`
2. Implement the observation mechanism in `src/`
3. Add configuration flags to `mstp.h`
4. Integrate with the main `mstp.c` state machine
5. Add to `CMakeLists.txt`

### Adding New Victim Code

1. Add victim functions to `mstp-victims/s/`
2. Create a Trusted Application in `mstp-victims/TAs/` if needed
3. Register with the evaluation framework in `mstp-eval/`
