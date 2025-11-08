#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/display_fbdev.h"

// This file provides integration between LVGL and the Waveshare e-paper driver
// The actual display update is now handled in display_fbdev.c

// Note: The Waveshare driver integration has been moved to display_fbdev.c
// This file is kept for future extensions or custom e-paper driver implementations

void epaper_driver_update(const uint8_t *buffer, size_t width, size_t height) {
    // This function is now handled by display_fbdev_flush() in display_fbdev.c
    // which calls EPD_2in13_V4_Display() or EPD_2in13_V4_Display_Fast()
    
    (void)buffer;
    (void)width;
    (void)height;
}

