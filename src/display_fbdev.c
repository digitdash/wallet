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
static lv_disp_t *display = NULL;
static lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t disp_buf;

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
    
    // Initialize LVGL display (v8.x API)
    // Allocate display buffer
    size_t buf_size = EPD_WIDTH * EPD_HEIGHT * sizeof(lv_color_t);
    void *buf1 = malloc(buf_size);
    if (!buf1) {
        fprintf(stderr, "Error: Failed to allocate display buffer\n");
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
    
    // Initialize display buffer
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, EPD_WIDTH * EPD_HEIGHT);
    
    // Initialize display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EPD_WIDTH;
    disp_drv.ver_res = EPD_HEIGHT;
    disp_drv.flush_cb = display_fbdev_flush;
    disp_drv.draw_buf = &disp_buf;
    
    // Register display driver
    display = lv_disp_drv_register(&disp_drv);
    if (!display) {
        fprintf(stderr, "Error: Failed to register LVGL display\n");
        free(buf1);
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
    
    printf("Display initialized: %dx%d (Waveshare 2.13\" V4)\n", EPD_WIDTH, EPD_HEIGHT);
    return 0;
}

void display_fbdev_deinit(void) {
    if (display) {
        lv_disp_remove(display);
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

void display_fbdev_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    if (!epaper_buffer || !color_p || !area) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    
    // Calculate area dimensions
    int32_t width = lv_area_get_width(area);
    int32_t height = lv_area_get_height(area);
    
    // Convert RGB565 to monochrome for the specified area
    // color_p is the buffer from LVGL containing the area to update
    // We need to convert only the area being updated, not the whole screen
    size_t mono_stride = (EPD_WIDTH + 7) / 8;
    
    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            // Get pixel from LVGL buffer
            lv_color_t pixel = color_p[y * width + x];
            
            // Convert RGB565 to grayscale
            uint16_t rgb565 = pixel.full;
            uint8_t r = (rgb565 >> 11) & 0x1F;
            uint8_t g = (rgb565 >> 5) & 0x3F;
            uint8_t b = rgb565 & 0x1F;
            
            // Convert to 8-bit grayscale
            uint8_t gray = ((r * 8) + (g * 4) + (b * 8)) / 3;
            
            // Threshold to black/white
            uint8_t bit = (gray < 128) ? 1 : 0;
            
            // Calculate position in full screen coordinates
            int32_t screen_x = area->x1 + x;
            int32_t screen_y = area->y1 + y;
            
            // Set bit in monochrome buffer
            size_t mono_idx = screen_y * mono_stride + (screen_x / 8);
            size_t bit_pos = 7 - (screen_x % 8);
            if (bit) {
                epaper_buffer[mono_idx] |= (1 << bit_pos);
            } else {
                epaper_buffer[mono_idx] &= ~(1 << bit_pos);
            }
        }
    }
    
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
    
    lv_disp_flush_ready(disp_drv);
}

uint32_t display_fbdev_get_width(void) {
    return EPD_WIDTH;
}

uint32_t display_fbdev_get_height(void) {
    return EPD_HEIGHT;
}

