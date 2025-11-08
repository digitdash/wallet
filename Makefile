# Makefile for Monero Hardware Wallet
# Radxa Zero 3w with 2.13" Waveshare e-paper display V4

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = -lm -lpthread -lssl -lcrypto

# Directories
SRC_DIR = src
INCLUDE_DIR = include
DRIVERS_DIR = drivers
AUTH_DIR = auth
DISPLAY_DRIVER_DIR = ../display_driver/c

# Include paths
INCLUDES = -I$(INCLUDE_DIR) \
           -I$(DRIVERS_DIR) \
           -I$(AUTH_DIR) \
           -I$(DISPLAY_DRIVER_DIR)/lib/Config \
           -I$(DISPLAY_DRIVER_DIR)/lib/e-Paper \
           -I$(DISPLAY_DRIVER_DIR)/lib/GUI \
           -I/usr/include/lvgl \
           -I.

# LVGL library (adjust path as needed)
LVGL_LIBS = -llvgl

# Source files
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/wallet_ui.c \
          $(SRC_DIR)/display_fbdev.c \
          $(DRIVERS_DIR)/epaper_driver.c \
          $(DRIVERS_DIR)/gpio_driver.c \
          $(AUTH_DIR)/device_binding.c \
          $(AUTH_DIR)/tropic_auth.c

# Display driver sources (Waveshare)
DISPLAY_DRIVER_SOURCES = $(DISPLAY_DRIVER_DIR)/lib/e-Paper/EPD_2in13_V4.c \
                         $(DISPLAY_DRIVER_DIR)/lib/Config/DEV_Config.c \
                         $(DISPLAY_DRIVER_DIR)/lib/Config/sysfs_gpio.c \
                         $(DISPLAY_DRIVER_DIR)/lib/Config/sysfs_software_spi.c \
                         $(DISPLAY_DRIVER_DIR)/lib/GUI/GUI_Paint.c

# Object files
OBJECTS = $(SOURCES:.c=.o) $(DISPLAY_DRIVER_SOURCES:.c=.o)

# Target executable
TARGET = wallet_app

# Compiler definitions
DEFINES = -DEPD=epd2in13V4 \
          -DUSE_DEV_LIB \
          -DRADXA_ZERO_3W \
          -DLV_CONF_INCLUDE_SIMPLE

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(LVGL_LIBS)
	@echo "Build complete: $(TARGET)"

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete"

# Install (requires root)
install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/
	@echo "Installed $(TARGET) to /usr/local/bin"

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstalled $(TARGET)"

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the wallet application (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  install   - Install to /usr/local/bin (requires root)"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help message"

.PHONY: all clean install uninstall help

