#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

// GPIO driver for button inputs
// This provides simple GPIO access for the three wallet buttons

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_DIR_FMT "/sys/class/gpio/gpio%d/direction"
#define GPIO_VALUE_FMT "/sys/class/gpio/gpio%d/value"

static int gpio_export(int gpio) {
    int fd = open(GPIO_EXPORT, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, strlen(buf));
    close(fd);
    
    // Wait for GPIO to be available
    usleep(100000);
    return 0;
}

static int gpio_unexport(int gpio) {
    int fd = open(GPIO_UNEXPORT, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", gpio);
    write(fd, buf, strlen(buf));
    close(fd);
    return 0;
}

static int gpio_set_direction(int gpio, const char *direction) {
    char path[64];
    snprintf(path, sizeof(path), GPIO_DIR_FMT, gpio);
    
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        return -1;
    }
    
    write(fd, direction, strlen(direction));
    close(fd);
    return 0;
}

int gpio_read(int gpio) {
    char path[64];
    snprintf(path, sizeof(path), GPIO_VALUE_FMT, gpio);
    
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    
    char value[3];
    read(fd, value, sizeof(value));
    close(fd);
    
    return (value[0] == '1') ? 1 : 0;
}

int gpio_init_button(int gpio) {
    if (gpio_export(gpio) < 0) {
        return -1;
    }
    
    if (gpio_set_direction(gpio, "in") < 0) {
        gpio_unexport(gpio);
        return -1;
    }
    
    return 0;
}

void gpio_deinit_button(int gpio) {
    gpio_unexport(gpio);
}

