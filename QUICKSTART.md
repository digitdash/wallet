# Quick Start Guide

## Overview

This guide will help you get started with the Monero hardware wallet implementation on Radxa Zero 3w.

## Prerequisites

1. **Hardware**:
   - Radxa Zero 3w running Armbian
   - 2.13" Waveshare e-paper display V4
   - Tropic01 secure element
   - Three buttons for user input

2. **Software**:
   - Armbian Linux (on eMMC)
   - Build tools (gcc, make, cmake)
   - LVGL library
   - OpenSSL development libraries

## Installation

### 1. Install Dependencies

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    liblvgl-dev \
    libssl-dev \
    git
```

### 2. Clone/Setup Repository

```bash
cd /path/to/nexus
cd wallet
```

### 3. Build

#### Using CMake (Recommended)

```bash
mkdir build && cd build
cmake ..
make
```

#### Using Make

```bash
make
```

### 4. Set Permissions

```bash
# Option 1: Run as root
sudo ./wallet_app

# Option 2: Add user to video group
sudo usermod -a -G video $USER
# Log out and back in, then:
./wallet_app
```

## Configuration

### Display Configuration

The display is configured for 2.13" Waveshare e-paper V4:
- Resolution: 122x250 pixels
- Monochrome (black/white)
- FBDEV backend

### Framebuffer Device

Default: `/dev/fb0`

To use a different device:
```bash
export LV_LINUX_FBDEV_DEVICE=/dev/fb1
./wallet_app
```

### GPIO Configuration

Configure GPIO pins for e-paper display in device tree or via sysfs:
- RST (Reset)
- DC (Data/Command)
- CS (Chip Select)
- BUSY (Busy signal)

## Running

```bash
# Basic run
sudo ./wallet_app

# With custom framebuffer
sudo LV_LINUX_FBDEV_DEVICE=/dev/fb1 ./wallet_app
```

## Next Steps

### 1. Integrate Waveshare Driver

See `INTEGRATION.md` for complete instructions. Summary:

```c
// In display_fbdev_init()
#include "EPD_2in13_V4.h"
DEV_Module_Init();
EPD_2in13_V4_Init();

// In display_fbdev_flush()
EPD_2in13_V4_Display(epaper_buffer);
```

### 2. Integrate Tropic01

Replace placeholder code in `auth/tropic_auth.c` with actual libtropic-linux calls:

```c
// Initialize Tropic01
tropic_handle_t *handle = tropic_open();

// Generate device certificate
tropic_cert_t *cert = tropic_generate_device_cert(handle);

// Sign data
tropic_signature_t *sig = tropic_sign(handle, data, data_size);
```

### 3. Add Button Input

Implement button handling in `drivers/gpio_driver.c`:

```c
// Initialize buttons
gpio_init_button(BUTTON_1_GPIO);
gpio_init_button(BUTTON_2_GPIO);
gpio_init_button(BUTTON_3_GPIO);

// Read button state
int state = gpio_read(BUTTON_1_GPIO);
```

### 4. Implement Authentication

See `AUTHENTICATION.md` for authentication flow:

```c
// Initialize device binding
device_binding_t binding;
device_binding_init(&binding);

// Create binding with base station
device_binding_create(&binding, "base_station_id", binding_key);

// Get service token
uint8_t token[32];
device_binding_get_service_token(&binding, "monerod", token, sizeof(token));
```

## Troubleshooting

### Display Not Working

1. Check framebuffer device:
   ```bash
   ls -l /dev/fb*
   ```

2. Test framebuffer:
   ```bash
   cat /dev/fb0 > /tmp/fb.raw
   ```

3. Check permissions:
   ```bash
   ls -l /dev/fb0
   sudo chmod 666 /dev/fb0  # Temporary fix
   ```

### LVGL Not Found

1. Install LVGL:
   ```bash
   sudo apt-get install liblvgl-dev
   ```

2. Or build from source:
   ```bash
   git clone https://github.com/lvgl/lvgl.git
   cd lvgl && make
   ```

### Tropic01 Not Found

1. Install libtropic-linux:
   ```bash
   git clone https://github.com/tropicsquare/libtropic-linux.git
   cd libtropic-linux && make && sudo make install
   ```

2. Update CMakeLists.txt or Makefile to link against libtropic

## Documentation

- **README.md**: Project overview
- **BUILD.md**: Detailed build instructions
- **AUTHENTICATION.md**: Authentication system documentation
- **INTEGRATION.md**: E-paper display integration guide
- **SUMMARY.md**: Implementation summary

## Support

For issues or questions:
1. Check documentation in `wallet/` directory
2. Review code comments
3. Check LVGL and Tropic01 documentation

## License

See project root LICENSE file.

