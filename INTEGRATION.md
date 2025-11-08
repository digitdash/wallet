# Integration Guide: LVGL with Waveshare E-Paper Display

## Overview

This document describes how to integrate LVGL with the 2.13" Waveshare e-paper display V4 using the FBDEV backend. The integration bridges LVGL's framebuffer interface with the Waveshare driver's SPI interface.

## Architecture

```
LVGL (Framebuffer) → FBDEV Backend → E-Paper Driver → SPI → Display
```

## Current Implementation

The current implementation (`display_fbdev.c`) uses the Linux framebuffer device (`/dev/fb0`) as an intermediate buffer. For full e-paper support, you need to integrate the Waveshare driver.

## Integration Steps

### 1. Initialize Waveshare Driver

In `display_fbdev_init()`, add:

```c
#include "EPD_2in13_V4.h"
#include "DEV_Config.h"

// Initialize GPIO and SPI
if (DEV_Module_Init() != 0) {
    fprintf(stderr, "Failed to initialize Waveshare driver\n");
    return -1;
}

// Initialize e-paper display
EPD_2in13_V4_Init();
```

### 2. Update Display Flush Function

Modify `display_fbdev_flush()` to use the Waveshare driver:

```c
void display_fbdev_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    // Get the buffer from LVGL
    lv_draw_buf_t *draw_buf = lv_display_get_draw_buf(disp);
    if (!draw_buf) {
        lv_display_flush_ready(disp);
        return;
    }
    
    // Convert RGB565 to monochrome
    rgb_to_monochrome((uint8_t *)draw_buf->buf, epaper_buffer, EPD_WIDTH, EPD_HEIGHT);
    
    // Use Waveshare driver to update display
    EPD_2in13_V4_Display(epaper_buffer);
    
    lv_display_flush_ready(disp);
}
```

### 3. Optimize for Partial Updates

For better performance, use partial updates when possible:

```c
void display_fbdev_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    // Check if this is a full screen update
    if (area->x1 == 0 && area->y1 == 0 && 
        area->x2 == EPD_WIDTH - 1 && area->y2 == EPD_HEIGHT - 1) {
        // Full screen update
        EPD_2in13_V4_Display(epaper_buffer);
    } else {
        // Partial update
        EPD_2in13_V4_Display_Partial(epaper_buffer);
    }
    
    lv_display_flush_ready(disp);
}
```

### 4. Handle E-Paper Refresh Timing

E-paper displays have specific refresh requirements:

```c
// Wait for display to finish updating
EPD_2in13_V4_ReadBusy();

// For fast updates, use fast mode
EPD_2in13_V4_Init_Fast();
EPD_2in13_V4_Display_Fast(epaper_buffer);
```

## GPIO Configuration

The Waveshare driver requires GPIO pins for:
- RST (Reset)
- DC (Data/Command)
- CS (Chip Select)
- BUSY (Busy signal)

Configure these in `DEV_Config.c` or via device tree overlay.

## SPI Configuration

The display uses SPI for communication. Ensure SPI is enabled and configured:

```bash
# Enable SPI in device tree
# Or use sysfs SPI interface
```

## Display Refresh Strategy

E-paper displays have limited refresh rates. Consider:

1. **Full Refresh**: Use for major UI changes (every 30-60 seconds)
2. **Partial Refresh**: Use for small updates (text changes, etc.)
3. **Fast Mode**: Use for rapid updates (may reduce quality)

## Performance Considerations

1. **Buffer Management**: Keep display buffer in memory
2. **Update Frequency**: Limit updates to avoid display wear
3. **Power Management**: Put display to sleep when not in use

## Example: Complete Integration

```c
int display_fbdev_init(void) {
    // Initialize Waveshare driver
    if (DEV_Module_Init() != 0) {
        return -1;
    }
    
    // Initialize e-paper display
    EPD_2in13_V4_Init();
    EPD_2in13_V4_Clear();
    
    // Allocate e-paper buffer
    size_t epaper_buf_size = ((EPD_WIDTH + 7) / 8) * EPD_HEIGHT;
    epaper_buffer = (uint8_t *)malloc(epaper_buf_size);
    if (!epaper_buffer) {
        return -1;
    }
    
    // Initialize LVGL display
    display = lv_display_create(EPD_WIDTH, EPD_HEIGHT);
    lv_display_set_flush_cb(display, display_fbdev_flush);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    
    // Allocate LVGL buffer
    size_t buf_size = EPD_WIDTH * EPD_HEIGHT * sizeof(lv_color_t);
    void *buf1 = malloc(buf_size);
    lv_display_set_buffers(display, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    return 0;
}

void display_fbdev_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    // Convert RGB565 to monochrome
    rgb_to_monochrome((uint8_t *)draw_buf->buf, epaper_buffer, EPD_WIDTH, EPD_HEIGHT);
    
    // Update e-paper display
    EPD_2in13_V4_Display(epaper_buffer);
    EPD_2in13_V4_ReadBusy();
    
    lv_display_flush_ready(disp);
}

void display_fbdev_deinit(void) {
    // Put display to sleep
    EPD_2in13_V4_Sleep();
    
    // Cleanup Waveshare driver
    DEV_Module_Exit();
    
    // Cleanup LVGL
    if (display) {
        lv_display_delete(display);
    }
    
    // Free buffers
    if (epaper_buffer) {
        free(epaper_buffer);
    }
}
```

## Testing

1. **Basic Display Test**: Verify display initializes and clears
2. **LVGL Test**: Create simple UI elements and verify they render
3. **Update Test**: Test partial and full updates
4. **Performance Test**: Measure update times and power consumption

## Troubleshooting

### Display Not Updating

- Check SPI connection
- Verify GPIO pins are configured correctly
- Check BUSY signal timing
- Ensure display is not in sleep mode

### Artifacts or Ghosting

- Use full refresh periodically
- Adjust refresh timing
- Check power supply stability

### Slow Updates

- Use fast mode for non-critical updates
- Optimize buffer conversion
- Consider partial updates

## References

- Waveshare 2.13" V4 Datasheet
- LVGL Documentation: https://docs.lvgl.io/
- LVGL Linux Port: https://deepwiki.com/lvgl/lv_port_linux/

