#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "EPD_2in13_V4.h"
#include "DEV_Config.h"

int main(int argc, char *argv[]) {
    printf("=== Waveshare 2.13\" V4 Display Test ===\n");
    
    // Initialize module
    printf("1. Initializing module...\n");
    if (DEV_Module_Init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize module\n");
        return 1;
    }
    printf("   Module initialized successfully\n");
    
    // Print GPIO pin numbers
    printf("2. GPIO Pin Configuration:\n");
    printf("   RST_PIN = %d\n", EPD_RST_PIN);
    printf("   DC_PIN = %d\n", EPD_DC_PIN);
    printf("   CS_PIN = %d\n", EPD_CS_PIN);
    printf("   BUSY_PIN = %d\n", EPD_BUSY_PIN);
    printf("   PWR_PIN = %d\n", EPD_PWR_PIN);
    
    // Initialize display
    printf("3. Initializing display...\n");
    EPD_2in13_V4_Init();
    printf("   Display initialized\n");
    
    // Clear display (should flash white)
    printf("4. Clearing display (should flash white)...\n");
    EPD_2in13_V4_Clear();
    printf("   Display cleared\n");
    sleep(2);
    
    // Create a simple test pattern
    printf("5. Creating test pattern...\n");
    UBYTE *image = (UBYTE *)malloc(EPD_2in13_V4_WIDTH * EPD_2in13_V4_HEIGHT / 8);
    if (!image) {
        fprintf(stderr, "ERROR: Failed to allocate image buffer\n");
        DEV_Module_Exit();
        return 1;
    }
    
    // Fill with pattern: checkerboard
    memset(image, 0xFF, EPD_2in13_V4_WIDTH * EPD_2in13_V4_HEIGHT / 8);
    
    // Draw a simple pattern - black rectangle in center
    for (int y = 50; y < 150; y++) {
        for (int x = 20; x < 100; x++) {
            int byte_idx = (y * EPD_2in13_V4_WIDTH + x) / 8;
            int bit_idx = 7 - (x % 8);
            image[byte_idx] &= ~(1 << bit_idx);  // Set bit to 0 (black)
        }
    }
    
    // Display the pattern
    printf("6. Displaying test pattern (should see black rectangle)...\n");
    EPD_2in13_V4_Display(image);
    printf("   Pattern displayed\n");
    sleep(3);
    
    // Clear again
    printf("7. Clearing display again...\n");
    EPD_2in13_V4_Clear();
    printf("   Display cleared\n");
    sleep(2);
    
    // Put display to sleep
    printf("8. Putting display to sleep...\n");
    EPD_2in13_V4_Sleep();
    printf("   Display sleeping\n");
    
    // Cleanup
    printf("9. Cleaning up...\n");
    free(image);
    DEV_Module_Exit();
    printf("   Cleanup complete\n");
    
    printf("\n=== Test Complete ===\n");
    printf("If you saw the display flash/change, the hardware is working!\n");
    printf("If you saw nothing, check:\n");
    printf("  - GPIO pin numbers are correct\n");
    printf("  - SPI device exists: ls /dev/spidev*\n");
    printf("  - Run with sudo for GPIO access\n");
    printf("  - Check physical connections\n");
    
    return 0;
}

