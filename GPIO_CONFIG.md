# GPIO Configuration for Waveshare 2.13" E-Paper Display V4 HAT

## GPIO Pin Configuration

**Using Waveshare HAT**: The configuration uses the standard Waveshare HAT pinout (Raspberry Pi compatible). The HAT mounts directly on the 40-pin GPIO header, simplifying connections.

The Waveshare driver requires the following GPIO pins to be configured. The default configuration uses the HAT standard pinout.

### Required Pins

| Pin | Function | Direction | GPIO Pin | Physical Pin |
|-----|----------|-----------|-------------------------|--------------|
| RST | Reset | Output | GPIO3_A1 | Pin 11 |
| DC  | Data/Command | Output | GPIO3_A3 | Pin 12 |
| CS  | Chip Select | Output | GPIO4_C6 (SPI3_CS0_M1) | Pin 24 |
| CLK | SPI Clock | Output | GPIO4_C2 (SPI3_CLK_M1) | Pin 23 |
| MOSI | SPI Data Out | Output | GPIO4_C3(SPI3_MOSI_M1) | Pin 19 |
| BUSY | Busy Signal | Input | GPIO3_C1 | Pin 22 |
| PWR | Power (optional) | Output | +3.3v | Pin 1 | 

### Radxa Zero 3w Configuration

For Radxa Zero 3w, you need to:

1. **Identify available GPIO pins**:
   ```bash
   # List available GPIO pins
   ls /sys/class/gpio/
   ```

2. **GPIO pins are configured** in `display_driver/c/lib/Config/DEV_Config.c`:
   ```c
   #elif RADXA_ZERO_3W
       // Radxa Zero 3w GPIO configuration for Waveshare 2.13" V4 HAT
       // Using Waveshare HAT standard pinout (Raspberry Pi compatible)
       EPD_RST_PIN     = 17;   // GPIO 17 (HAT standard, physical pin 11)
       EPD_DC_PIN      = 25;   // GPIO 25 (HAT standard, physical pin 22)
       EPD_CS_PIN      = 8;    // GPIO 8 / SPI0_CE0 (HAT standard, physical pin 24)
       EPD_BUSY_PIN    = 24;   // GPIO 24 (HAT standard, physical pin 18)
       EPD_PWR_PIN     = 18;   // GPIO 18 (optional, physical pin 12)
   #endif
   ```
   
   **Note**: The GPIO pins use the Waveshare HAT standard pinout. If your Radxa Zero 3w GPIO numbers differ from Raspberry Pi, you may need to adjust these values. The physical pin positions should match Raspberry Pi layout.

3. **SPI Configuration**:
   - Ensure SPI is enabled in device tree
   - Configure SPI speed (10MHz as configured in code)
   - Set SPI mode (Mode 0 for Waveshare)
   - **CLK and MOSI pins**: 
     - **Physical connection**: YES - You MUST physically connect CLK and MOSI wires from the display to Radxa Zero 3w SPI pins
     - **Software configuration**: NO - These are automatically managed by the hardware SPI driver (`/dev/spidev0.0`)
     - CLK: Connect to SPI hardware pin (typically GPIO 23 / SPI0_SCLK on Radxa Zero 3w)
     - MOSI: Connect to SPI hardware pin (typically GPIO 19 / SPI0_MOSI on Radxa Zero 3w)
   - **Note**: If using software SPI, you would need to configure CLK and MOSI as GPIO pins in software, but with hardware SPI you only need the physical connections.

### Current Configuration (Waveshare HAT Standard)

The GPIO pins are configured in `display_driver/c/lib/Config/DEV_Config.c` using the Waveshare HAT standard pinout:

- **RST**: GPIO 17 (physical pin 11)
- **DC**: GPIO 25 (physical pin 22)
- **CS**: GPIO 8 / SPI0_CE0 (physical pin 24)
- **CLK**: GPIO 11 / SPI0_SCLK (physical pin 23) - Hardware SPI managed
- **MOSI**: GPIO 10 / SPI0_MOSI (physical pin 19) - Hardware SPI managed
- **BUSY**: GPIO 24 (physical pin 18)
- **PWR**: GPIO 18 (physical pin 12, optional)

**Important Notes**:
- **HAT Mounting**: The HAT mounts directly on the 40-pin GPIO header - no individual wire connections needed!
- **Physical Pinout**: Uses Raspberry Pi compatible physical pin positions
- **GPIO Numbers**: Uses Raspberry Pi BCM GPIO numbering. If Radxa Zero 3w GPIO numbers differ, you may need to map them.
- **CLK and MOSI**: These are handled automatically by the Linux SPI driver when using hardware SPI (`/dev/spidev0.0`).
- **CS (Chip Select)**: Configured as GPIO 8 because it's manually controlled by the driver.
- To change these pins, edit the `DEV_GPIO_Init()` function in `DEV_Config.c` under the `#elif RADXA_ZERO_3W` section.

A header file `wallet/include/gpio_config.h` is also available for reference, but the actual configuration is in `DEV_Config.c`.

### Testing GPIO Pins

1. **Export GPIO pin**:
   ```bash
   echo <GPIO_NUMBER> > /sys/class/gpio/export
   ```

2. **Set direction**:
   ```bash
   echo out > /sys/class/gpio/gpio<GPIO_NUMBER>/direction
   ```

3. **Test write**:
   ```bash
   echo 1 > /sys/class/gpio/gpio<GPIO_NUMBER>/value
   echo 0 > /sys/class/gpio/gpio<GPIO_NUMBER>/value
   ```

4. **Unexport**:
   ```bash
   echo <GPIO_NUMBER> > /sys/class/gpio/unexport
   ```

### SPI Configuration

1. **Enable SPI**:
   ```bash
   # Check if SPI is enabled
   ls /dev/spidev*
   ```

2. **Set SPI speed** (if needed):
   ```bash
   # Check current SPI speed
   cat /sys/bus/spi/devices/spi0.0/speed
   ```

3. **SPI Mode**: The Waveshare driver uses SPI Mode 0 (CPOL=0, CPHA=0)

### Troubleshooting

1. **GPIO not accessible**:
   - Check permissions: `ls -l /sys/class/gpio/`
   - Run with sudo or add user to gpio group
   - Verify GPIO is not used by another driver

2. **SPI not working**:
   - Check SPI is enabled in device tree
   - Verify SPI device exists: `ls /dev/spidev*`
   - Check SPI permissions

3. **Display not responding**:
   - Verify GPIO connections
   - Check power supply
   - Test with Waveshare example code first

### References

- Radxa Zero 3w GPIO documentation
- Waveshare 2.13" V4 datasheet
- Linux GPIO sysfs interface

