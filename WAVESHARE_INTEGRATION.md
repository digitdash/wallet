# Waveshare Driver Integration - Complete

## Overview

The Waveshare 2.13" e-paper display V4 driver has been fully integrated into the wallet application. The integration is complete and ready for use.

## Integration Details

### Files Modified

1. **`src/display_fbdev.c`**:
   - Added Waveshare driver includes (`EPD_2in13_V4.h`, `DEV_Config.h`)
   - Integrated `DEV_Module_Init()` for GPIO/SPI initialization
   - Integrated `EPD_2in13_V4_Init()` for display initialization
   - Integrated `EPD_2in13_V4_Display()` and `EPD_2in13_V4_Display_Fast()` for updates
   - Added display sleep on deinit (`EPD_2in13_V4_Sleep()`)

### Key Features

1. **Automatic Initialization**:
   - GPIO and SPI are initialized via `DEV_Module_Init()`
   - Display is initialized via `EPD_2in13_V4_Init()`
   - Display is cleared on startup

2. **Smart Update Strategy**:
   - Full quality updates for complete screen refreshes
   - Fast mode for partial updates (every 10th update is full quality to prevent ghosting)
   - Automatic mode switching between fast and full quality

3. **Power Management**:
   - Display is put to sleep on application exit
   - GPIO/SPI resources are properly cleaned up

4. **Error Handling**:
   - Proper error checking at each initialization step
   - Graceful cleanup on errors

## How It Works

### Initialization Flow

```
1. display_fbdev_init()
   ├── DEV_Module_Init()          // Initialize GPIO and SPI
   ├── EPD_2in13_V4_Init()       // Initialize e-paper display
   ├── EPD_2in13_V4_Clear()      // Clear display
   └── Allocate buffers           // Allocate LVGL and e-paper buffers
```

### Update Flow

```
1. LVGL renders to RGB565 buffer
2. display_fbdev_flush() is called
   ├── Convert RGB565 to monochrome (1 bit per pixel)
   ├── Determine if full or partial update
   ├── EPD_2in13_V4_Display() or EPD_2in13_V4_Display_Fast()
   ├── EPD_2in13_V4_ReadBusy()  // Wait for display to finish
   └── Notify LVGL that flush is complete
```

### Deinitialization Flow

```
1. display_fbdev_deinit()
   ├── EPD_2in13_V4_Sleep()      // Put display to sleep
   ├── DEV_Module_Exit()         // Cleanup GPIO/SPI
   └── Free all buffers
```

## Configuration

### GPIO Pins

The GPIO pins are configured in `DEV_Config.c`. For Radxa Zero 3w, you may need to adjust these pins. See `GPIO_CONFIG.md` for details.

### SPI Configuration

- **Device**: `/dev/spidev0.0` (default)
- **Speed**: 10MHz (configurable in `DEV_Config.c`)
- **Mode**: SPI Mode 0 (CPOL=0, CPHA=0)

### Display Settings

- **Resolution**: 122x250 pixels
- **Color Format**: Monochrome (1 bit per pixel)
- **Update Modes**: Full quality and fast mode

## Usage

The integration is automatic. Simply:

1. Call `display_fbdev_init()` to initialize
2. Use LVGL normally - updates are handled automatically
3. Call `display_fbdev_deinit()` on exit

## Performance

- **Full Update**: ~2-3 seconds (high quality, prevents ghosting)
- **Fast Update**: ~0.5-1 second (lower quality, may cause ghosting)
- **Update Strategy**: Full update every 10th update to prevent ghosting

## Troubleshooting

### Display Not Initializing

1. Check GPIO pins are correctly configured
2. Verify SPI is enabled: `ls /dev/spidev*`
3. Check permissions: Run with `sudo` or add user to `gpio` group
4. Verify hardware connections

### Display Not Updating

1. Check BUSY pin is connected correctly
2. Verify SPI communication
3. Check power supply
4. Try full update mode: Set `update_count % 10 == 0` to always true

### Ghosting Issues

1. Increase full refresh frequency (change `update_count % 10` to `update_count % 5`)
2. Always use full quality mode (remove fast mode code)
3. Check display temperature (affects refresh quality)

## Testing

To test the integration:

1. Build the application: `make` or `cmake .. && make`
2. Run with sudo: `sudo ./wallet_app`
3. Verify display initializes and clears
4. Test UI updates

## Next Steps

1. **GPIO Configuration**: Adjust GPIO pins for Radxa Zero 3w (see `GPIO_CONFIG.md`)
2. **SPI Configuration**: Verify SPI device and speed
3. **Button Integration**: Add button input handling
4. **Optimization**: Fine-tune update strategy based on usage patterns

## References

- Waveshare 2.13" V4 Datasheet
- `display_driver/c/lib/e-Paper/EPD_2in13_V4.c` - Driver implementation
- `display_driver/c/lib/Config/DEV_Config.c` - GPIO/SPI configuration
- `INTEGRATION.md` - Original integration guide


