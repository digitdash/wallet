#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    printf("=== SPI Test for /dev/spidev1.0 ===\n\n");
    
    const char *spi_device = "/dev/spidev1.0";
    int fd;
    
    // Open SPI device
    printf("1. Opening SPI device %s...\n", spi_device);
    fd = open(spi_device, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "ERROR: Failed to open %s: %s (errno: %d)\n", 
                spi_device, strerror(errno), errno);
        return 1;
    }
    printf("   SPI device opened successfully (fd=%d)\n", fd);
    
    // Get SPI mode
    uint8_t mode;
    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0) {
        fprintf(stderr, "ERROR: Failed to get SPI mode: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    printf("   Current SPI mode: %d\n", mode);
    
    // Set SPI mode 0
    mode = SPI_MODE_0;
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        fprintf(stderr, "ERROR: Failed to set SPI mode: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    printf("   SPI mode set to: %d (SPI_MODE_0)\n", mode);
    
    // Get bits per word
    uint8_t bits = 8;
    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0) {
        fprintf(stderr, "WARNING: Failed to get bits per word: %s\n", strerror(errno));
    } else {
        printf("   Bits per word: %d\n", bits);
    }
    
    // Set bits per word
    bits = 8;
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        fprintf(stderr, "WARNING: Failed to set bits per word: %s\n", strerror(errno));
    }
    
    // Get max speed
    uint32_t speed = 10000000;  // 10MHz
    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
        fprintf(stderr, "WARNING: Failed to get max speed: %s\n", strerror(errno));
    } else {
        printf("   Max speed: %d Hz\n", speed);
    }
    
    // Set speed
    speed = 10000000;  // 10MHz
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        fprintf(stderr, "WARNING: Failed to set speed: %s\n", strerror(errno));
    } else {
        printf("   Speed set to: %d Hz (10MHz)\n", speed);
    }
    
    // Test SPI transfer
    printf("\n2. Testing SPI transfer...\n");
    uint8_t tx_buf[4] = {0x12, 0x34, 0x56, 0x78};
    uint8_t rx_buf[4] = {0};
    
    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 4,
        .speed_hz = speed,
        .bits_per_word = bits,
        .delay_usecs = 0,
    };
    
    printf("   Sending: 0x%02x 0x%02x 0x%02x 0x%02x\n", 
           tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3]);
    
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        fprintf(stderr, "ERROR: SPI transfer failed: %s (errno: %d)\n", 
                strerror(errno), errno);
        close(fd);
        return 1;
    }
    
    printf("   Received: 0x%02x 0x%02x 0x%02x 0x%02x\n", 
           rx_buf[0], rx_buf[1], rx_buf[2], rx_buf[3]);
    printf("   SPI transfer successful!\n");
    
    // Close SPI device
    printf("\n3. Closing SPI device...\n");
    close(fd);
    printf("   SPI device closed\n");
    
    printf("\n=== SPI Test Complete ===\n");
    printf("If SPI transfer succeeded, SPI communication is working.\n");
    printf("If it failed, check:\n");
    printf("  - SPI device exists: ls /dev/spidev*\n");
    printf("  - Run with sudo\n");
    printf("  - SPI is enabled in device tree\n");
    
    return 0;
}

