#!/bin/bash
# Script to find GPIO numbers for Radxa Zero 3w

echo "=== Finding GPIO Numbers for Radxa Zero 3w ==="
echo ""
echo "Physical Pin -> GPIO Name mapping:"
echo "Pin 11 -> GPIO3_A1 (RST)"
echo "Pin 12 -> GPIO3_A3 (DC)"
echo "Pin 22 -> GPIO3_C1 (BUSY)"
echo "Pin 24 -> GPIO4_C6 (CS)"
echo ""

echo "Checking available GPIO chips..."
ls -la /sys/class/gpio/ | grep gpiochip

echo ""
echo "Checking GPIO chip info..."
for chip in /sys/class/gpio/gpiochip*; do
    if [ -d "$chip" ]; then
        base=$(cat "$chip/base" 2>/dev/null)
        ngpio=$(cat "$chip/ngpio" 2>/dev/null)
        label=$(cat "$chip/label" 2>/dev/null)
        echo "Chip: $(basename $chip), Base: $base, GPIOs: $ngpio, Label: $label"
    fi
done

echo ""
echo "To find GPIO numbers, you can:"
echo "1. Check /sys/kernel/debug/pinctrl/ (if available)"
echo "2. Use gpioinfo from libgpiod: gpioinfo"
echo "3. Check device tree: cat /proc/device-tree/pinctrl*/gpio*/gpio-controller"
echo ""
echo "For Rockchip RK3566, GPIO numbering is typically:"
echo "  GPIO0: 0-31"
echo "  GPIO1: 32-63"
echo "  GPIO2: 64-95"
echo "  GPIO3: 96-127 (A=0-7, B=8-15, C=16-23, D=24-31)"
echo "  GPIO4: 128-159 (A=0-7, B=8-15, C=16-23, D=24-31)"
echo ""
echo "So:"
echo "  GPIO3_A1 = 96 + 0*8 + 1 = 97"
echo "  GPIO3_A3 = 96 + 0*8 + 3 = 99"
echo "  GPIO3_C1 = 96 + 2*8 + 1 = 113"
echo "  GPIO4_C6 = 128 + 2*8 + 6 = 150"
echo ""
echo "But these might need to be verified with actual hardware!"

