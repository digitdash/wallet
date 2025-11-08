#!/bin/bash
# Quick script to manually install LVGL for Radxa Zero 3w

set -e

echo "Installing LVGL from source..."

# Install dependencies
echo "Installing build dependencies..."
sudo apt-get update
sudo apt-get install -y build-essential cmake git

# Clone LVGL
echo "Cloning LVGL..."
cd ~
if [ -d "lvgl" ]; then
    echo "LVGL directory exists, updating..."
    cd lvgl
    git pull
else
    git clone https://github.com/lvgl/lvgl.git
    cd lvgl
fi

# Checkout stable version
git checkout v9.4.0

# Copy configuration
echo "Copying LVGL configuration..."
cp ~/wallet/lv_conf.h lv_conf.h

# Build LVGL
echo "Building LVGL..."
mkdir -p build
cd build
cmake ..
make -j$(nproc)

# Install
echo "Installing LVGL..."
sudo make install

echo "LVGL installed successfully!"
echo "You can now build the wallet application."

