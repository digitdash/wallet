cmake_minimum_required(VERSION 3.10)
project(monero_wallet VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/auth
    ${CMAKE_SOURCE_DIR}/../display_driver/c/lib/Config
    ${CMAKE_SOURCE_DIR}/../display_driver/c/lib/e-Paper
    ${CMAKE_SOURCE_DIR}/../display_driver/c/lib/GUI
)

# Find required packages
find_package(PkgConfig REQUIRED)

# LVGL (assuming installed via system package or submodule)
pkg_check_modules(LVGL REQUIRED lvgl)
if(NOT LVGL_FOUND)
    message(FATAL_ERROR "LVGL not found. Install via: apt-get install liblvgl-dev or use submodule")
endif

# Framebuffer libraries
find_library(LINUX_FBDEV linux_fbdev)

# Source files
set(SOURCES
    src/main.c
    src/wallet_ui.c
    src/display_fbdev.c
    drivers/epaper_driver.c
    drivers/gpio_driver.c
    auth/device_binding.c
    auth/tropic_auth.c
)

# Display driver sources (Waveshare)
set(DISPLAY_DRIVER_SOURCES
    ../display_driver/c/lib/e-Paper/EPD_2in13_V4.c
    ../display_driver/c/lib/Config/DEV_Config.c
    ../display_driver/c/lib/Config/sysfs_gpio.c
    ../display_driver/c/lib/Config/sysfs_software_spi.c
    ../display_driver/c/lib/GUI/GUI_Paint.c
)

# Create executable
add_executable(wallet_app ${SOURCES} ${DISPLAY_DRIVER_SOURCES})

# Link libraries
target_link_libraries(wallet_app
    ${LVGL_LIBRARIES}
    m
    pthread
)

target_compile_options(wallet_app PRIVATE ${LVGL_CFLAGS_OTHER})

# Compiler definitions
target_compile_definitions(wallet_app PRIVATE
    EPD=epd2in13V4
    USE_DEV_LIB
    RADXA_ZERO_3W
    LV_CONF_INCLUDE_SIMPLE
)

# Installation
install(TARGETS wallet_app DESTINATION /usr/local/bin)

