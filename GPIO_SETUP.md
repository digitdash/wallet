# GPIO Setup Summary - Waveshare HAT

## Current Configuration (Waveshare HAT)

The GPIO pins for Radxa Zero 3w are now configured for the Waveshare 2.13" V4 HAT:

| Pin | Function | GPIO Number (HAT) | Physical Pin | Notes |
|-----|----------|-------------------|--------------|-------|
| RST | Reset | 17 | Pin 11 | Manually controlled |
| DC  | Data/Command | 25 | Pin 22 | Manually controlled |
| CS  | Chip Select | 8 (SPI0_CE0) | Pin 24 | Manually controlled |
| CLK | SPI Clock | 11 (SPI0_SCLK) | Pin 23 | Hardware SPI managed |
| MOSI | SPI Data Out | 10 (SPI0_MOSI) | Pin 19 | Hardware SPI managed |
| BUSY | Busy Signal | 24 | Pin 18 | Input pin |
| PWR | Power (optional) | 18 | Pin 12 | Optional power control |

## HAT Installation

**No individual wire connections needed!** Simply:

1. **Mount the HAT**:
   - Align the HAT's 40-pin header with Radxa Zero 3w's GPIO header
   - Ensure proper alignment (check pin 1 markers)
   - Press down firmly to seat the HAT

2. **Verify Mounting**:
   - HAT should be securely mounted
   - No bent pins
   - Proper power connection (3.3V and GND are on the header)

## Build Configuration

When building the wallet application, the `RADXA_ZERO_3W` define is automatically set:

- **CMake**: Defined in `CMakeLists.txt`
- **Make**: Defined in `Makefile`

This ensures the correct GPIO pins (HAT standard) are used during compilation.

## Physical Connections

**Using HAT - No wiring needed!**

The HAT mounts directly on the GPIO header. The connections are:

```
Waveshare HAT Pins    →    Radxa Zero 3w GPIO Header
─────────────────────────────────────────────────────
All pins connect automatically via 40-pin header
```

**Pin Mapping** (for reference):
- RST → GPIO 17 (physical pin 11)
- DC → GPIO 25 (physical pin 22)
- CS → GPIO 8 (physical pin 24)
- CLK → GPIO 11 (physical pin 23)
- MOSI → GPIO 10 (physical pin 19)
- BUSY → GPIO 24 (physical pin 18)
- 3.3V → 3.3V (physical pin 1/17)
- GND → GND (physical pins 6/9/14/20/25/30/34/39)

## Verification

1. **Check HAT is mounted**:
   - Verify HAT is properly seated on GPIO header
   - Check for any bent pins
   - Ensure secure connection

2. **Check SPI is enabled**:
   ```bash
   ls /dev/spidev*
   # Should show /dev/spidev0.0 or similar
   ```

3. **Test GPIO pins** (as root):
   ```bash
   # Test RST pin (GPIO 17)
   echo 17 > /sys/class/gpio/export
   echo out > /sys/class/gpio/gpio17/direction
   echo 1 > /sys/class/gpio/gpio17/value
   echo 0 > /sys/class/gpio/gpio17/value
   echo 17 > /sys/class/gpio/unexport
   ```

4. **Run the application**:
   ```bash
   sudo ./wallet_app
   ```

## GPIO Number Mapping

**Important**: The HAT uses Raspberry Pi BCM GPIO numbering. If your Radxa Zero 3w uses different GPIO numbers for the same physical pins, you may need to adjust the GPIO values in `DEV_Config.c`.

To check if mapping is needed:
1. Verify Radxa Zero 3w physical pinout matches Raspberry Pi
2. Check if GPIO numbers differ
3. If different, update GPIO values in `DEV_Config.c`

## Troubleshooting

### HAT Not Working

1. **Check HAT mounting**:
   - Verify HAT is properly seated
   - Check for bent pins
   - Ensure all pins are making contact

2. **Verify GPIO compatibility**:
   - Check if Radxa Zero 3w GPIO numbers match Raspberry Pi
   - Test individual GPIO pins
   - May need to remap GPIO numbers

3. **Check SPI**:
   - Ensure SPI is enabled
   - Verify SPI device exists: `ls /dev/spidev*`

### Display Not Initializing

1. **Check power**:
   - Verify 3.3V is present on header
   - Check power consumption

2. **Verify pin mapping**:
   - Test GPIO pins individually
   - Check if GPIO numbers need remapping

3. **Test with Waveshare example**:
   - Use original Waveshare test code
   - Verify HAT works with standard driver

## Advantages of HAT

✅ **No wiring needed** - Just mount on GPIO header
✅ **Standard pinout** - Uses Raspberry Pi compatible layout
✅ **Secure connection** - HAT mounts directly, no loose wires
✅ **Clean setup** - Professional appearance
✅ **Additional features** - Some HATs include buttons or other components

## References

- `HAT_SETUP.md` - Detailed HAT setup guide
- `GPIO_CONFIG.md` - GPIO configuration details
- Waveshare 2.13" V4 HAT datasheet
- Radxa Zero 3w GPIO pinout diagram
