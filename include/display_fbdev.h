#ifndef DISPLAY_FBDEV_H
#define DISPLAY_FBDEV_H

#include <lvgl.h>

/**
 * Initialize LVGL with FBDEV backend
 * @return 0 on success, negative on error
 */
int display_fbdev_init(void);

/**
 * Deinitialize display
 */
void display_fbdev_deinit(void);

/**
 * Flush display buffer (called by LVGL)
 * @param disp_drv Display driver
 * @param area Area to flush
 * @param color_p Buffer containing pixel data
 */
void display_fbdev_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);

/**
 * Get display width
 * @return Display width in pixels
 */
uint32_t display_fbdev_get_width(void);

/**
 * Get display height
 * @return Display height in pixels
 */
uint32_t display_fbdev_get_height(void);

#endif // DISPLAY_FBDEV_H

