# Building the Wallet Application

## Prerequisites

### System Requirements

- Radxa Zero 3w running Armbian
- Root or video group permissions for framebuffer access
- LVGL library (v9.x) installed
- OpenSSL development libraries
- Waveshare e-paper display driver (included in `../display_driver/`)

### Required Packages

```bash
# Install build dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    liblvgl-dev \
    libssl-dev \
    libcrypto++-dev

# Or if LVGL is not available as package, build from source:
# git clone https://github.com/lvgl/lvgl.git
# cd lvgl && make
```

## Building with CMake

```bash
cd wallet
mkdir build && cd build
cmake ..
make
```

The executable will be created as `build/wallet_app`.

## Building with Make

```bash
cd wallet
make
```

The executable will be created as `wallet_app`.

## Configuration

### Display Configuration

The display is configured for the 2.13" Waveshare e-paper display V4:
- Resolution: 122x250 pixels
- Monochrome (black/white)
- FBDEV backend

### Framebuffer Device

By default, the application uses `/dev/fb0`. To use a different device:

```bash
export LV_LINUX_FBDEV_DEVICE=/dev/fb1
./wallet_app
```

### Permissions

The application needs access to the framebuffer device. Either:

1. Run as root:
   ```bash
   sudo ./wallet_app
   ```

2. Add user to video group:
   ```bash
   sudo usermod -a -G video $USER
   # Log out and back in
   ```

## Running

```bash
# With root
sudo ./wallet_app

# Or with video group
./wallet_app
```

## Troubleshooting

### Framebuffer Not Found

If you get "Cannot open framebuffer device":
- Check that `/dev/fb0` exists: `ls -l /dev/fb*`
- Verify permissions: `ls -l /dev/fb0`
- Try running with `sudo`

### LVGL Not Found

If you get "LVGL not found":
- Install LVGL development package: `sudo apt-get install liblvgl-dev`
- Or build LVGL from source and adjust include paths in CMakeLists.txt/Makefile

### Display Not Updating

- Verify framebuffer is working: `cat /dev/fb0 > /tmp/fb.raw`
- Check display driver initialization
- Ensure e-paper display is properly connected

## Development

### Debug Build

```bash
# CMake
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Make
make CFLAGS="-g -O0"
```

### Verbose Output

The application includes debug output. To see more details, check the console output when running.

## Integration with Waveshare Driver

The current implementation uses FBDEV as a bridge. For full e-paper support, you need to:

1. Integrate the Waveshare driver functions
2. Call `EPD_2in13_V4_Init()` during initialization
3. Call `EPD_2in13_V4_Display()` or `EPD_2in13_V4_Display_Fast()` for updates

See `drivers/epaper_driver.c` for the integration point.

