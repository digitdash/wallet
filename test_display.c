#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include "EPD_2in13_V4.h"
#include "DEV_Config.h"

int main(int argc, char *argv[]) {
    printf("=== Waveshare 2.13\" V4 Display Test ===\n");
    
    // Initialize module
    printf("1. Initializing module...\n");
    printf("   Attempting DEV_Module_Init()...\n");
    int init_result = DEV_Module_Init();
    printf("   DEV_Module_Init() returned: %d\n", init_result);
    if (init_result != 0) {
        fprintf(stderr, "ERROR: Failed to initialize module (returned %d)\n", init_result);
        fprintf(stderr, "   Check:\n");
        fprintf(stderr, "   - GPIO pins are correct\n");
        fprintf(stderr, "   - Run with sudo\n");
        fprintf(stderr, "   - SPI device exists: ls /dev/spidev*\n");
        return 1;
    }
    printf("   Module initialized successfully\n");
    
    // Print GPIO pin numbers
    printf("2. GPIO Pin Configuration:\n");
    printf("   RST_PIN = %d (GPIO3_A1, physical pin 11)\n", EPD_RST_PIN);
    printf("   DC_PIN = %d (GPIO3_A3, physical pin 12)\n", EPD_DC_PIN);
    printf("   CS_PIN = %d (GPIO4_C6, physical pin 24)\n", EPD_CS_PIN);
    printf("   BUSY_PIN = %d (GPIO3_C1, physical pin 22)\n", EPD_BUSY_PIN);
    printf("   PWR_PIN = %d (not used, physical pin 1 is 3.3V)\n", EPD_PWR_PIN);
    printf("\n");
    printf("   Checking GPIO chip base numbers...\n");
    FILE *fp;
    char line[256];
    for (int i = 0; i < 10; i++) {
        char path[256];
        snprintf(path, sizeof(path), "/sys/class/gpio/gpiochip%d/base", i);
        fp = fopen(path, "r");
        if (fp) {
            if (fgets(line, sizeof(line), fp)) {
                int base = atoi(line);
                snprintf(path, sizeof(path), "/sys/class/gpio/gpiochip%d/label", i);
                FILE *fp2 = fopen(path, "r");
                char label[64] = "unknown";
                if (fp2) {
                    if (fgets(label, sizeof(label), fp2)) {
                        label[strcspn(label, "\n")] = 0;
                    }
                    fclose(fp2);
                }
                printf("   GPIO chip %d: base=%d, label=%s\n", i, base, label);
            }
            fclose(fp);
        }
    }
    printf("\n");
    
    // Initialize display
    printf("3. Initializing display...\n");
    printf("   Calling EPD_2in13_V4_Init()...\n");
    EPD_2in13_V4_Init();
    printf("   EPD_2in13_V4_Init() completed\n");
    printf("   Display initialized\n");
    
    // Clear display (should flash white)
    printf("4. Clearing display (should flash white)...\n");
    printf("   Calling EPD_2in13_V4_Clear()...\n");
    EPD_2in13_V4_Clear();
    printf("   EPD_2in13_V4_Clear() completed\n");
    printf("   Display cleared - you should see a white flash!\n");
    sleep(3);
    
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

