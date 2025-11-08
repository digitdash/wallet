#include "display_fbdev.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <string.h>
#include <errno.h>

// Waveshare e-paper driver includes
#include "EPD_2in13_V4.h"
#include "DEV_Config.h"

// E-paper display dimensions (2.13" V4)
#define EPD_WIDTH  122
#define EPD_HEIGHT 250

static int fb_fd = -1;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static uint8_t *fb_mem = NULL;
static size_t fb_size = 0;
static lv_display_t *display = NULL;

// For e-paper, we need to convert RGB to monochrome
static uint8_t *epaper_buffer = NULL;
static bool waveshare_initialized = false;
static bool use_fast_mode = false;
static uint32_t update_count = 0;

/**
 * Convert RGB565 to monochrome (1 bit per pixel)
 */
static void rgb_to_monochrome(const uint8_t *rgb, uint8_t *mono, size_t width, size_t height) {
    size_t mono_stride = (width + 7) / 8;
    
    for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
            // Extract RGB565 pixel
            size_t rgb_idx = (y * width + x) * 2;
            uint16_t pixel = rgb[rgb_idx] | (rgb[rgb_idx + 1] << 8);
            
            // Convert to grayscale
            uint8_t r = (pixel >> 11) & 0x1F;
            uint8_t g = (pixel >> 5) & 0x3F;
            uint8_t b = pixel & 0x1F;
            
            // Convert to 8-bit grayscale
            uint8_t gray = ((r * 8) + (g * 4) + (b * 8)) / 3;
            
            // Threshold to black/white
            uint8_t bit = (gray < 128) ? 1 : 0;
            
            // Set bit in monochrome buffer
            size_t mono_idx = y * mono_stride + (x / 8);
            size_t bit_pos = 7 - (x % 8);
            if (bit) {
                mono[mono_idx] |= (1 << bit_pos);
            } else {
                mono[mono_idx] &= ~(1 << bit_pos);
            }
        }
    }
}

int display_fbdev_init(void) {
    // Initialize Waveshare driver first
    printf("Initializing Waveshare e-paper driver...\n");
    if (DEV_Module_Init() != 0) {
        fprintf(stderr, "Error: Failed to initialize Waveshare driver\n");
        return -1;
    }
    
    // Initialize e-paper display
    printf("Initializing e-paper display...\n");
    EPD_2in13_V4_Init();
    EPD_2in13_V4_Clear();
    waveshare_initialized = true;
    update_count = 0;
    
    // Allocate e-paper buffer (monochrome)
    // Format: (width + 7) / 8 bytes per row, height rows
    size_t epaper_buf_size = ((EPD_WIDTH + 7) / 8) * EPD_HEIGHT;
    epaper_buffer = (uint8_t *)malloc(epaper_buf_size);
    if (!epaper_buffer) {
        fprintf(stderr, "Error: Failed to allocate e-paper buffer\n");
        DEV_Module_Exit();
        return -1;
    }
    
    // Clear buffer (white)
    memset(epaper_buffer, 0xFF, epaper_buf_size);
    
    // Optional: Try to open framebuffer for debugging/fallback
    const char *fbdev_path = getenv("LV_LINUX_FBDEV_DEVICE");
    if (!fbdev_path) {
        fbdev_path = "/dev/fb0";
    }
    
    fb_fd = open(fbdev_path, O_RDWR);
    if (fb_fd >= 0) {
        // Get framebuffer info for debugging
        if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo) == 0 &&
            ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo) == 0) {
            printf("Framebuffer available: %dx%d, %dbpp\n",
                   vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
            
            // Memory map the framebuffer (for debugging)
            fb_size = finfo.smem_len;
            fb_mem = (uint8_t *)mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
            if (fb_mem == MAP_FAILED) {
                fb_mem = NULL;
            }
        } else {
            close(fb_fd);
            fb_fd = -1;
        }
    }
    
    // Initialize LVGL display
    display = lv_display_create(EPD_WIDTH, EPD_HEIGHT);
    if (!display) {
        fprintf(stderr, "Error: Failed to create LVGL display\n");
        free(epaper_buffer);
        if (waveshare_initialized) {
            DEV_Module_Exit();
        }
        if (fb_mem) {
            munmap(fb_mem, fb_size);
        }
        if (fb_fd >= 0) {
            close(fb_fd);
        }
        return -1;
    }
    
    // Set flush callback
    lv_display_set_flush_cb(display, display_fbdev_flush);
    
    // Set color format (RGB565, will be converted to monochrome)
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    
    // Allocate display buffer
    size_t buf_size = EPD_WIDTH * EPD_HEIGHT * sizeof(lv_color_t);
    void *buf1 = malloc(buf_size);
    if (!buf1) {
        fprintf(stderr, "Error: Failed to allocate display buffer\n");
        lv_display_delete(display);
        free(epaper_buffer);
        if (waveshare_initialized) {
            DEV_Module_Exit();
        }
        if (fb_mem) {
            munmap(fb_mem, fb_size);
        }
        if (fb_fd >= 0) {
            close(fb_fd);
        }
        return -1;
    }
    
    lv_display_set_buffers(display, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    printf("Display initialized: %dx%d (Waveshare 2.13\" V4)\n", EPD_WIDTH, EPD_HEIGHT);
    return 0;
}

void display_fbdev_deinit(void) {
    if (display) {
        lv_display_delete(display);
        display = NULL;
    }
    
    // Put e-paper display to sleep
    if (waveshare_initialized) {
        printf("Putting e-paper display to sleep...\n");
        EPD_2in13_V4_Sleep();
        DEV_Module_Exit();
        waveshare_initialized = false;
    }
    
    if (epaper_buffer) {
        free(epaper_buffer);
        epaper_buffer = NULL;
    }
    
    if (fb_mem != MAP_FAILED && fb_mem != NULL) {
        munmap(fb_mem, fb_size);
        fb_mem = NULL;
    }
    
    if (fb_fd >= 0) {
        close(fb_fd);
        fb_fd = -1;
    }
}

void display_fbdev_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    // Get the buffer from LVGL
    lv_draw_buf_t *draw_buf = lv_display_get_draw_buf(disp);
    if (!draw_buf || !epaper_buffer) {
        lv_display_flush_ready(disp);
        return;
    }
    
    // Convert RGB565 to monochrome
    rgb_to_monochrome((uint8_t *)draw_buf->buf, epaper_buffer, EPD_WIDTH, EPD_HEIGHT);
    
    // Optional: Write to framebuffer for debugging (if available)
    if (fb_mem && fb_fd >= 0) {
        size_t fb_line_size = finfo.line_length;
        size_t epaper_line_size = (EPD_WIDTH + 7) / 8;
        
        for (int y = 0; y < EPD_HEIGHT && y < (int)vinfo.yres; y++) {
            memcpy(fb_mem + y * fb_line_size, 
                   epaper_buffer + y * epaper_line_size,
                   epaper_line_size < fb_line_size ? epaper_line_size : fb_line_size);
        }
    }
    
    // Update e-paper display using Waveshare driver
    if (waveshare_initialized) {
        // Determine if this is a full screen update
        bool is_full_update = (area->x1 == 0 && area->y1 == 0 && 
                               area->x2 == EPD_WIDTH - 1 && area->y2 == EPD_HEIGHT - 1);
        
        // Use full refresh every 10 updates to prevent ghosting
        // Use fast mode for partial updates, full mode for complete refreshes
        if (is_full_update || (update_count % 10 == 0)) {
            // Full quality update
            EPD_2in13_V4_Display(epaper_buffer);
        } else {
            // Fast partial update
            if (!use_fast_mode) {
                EPD_2in13_V4_Init_Fast();
                use_fast_mode = true;
            }
            EPD_2in13_V4_Display_Fast(epaper_buffer);
        }
        
        // Wait for display to finish updating
        EPD_2in13_V4_ReadBusy();
        
        update_count++;
    }
    
    lv_display_flush_ready(disp);
}

uint32_t display_fbdev_get_width(void) {
    return EPD_WIDTH;
}

uint32_t display_fbdev_get_height(void) {
    return EPD_HEIGHT;
}

