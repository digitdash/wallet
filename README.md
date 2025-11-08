# Monero Hardware Wallet - Radxa Zero 3w

This directory contains the wallet application for the Radxa Zero 3w with:
- 2.13" Waveshare e-paper display V4
- Tropic01 secure element
- LVGL-based UI with FBDEV backend

## Structure

```
wallet/
├── src/              # Main application source
├── include/          # Header files
├── drivers/          # Display and hardware drivers
├── auth/             # Authentication and binding
├── CMakeLists.txt    # Build configuration
└── README.md         # This file
```

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

```bash
sudo ./wallet_app
```

Note: Requires root or video group permissions for framebuffer access.

# wallet
