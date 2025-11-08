# Physical Connections Guide

## Complete Wiring Diagram

**YES, you MUST connect ALL pins including CLK and MOSI!**

```
Waveshare 2.13" V4 Display    →    Radxa Zero 3w
─────────────────────────────────────────────────
VCC                           →    3.3V
GND                           →    GND
DIN (MOSI)                    →    SPI0_MOSI (GPIO 19) ⚠️ MUST CONNECT
CLK (SCLK)                    →    SPI0_SCLK (GPIO 23) ⚠️ MUST CONNECT
CS                            →    GPIO 24 ⚠️ MUST CONNECT
DC                            →    GPIO 13 ⚠️ MUST CONNECT
RST                           →    GPIO 11 ⚠️ MUST CONNECT
BUSY                          →    GPIO 35 ⚠️ MUST CONNECT
```

## Important Clarification

### Physical Connections (REQUIRED)
✅ **You MUST physically connect ALL pins**, including:
- CLK (SPI Clock)
- MOSI (SPI Data Out)
- CS (Chip Select)
- DC (Data/Command)
- RST (Reset)
- BUSY (Busy Signal)
- VCC (Power)
- GND (Ground)

### Software Configuration (AUTOMATIC)
❌ **You DON'T need to configure CLK and MOSI as GPIO pins in software** because:
- The hardware SPI driver (`/dev/spidev0.0`) automatically manages CLK and MOSI
- The Linux kernel handles clock generation and data transmission
- You only need to configure CS, DC, RST, and BUSY as GPIO pins

## Pin-by-Pin Breakdown

| Display Pin | Radxa Zero 3w Pin | Type | Software Config? |
|-------------|-------------------|------|------------------|
| VCC | 3.3V | Power | No |
| GND | GND | Ground | No |
| DIN (MOSI) | SPI0_MOSI (GPIO 19) | SPI Data | No - Hardware managed |
| CLK (SCLK) | SPI0_SCLK (GPIO 23) | SPI Clock | No - Hardware managed |
| CS | GPIO 24 | Control | Yes - Manual GPIO |
| DC | GPIO 13 | Control | Yes - Manual GPIO |
| RST | GPIO 11 | Control | Yes - Manual GPIO |
| BUSY | GPIO 35 | Status | Yes - Manual GPIO |

## Why This Confusion?

The confusion comes from the difference between:
1. **Physical wiring** (always required)
2. **Software GPIO configuration** (only needed for manually controlled pins)

For hardware SPI:
- CLK and MOSI are **physically connected** (wires)
- CLK and MOSI are **NOT configured as GPIO** in software (handled by SPI driver)

For manually controlled pins (CS, DC, RST, BUSY):
- They are **physically connected** (wires)
- They are **configured as GPIO** in software (manual control)

## Verification Checklist

Before running the application:

- [ ] VCC connected to 3.3V
- [ ] GND connected to GND
- [ ] CLK connected to SPI0_SCLK (GPIO 23)
- [ ] MOSI connected to SPI0_MOSI (GPIO 19)
- [ ] CS connected to GPIO 24
- [ ] DC connected to GPIO 13
- [ ] RST connected to GPIO 11
- [ ] BUSY connected to GPIO 35
- [ ] SPI device exists: `ls /dev/spidev*`
- [ ] All connections are secure

## Troubleshooting

### Display Not Working?

1. **Check all physical connections** - especially CLK and MOSI
2. **Verify SPI is enabled**: `ls /dev/spidev*`
3. **Check power**: Measure 3.3V at VCC pin
4. **Test SPI communication**: Use `spidev_test` if available
5. **Check pin numbers**: Verify against Radxa Zero 3w pinout diagram

### Common Mistakes

❌ **"I don't need to connect CLK and MOSI"** - WRONG! You must connect them.
❌ **"I need to configure CLK as GPIO"** - WRONG! Hardware SPI handles it.
✅ **"I connect CLK and MOSI physically, but don't configure them in software"** - CORRECT!

## Summary

**Physical connections**: Connect ALL pins including CLK and MOSI
**Software configuration**: Only configure CS, DC, RST, and BUSY as GPIO pins

The hardware SPI driver automatically handles CLK and MOSI once they're physically connected.


