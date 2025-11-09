#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "sysfs_gpio.h"

int main(int argc, char *argv[]) {
    printf("=== GPIO Test for Radxa Zero 3w ===\n\n");
    
    // Test GPIO pins
    int pins[] = {97, 99, 150, 113};  // RST, DC, CS, BUSY
    const char *names[] = {"RST (GPIO3_A1)", "DC (GPIO3_A3)", "CS (GPIO4_C6)", "BUSY (GPIO3_C1)"};
    
    printf("Testing GPIO pins:\n");
    for (int i = 0; i < 4; i++) {
        printf("\n%d. Testing %s (GPIO %d):\n", i+1, names[i], pins[i]);
        
        // Export
        printf("   Exporting... ");
        if (SYSFS_GPIO_Export(pins[i]) == 0) {
            printf("OK\n");
        } else {
            printf("FAILED (errno: %d, %s)\n", errno, strerror(errno));
            continue;
        }
        usleep(100000);  // Wait 100ms
        
        // Set direction (output for RST, DC, CS; input for BUSY)
        printf("   Setting direction... ");
        if (i == 3) {  // BUSY is input
            if (SYSFS_GPIO_Direction(pins[i], IN) == 0) {
                printf("OK (INPUT)\n");
            } else {
                printf("FAILED (errno: %d, %s)\n", errno, strerror(errno));
            }
        } else {  // RST, DC, CS are outputs
            if (SYSFS_GPIO_Direction(pins[i], OUT) == 0) {
                printf("OK (OUTPUT)\n");
            } else {
                printf("FAILED (errno: %d, %s)\n", errno, strerror(errno));
            }
        }
        usleep(100000);
        
        // Test write (for outputs)
        if (i != 3) {
            printf("   Writing HIGH... ");
            if (SYSFS_GPIO_Write(pins[i], HIGH) == 0) {
                printf("OK\n");
            } else {
                printf("FAILED (errno: %d, %s)\n", errno, strerror(errno));
            }
            usleep(500000);  // 500ms
            
            printf("   Writing LOW... ");
            if (SYSFS_GPIO_Write(pins[i], LOW) == 0) {
                printf("OK\n");
            } else {
                printf("FAILED (errno: %d, %s)\n", errno, strerror(errno));
            }
            usleep(500000);
        } else {
            // Test read (for BUSY input)
            printf("   Reading value... ");
            int value = SYSFS_GPIO_Read(pins[i]);
            if (value >= 0) {
                printf("OK (value = %d)\n", value);
            } else {
                printf("FAILED (errno: %d, %s)\n", errno, strerror(errno));
            }
        }
        
        // Unexport
        printf("   Unexporting... ");
        if (SYSFS_GPIO_Unexport(pins[i]) == 0) {
            printf("OK\n");
        } else {
            printf("FAILED (errno: %d, %s)\n", errno, strerror(errno));
        }
    }
    
    printf("\n=== GPIO Test Complete ===\n");
    printf("If all tests passed, GPIO pins are working.\n");
    printf("If tests failed, check:\n");
    printf("  - GPIO numbers are correct\n");
    printf("  - Run with sudo\n");
    printf("  - GPIO pins are not in use by another driver\n");
    
    return 0;
}

