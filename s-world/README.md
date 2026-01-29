# Secure World: Runtime Environment and Third-Party Modules

This directory contains the Secure (S) world runtime environment and third-party modules required for M-Step experiments. All components are managed as Git submodules.

## Overview

The Secure world hosts the victim code that M-Step targets. It consists of:

- **Trusted Firmware-M (TF-M)**: The secure firmware providing isolation and services
- **Mbed TLS**: Cryptographic library used in proof-of-concept attacks
- **MCUboot**: Secure bootloader providing Root of Trust
- **Custom patches**: Modifications for debugging and PoC attacks

## Directory Structure

```
s-world/
├── mbedtls/                # Mbed TLS cryptographic library
├── mcuboot/                # MCUboot secure bootloader
├── patches/                # Custom patches for TF-M and Mbed TLS
├── tf-m-tests/             # Official TF-M test suite
└── trusted-firmware-m/     # Trusted Firmware-M (TF-M)
```

## Components

### Trusted Firmware-M (`trusted-firmware-m/`)

**Version**: TF-Mv2.2.0 (configurable via build scripts)

**Documentation**: [TF-M User Guide](https://tf-m-user-guide.trustedfirmware.org/)

### Mbed TLS (`mbedtls/`)

**Version**: mbedtls-3.6.3

Used in PoC attacks to demonstrate RSA key extraction.

### MCUboot (`mcuboot/`)

Secure bootloader.

### TF-M Tests (`tf-m-tests/`)

Official TF-M test suite used as templates for M-Step evaluation tests.

### Patches (`patches/`)

Custom patches applied to TF-M and Mbed TLS:

| Patch | Purpose |
|-------|---------|
| `001-tfm-toolchain-Os.patch` | Enable `-Os` optimization for fitting on target |
| `002-tfm-mstp-metrics.patch` | Add M-Step metrics collection support |
| `003-mstp-poc.patch` | Enable PoC attack triggers and key printing |