# Waveshare 2.13" V4 HAT Setup for Radxa Zero 3w

## Overview

Using the Waveshare 2.13" e-paper HAT simplifies connections by providing a standardized 40-pin GPIO header interface. The HAT is designed for Raspberry Pi GPIO pinout, but can work with Radxa Zero 3w with proper pin mapping.

## HAT Pin Configuration

The Waveshare 2.13" V4 HAT uses the following Raspberry Pi GPIO pins:

| Function | Raspberry Pi BCM | Physical Pin | Notes |
|----------|------------------|--------------|-------|
| RST | GPIO 17 | Pin 11 | Reset |
| DC | GPIO 25 | Pin 22 | Data/Command |
| CS | GPIO 8 (CE0) | Pin 24 | Chip Select |
| BUSY | GPIO 24 | Pin 18 | Busy Signal |
| CLK | GPIO 11 (SCLK) | Pin 23 | SPI Clock |
| MOSI | GPIO 10 (MOSI) | Pin 19 | SPI Data Out |
| 3.3V | 3.3V | Pin 1/17 | Power |
| GND | GND | Pin 6/9/14/20/25/30/34/39 | Ground |

## Radxa Zero 3w Compatibility

The Radxa Zero 3w has a 40-pin GPIO header that is **mostly compatible** with Raspberry Pi pinout, but GPIO numbers may differ. You have two options:

### Option 1: Use Raspberry Pi Pin Numbers (Recommended)

If Radxa Zero 3w's GPIO header matches Raspberry Pi physical pinout, you can use the same physical pins but may need to map GPIO numbers.

### Option 2: Map to Radxa Zero 3w GPIO Numbers

If GPIO numbers differ, you'll need to map them. Check your Radxa Zero 3w pinout diagram.

## Current Configuration

The code is currently configured with custom GPIO pins. For HAT usage, we should use the standard Raspberry Pi HAT pinout:

```c
// Waveshare HAT standard pinout (Raspberry Pi BCM)
EPD_RST_PIN     = 17;   // GPIO 17 (BCM)
EPD_DC_PIN      = 25;   // GPIO 25 (BCM)
EPD_CS_PIN      = 8;    // GPIO 8 / SPI0_CE0 (BCM)
EPD_BUSY_PIN    = 24;   // GPIO 24 (BCM)
```

## Physical Installation

1. **Mount the HAT**:
   - Align the HAT's 40-pin header with Radxa Zero 3w's GPIO header
   - Ensure proper alignment (check pin 1 markers)
   - Press down firmly to seat the HAT

2. **Verify Connections**:
   - HAT should be securely mounted
   - No bent pins
   - Proper power connection (3.3V and GND)

3. **Check Compatibility**:
   - Verify Radxa Zero 3w GPIO header matches Raspberry Pi physical pinout
   - Check if GPIO numbers need mapping

## Software Configuration

### For Raspberry Pi Compatible Pinout

If your Radxa Zero 3w GPIO header matches Raspberry Pi physical pinout, you can use the standard HAT configuration:

```c
#ifdef WAVESHARE_HAT
    // Waveshare HAT standard pinout
    EPD_RST_PIN     = 17;   // GPIO 17
    EPD_DC_PIN      = 25;   // GPIO 25
    EPD_CS_PIN      = 8;    // SPI0_CE0
    EPD_BUSY_PIN    = 24;   // GPIO 24
#endif
```

### For Custom GPIO Mapping

If GPIO numbers differ, create a mapping:

```c
#ifdef RADXA_ZERO_3W_HAT
    // Map HAT pins to Radxa Zero 3w GPIO numbers
    // Physical pin positions match, but GPIO numbers may differ
    EPD_RST_PIN     = <RADXA_GPIO_FOR_PHYSICAL_PIN_11>;
    EPD_DC_PIN      = <RADXA_GPIO_FOR_PHYSICAL_PIN_22>;
    EPD_CS_PIN      = <RADXA_GPIO_FOR_PHYSICAL_PIN_24>;
    EPD_BUSY_PIN    = <RADXA_GPIO_FOR_PHYSICAL_PIN_18>;
#endif
```

## Advantages of Using HAT

✅ **Simplified Connections**: No individual wire connections needed
✅ **Standard Pinout**: Uses Raspberry Pi standard GPIO layout
✅ **Secure Mounting**: HAT mounts directly on GPIO header
✅ **Additional Features**: Some HATs include buttons or other features
✅ **Better Organization**: Cleaner setup than individual wires

## Verification Steps

1. **Check HAT Mounting**:
   ```bash
   # Verify HAT is properly seated
   # Check for any bent pins
   ```

2. **Verify SPI**:
   ```bash
   ls /dev/spidev*
   # Should show /dev/spidev0.0 or similar
   ```

3. **Test GPIO Pins**:
   ```bash
   # Test RST pin (GPIO 17 on RPi, may differ on Radxa)
   echo 17 > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio17/direction
   echo 1 > /sys/class/gpio/gpio17/value
   ```

4. **Run Application**:
   ```bash
   sudo ./wallet_app
   ```

## Troubleshooting

### HAT Not Detected

1. **Check Physical Connection**:
   - Verify HAT is properly seated
   - Check for bent pins
   - Ensure power is connected

2. **Verify Pin Compatibility**:
   - Check Radxa Zero 3w pinout matches Raspberry Pi physical layout
   - Verify GPIO numbers are correct

3. **Check SPI**:
   - Ensure SPI is enabled
   - Verify SPI device exists

### Display Not Working

1. **Verify GPIO Mapping**:
   - Check if GPIO numbers need to be remapped
   - Test individual GPIO pins

2. **Check Power**:
   - Verify 3.3V is present
   - Check power consumption

3. **Test with Waveshare Example**:
   - Use original Waveshare test code first
   - Verify HAT works with standard driver

## Next Steps

1. **Verify Pin Compatibility**: Check if Radxa Zero 3w GPIO header matches Raspberry Pi physical pinout
2. **Update GPIO Configuration**: Modify `DEV_Config.c` to use HAT pinout
3. **Test Display**: Run test code to verify HAT works
4. **Update Documentation**: Document any pin mapping needed

## References

- Waveshare 2.13" V4 HAT datasheet
- Radxa Zero 3w GPIO pinout diagram
- Raspberry Pi GPIO pinout reference

