# SPI Pin Configuration for Radxa Zero 3w

## Overview

The Waveshare 2.13" e-paper display uses SPI communication. For Radxa Zero 3w, we're using **hardware SPI** which means:

**IMPORTANT**: You MUST physically connect CLK and MOSI wires from the display to the Radxa Zero 3w SPI pins. The difference is:
- **Physical connection**: YES - Required! Connect the wires.
- **Software GPIO configuration**: NO - Not needed because the Linux SPI driver automatically manages these pins.

## Hardware SPI vs Software SPI

### Hardware SPI (Current Configuration)

When using hardware SPI (`/dev/spidev0.0`):
- **CLK (Clock)**: Automatically managed by SPI hardware
- **MOSI (Data Out)**: Automatically managed by SPI hardware
- **CS (Chip Select)**: Manually controlled via GPIO 24
- **MISO (Data In)**: Not needed for e-paper display (read-only)

**Advantages**:
- Faster communication
- Lower CPU usage
- Automatic clock generation
- No need to configure CLK/MOSI as GPIO pins in software (but you still need to physically connect them!)

### Software SPI (Alternative)

If you need to use software SPI (bit-banging), you would need to:
- Configure CLK as a GPIO pin
- Configure MOSI as a GPIO pin
- Manually toggle these pins in software

**Note**: We're using hardware SPI, so this is not needed.

## Radxa Zero 3w SPI Pin Mapping

The hardware SPI pins on Radxa Zero 3w are typically:

| SPI Function | Physical Pin | GPIO Number | Notes |
|--------------|--------------|-------------|-------|
| SPI0_SCLK | Pin 23 | GPIO 23 | Clock signal |
| SPI0_MOSI | Pin 19 | GPIO 19 | Master Out Slave In |
| SPI0_MISO | Pin 21 | GPIO 21 | Master In Slave Out (not used) |
| SPI0_CS0 | Pin 24 | GPIO 24 | Chip Select 0 (manually controlled) |

**Important**: Check your Radxa Zero 3w pinout diagram to confirm these pin numbers, as they may vary depending on the board revision.

## Current Configuration

In our setup:
- **Hardware SPI device**: `/dev/spidev0.0`
- **SPI speed**: 10MHz (configured in `DEV_Config.c`)
- **SPI mode**: Mode 0 (CPOL=0, CPHA=0)
- **CS control**: Manual via GPIO 24

## Verification

1. **Check SPI device exists**:
   ```bash
   ls -l /dev/spidev*
   # Should show /dev/spidev0.0
   ```

2. **Check SPI pins are available**:
   ```bash
   # Check if SPI is enabled in device tree
   cat /sys/kernel/debug/pinctrl/*/pinmux-pins | grep spi
   ```

3. **Test SPI communication**:
   ```bash
   # Install spidev-test if available
   spidev_test -D /dev/spidev0.0 -s 10000000
   ```

## Troubleshooting

### SPI Device Not Found

1. **Enable SPI in device tree**:
   - Edit device tree overlay
   - Enable SPI0
   - Rebuild and flash device tree

2. **Check kernel modules**:
   ```bash
   lsmod | grep spi
   # Should show spi_rockchip or similar
   ```

3. **Check dmesg**:
   ```bash
   dmesg | grep spi
   # Look for SPI initialization messages
   ```

### SPI Communication Issues

1. **Check pin connections**:
   - Verify CLK is connected to SPI0_SCLK
   - Verify MOSI is connected to SPI0_MOSI
   - Verify CS is connected to GPIO 24

2. **Check SPI speed**:
   - Current: 10MHz
   - Can be reduced if needed: Edit `DEV_HARDWARE_SPI_setSpeed()` in `DEV_Config.c`

3. **Verify SPI mode**:
   - Must be Mode 0 for Waveshare displays
   - Checked in `DEV_HARDWARE_SPI_begin()`

## References

- Radxa Zero 3w pinout diagram
- Linux SPI driver documentation: `/usr/src/linux/Documentation/spi/`
- Waveshare 2.13" V4 datasheet

