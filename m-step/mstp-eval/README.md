# M-Step Evaluation Framework

This directory provides the infrastructure for configuring, running, and managing M-Step experiments and tests.

## Overview

The evaluation framework allows you to:

- Define and run custom experiments against victim code
- Configure M-Step parameters per test
- Manage board configurations
- Orchestrate test execution

## Directory Structure

```
mstp-eval/
├── README.md           # This file
├── inc/                # Header files
│   ├── test_eval_config.h    # Test enable/disable flags
│   ├── mstp_eval.h           # Evaluation framework API
│   └── ...
└── src/                # Source files
    ├── mstp_eval.c           # Main evaluation logic
    ├── test_*.c              # Individual test implementations
    └── ...
```

## Quick Start

### Running an Existing Test

```c
// In your main.c
#include "mstp_eval.h"
#include "test_covert.h"

int main(void) {
    // Run a trace-based test
    run_trace(test_conf_covert, BOARD_DEFAULT, TEST_UDIV, S_TFM);
    
    // Or run without tracing (for metrics)
    run_no_trace(test_conf_covert, BOARD_DEFAULT, TEST_UDIV, S_TFM);
}
```

### Enabling/Disabling Tests

Edit `inc/test_eval_config.h`:

```c
// Enable specific tests
#define TEST1_ENABLE    // M-Step metrics
#define TEST2_ENABLE    // Division timing
// #define TEST3_ENABLE  // Disabled
```

## Adding New Tests

### Step 1: Create Header File

Create `inc/test_mytests.h`:

```c
#ifndef TEST_MYTESTS_H
#define TEST_MYTESTS_H

#include "mstp_eval.h"

// Define test enumeration
typedef enum {
    MYTEST_001,
    MYTEST_002,
    N_MYTESTS       // Must be last - used to count tests
} mytest_id_t;

// Export test configuration
extern mstp_test_conf_t test_conf_mytests;

#endif
```

### Step 2: Create Source File

Create `src/test_mytests.c`:

```c
#include "test_mytests.h"
#include "mstp.h"

//------------------------------------------------------------------------------
// Test Functions
//------------------------------------------------------------------------------

void mytest_001(void) {
    // Your victim/test code here
    volatile int x = 0;
    for (int i = 0; i < 100; i++) {
        x += i;
    }
}

void mytest_002(void) {
    // Another test
}

//------------------------------------------------------------------------------
// Test Arrays
//------------------------------------------------------------------------------

// Array of test function pointers
void (*mytests[N_MYTESTS])() = {
    mytest_001,
    mytest_002
};

// Array of test names (for logging)
char *mytest_names[N_MYTESTS] = {
    "mytest_001",
    "mytest_002"
};

//------------------------------------------------------------------------------
// M-Step Configuration (Optional)
//------------------------------------------------------------------------------

// Custom M-Step configuration per test
// Set to NULL to use defaults
mstp_conf_t mstp_conf_mytests[N_MYTESTS] = {
    // Configuration for mytest_001
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = STREAK_THRESHOLD,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,
        .debug                  = DEBUG_ON,
        .trace_enable           = ENABLE_TRACE,
        .print_iri_latency      = DISABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .mstp_cache_enabled     = DISABLE_MSTP_CACHE,
        .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
        .start_trace_window     = 0,
        .end_trace_window       = (uint64_t)-1
    },
    // Configuration for mytest_002
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = STREAK_THRESHOLD,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,
        .debug                  = DEBUG_OFF,
        .trace_enable           = ENABLE_TRACE,
        .print_iri_latency      = DISABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .mstp_cache_enabled     = ENABLE_MSTP_CACHE,
        .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
        .start_trace_window     = 0,
        .end_trace_window       = (uint64_t)-1
    }
};

//------------------------------------------------------------------------------
// Test Configuration Export
//------------------------------------------------------------------------------

mstp_test_conf_t test_conf_mytests = {
    .board_config   = board_conf,       // Use default board config
    .config_name    = board_conf_names, // Use default board config names
    .test           = mytests,          // Your test functions
    .name           = mytest_names,     // Your test names
    .mstp_conf      = mstp_conf_mytests,// Your M-Step config (or NULL)
    .n_experiments  = N_MYTESTS         // Number of tests
};
```

### Step 3: Add to Build System

Edit `../CMakeLists.txt` to include your new source file:

```cmake
set(MSTP_EVAL_SOURCES
    mstp-eval/src/mstp_eval.c
    mstp-eval/src/test_mytests.c  # Add this line
    # ... other sources
)
```

### Step 4: Run Your Tests

```c
// In main.c
#include "test_mytests.h"

run_trace(test_conf_mytests, BOARD_DEFAULT, MYTEST_001, S_TFM);
run_trace(test_conf_mytests, BOARD_DEFAULT, MYTEST_002, S_TFM);
```

## Configuration Options

### M-Step Configuration Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `base_ISR_time` | `uint32_t` | Interrupt handler overhead in cycles |
| `base_clk` | `uint32_t` | Base timer value for interrupt injection |
| `base_clk_2_collide` | `uint32_t` | Timer value for contention detection |
| `streak_threshold` | `uint32_t` | Zero-steps before recovery action |
| `ici_streak_threshold` | `uint32_t` | ICI instructions before restart |
| `preemptions_threshold` | `uint32_t` | Preemptions before clock adjustment |
| `base_inst_time` | `uint32_t` | Expected single instruction time |
| `debug` | `uint8_t` | `DEBUG_ON` or `DEBUG_OFF` |
| `trace_enable` | `uint8_t` | `ENABLE_TRACE` or `DISABLE_TRACE` |
| `print_iri_latency` | `uint8_t` | Print interrupt latency |
| `print_ici_latency` | `uint8_t` | Print ICI latency |
| `start_trace_window` | `uint64_t` | Start tracing at instruction N |
| `end_trace_window` | `uint64_t` | Stop tracing at instruction N |
| `mstp_cache_enabled` | `uint8_t` | Enable cache timing channel |
| `mstp_busted_enabled` | `uint8_t` | Enable bus contention channel |

### Default Values

```c
// From mstp.h
#define STREAK_THRESHOLD        10
#define ICI_STREAK_THRESHOLD    4
#define PREEMPTIONS_THRESHOLD   5
#define BASE_CLK_S              5
#define BASE_ISR_TIME_S         47
#define BASE_ISR_TIME_NS        29
```

### Board Configurations

| Constant | Description |
|----------|-------------|
| `BOARD_DEFAULT` | Standard configuration |
| `BOARD_CACHE_ENABLED` | Cache enabled for victim |
| `BOARD_CACHE_DISABLED` | Cache disabled |

### Target Worlds

| Constant | Description |
|----------|-------------|
| `S_TFM` | Secure world (TF-M) |
| `NS_BARE` | Non-Secure baremetal |

## API Reference

### `run_trace()`

Run a test with M-Step tracing enabled.

```c
void run_trace(
    mstp_test_conf_t config,    // Test configuration
    board_conf_t board,         // Board configuration
    int test_id,                // Test ID from enum
    target_t target             // Target world
);
```

### `run_no_trace()`

Run a test without tracing (for metrics collection).

```c
void run_no_trace(
    mstp_test_conf_t config,
    board_conf_t board,
    int test_id,
    target_t target
);
```

## Using Default Configurations

If you don't need custom M-Step settings, set `mstp_conf` to `NULL`:

```c
mstp_test_conf_t test_conf_simple = {
    .board_config   = board_conf,
    .config_name    = board_conf_names,
    .test           = my_tests,
    .name           = my_test_names,
    .mstp_conf      = NULL,         // Use defaults
    .n_experiments  = N_TESTS
};
```

## Trace Window

To trace only a portion of execution (useful for large victim code):

```c
mstp_conf_t config = {
    // ...
    .start_trace_window = 1000,     // Start at instruction 1000
    .end_trace_window   = 5000,     // Stop at instruction 5000
    // ...
};
```

## Example: Complete Test File

See existing tests for complete examples:

- `m-step/mstp-eval/src/mstp_eval_covert_ch.c` - Covert channel tests
- `m-step/mstp-eval/src/mstp_eval_mstp_metrics.c` - Performance metrics
- `m-step/mstp-eval/src/mstp_eval_poc.c` - Proof-of-concept attacks