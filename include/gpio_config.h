#ifndef GPIO_CONFIG_H
#define GPIO_CONFIG_H

/**
 * GPIO Configuration for Radxa Zero 3w
 * Waveshare 2.13" E-Paper Display V4 HAT
 * 
 * Using Waveshare HAT standard pinout (Raspberry Pi compatible)
 * Physical pin positions match Raspberry Pi, but GPIO numbers may need mapping
 */

// Waveshare HAT standard pinout (Raspberry Pi BCM numbering)
// If Radxa Zero 3w GPIO numbers differ, adjust these values
#define RADXA_EPD_RST_PIN     17   // GPIO 17 (HAT standard, physical pin 11)
#define RADXA_EPD_DC_PIN      25   // GPIO 25 (HAT standard, physical pin 22)
#define RADXA_EPD_CS_PIN      8    // GPIO 8 / SPI0_CE0 (HAT standard, physical pin 24)
#define RADXA_EPD_BUSY_PIN    24   // GPIO 24 (HAT standard, physical pin 18)
#define RADXA_EPD_PWR_PIN     18   // GPIO 18 (optional, physical pin 12)

#endif // GPIO_CONFIG_H


