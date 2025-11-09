cmake_minimum_required(VERSION 3.10)
project(monero_wallet VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories (will be updated after finding display_driver)
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/drivers
    ${CMAKE_SOURCE_DIR}/auth
    ${CMAKE_SOURCE_DIR}
)

# Find display_driver directory first (now inside wallet folder)
set(DISPLAY_DRIVER_BASE_DIR "")
if(EXISTS ${CMAKE_SOURCE_DIR}/display_driver/c/lib/e-Paper/EPD_2in13_V4.c)
    set(DISPLAY_DRIVER_BASE_DIR ${CMAKE_SOURCE_DIR}/display_driver)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/../display_driver/c/lib/e-Paper/EPD_2in13_V4.c)
    set(DISPLAY_DRIVER_BASE_DIR ${CMAKE_SOURCE_DIR}/../display_driver)
elseif(EXISTS /root/wallet/display_driver/c/lib/e-Paper/EPD_2in13_V4.c)
    set(DISPLAY_DRIVER_BASE_DIR /root/wallet/display_driver)
else()
    message(FATAL_ERROR "Cannot find display_driver directory. Please check the path.")
endif()

message(STATUS "Using display_driver at: ${DISPLAY_DRIVER_BASE_DIR}")

# Add display_driver include directories
include_directories(
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/e-Paper
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/GUI
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Fonts
)

# Find required packages
find_package(PkgConfig REQUIRED)

# LVGL - Try multiple methods to find it
set(LVGL_FOUND FALSE)

# Method 1: Try pkg-config
pkg_check_modules(LVGL_PKG lvgl)
if(LVGL_PKG_FOUND)
    set(LVGL_FOUND TRUE)
    set(LVGL_INCLUDE_DIRS ${LVGL_PKG_INCLUDE_DIRS})
    set(LVGL_LIBRARIES ${LVGL_PKG_LIBRARIES})
    set(LVGL_CFLAGS_OTHER ${LVGL_PKG_CFLAGS_OTHER})
    message(STATUS "Found LVGL via pkg-config")
endif()

# Method 2: Try to find LVGL as submodule or in parent directory
if(NOT LVGL_FOUND)
    # Check for LVGL submodule
    if(EXISTS ${CMAKE_SOURCE_DIR}/../lvgl/CMakeLists.txt)
        set(LVGL_FOUND TRUE)
        add_subdirectory(${CMAKE_SOURCE_DIR}/../lvgl)
        set(LVGL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/../lvgl)
        set(LVGL_LIBRARIES lvgl)
        set(LVGL_CFLAGS_OTHER "")
        message(STATUS "Found LVGL as submodule")
    endif()
endif()

# Method 3: Try to find LVGL in system paths
if(NOT LVGL_FOUND)
    find_path(LVGL_INCLUDE_DIR lvgl.h 
        PATHS 
        /usr/include/lvgl 
        /usr/local/include/lvgl
        /usr/include
        /usr/local/include
    )
    find_library(LVGL_LIBRARY 
        NAMES lvgl liblvgl
        PATHS 
        /usr/lib 
        /usr/local/lib
        /usr/lib/aarch64-linux-gnu
    )
    if(LVGL_INCLUDE_DIR AND LVGL_LIBRARY)
        set(LVGL_FOUND TRUE)
        set(LVGL_INCLUDE_DIRS ${LVGL_INCLUDE_DIR})
        set(LVGL_LIBRARIES ${LVGL_LIBRARY})
        set(LVGL_CFLAGS_OTHER "")
        message(STATUS "Found LVGL in system paths")
    endif()
endif()

# Method 4: Download and build LVGL if not found
if(NOT LVGL_FOUND)
    message(STATUS "LVGL not found. Will download and build from source...")
    include(FetchContent)
    FetchContent_Declare(
        lvgl
        GIT_REPOSITORY https://github.com/lvgl/lvgl.git
        GIT_TAG v8.4.0
    )
    # Configure LVGL before making it available
    set(LV_USE_DRAW_SW_HELIUM OFF CACHE BOOL "Disable Helium assembly optimizations" FORCE)
    set(LV_USE_DRAW_SW_ASM OFF CACHE BOOL "Disable all assembly optimizations" FORCE)
    set(LV_BUILD_EXAMPLES OFF CACHE BOOL "Disable examples" FORCE)
    set(LV_BUILD_DEMOS OFF CACHE BOOL "Disable demos" FORCE)
    
    FetchContent_MakeAvailable(lvgl)
    
    # Copy our lv_conf.h to LVGL source directory
    configure_file(
        ${CMAKE_SOURCE_DIR}/lv_conf.h
        ${lvgl_SOURCE_DIR}/lv_conf.h
        COPYONLY
    )
    
    # Note: LVGL v8.4.0 doesn't have Helium assembly optimizations
    # This avoids the assembler issues with v9.x
    
    set(LVGL_FOUND TRUE)
    set(LVGL_INCLUDE_DIRS ${lvgl_SOURCE_DIR})
    set(LVGL_LIBRARIES lvgl)
    set(LVGL_CFLAGS_OTHER "")
    message(STATUS "LVGL will be downloaded and built from source")
endif()

# Source files (using CMAKE_SOURCE_DIR for proper paths)
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/wallet_ui.c
    ${CMAKE_SOURCE_DIR}/src/display_fbdev.c
    ${CMAKE_SOURCE_DIR}/drivers/epaper_driver.c
    ${CMAKE_SOURCE_DIR}/drivers/gpio_driver.c
    ${CMAKE_SOURCE_DIR}/auth/device_binding.c
    ${CMAKE_SOURCE_DIR}/auth/tropic_auth.c
)

# Display driver sources (Waveshare) - using DISPLAY_DRIVER_BASE_DIR found above
set(DISPLAY_DRIVER_SOURCES
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/e-Paper/EPD_2in13_V4.c
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config/DEV_Config.c
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config/sysfs_gpio.c
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config/dev_hardware_SPI.c
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config/sysfs_software_spi.c
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/GUI/GUI_Paint.c
)

# Create executable
add_executable(wallet_app ${SOURCES} ${DISPLAY_DRIVER_SOURCES})

# Add LVGL include directories
if(LVGL_FOUND)
    if(LVGL_INCLUDE_DIRS)
        target_include_directories(wallet_app PRIVATE ${LVGL_INCLUDE_DIRS})
    endif()
endif()

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Link libraries
target_link_libraries(wallet_app
    ${LVGL_LIBRARIES}
    OpenSSL::SSL
    OpenSSL::Crypto
    m
    pthread
)

# Add LVGL compile options
if(LVGL_FOUND AND LVGL_CFLAGS_OTHER)
    target_compile_options(wallet_app PRIVATE ${LVGL_CFLAGS_OTHER})
endif()

# Compiler definitions
target_compile_definitions(wallet_app PRIVATE
    EPD=epd2in13V4
    USE_DEV_LIB
    RADXA_ZERO_3W
    LV_CONF_INCLUDE_SIMPLE
    DEBUG=1
)

# Test display executable
add_executable(test_display 
    test_display.c
    ${DISPLAY_DRIVER_SOURCES}
)

target_include_directories(test_display PRIVATE
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/e-Paper
)

target_compile_definitions(test_display PRIVATE
    EPD=epd2in13V4
    USE_DEV_LIB
    RADXA_ZERO_3W
    DEBUG=1
)

target_link_libraries(test_display
    m
    pthread
)

# Test GPIO executable
add_executable(test_gpio 
    test_gpio.c
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config/sysfs_gpio.c
)

target_include_directories(test_gpio PRIVATE
    ${DISPLAY_DRIVER_BASE_DIR}/c/lib/Config
)

target_link_libraries(test_gpio
    m
)

# Test SPI executable
add_executable(test_spi 
    test_spi.c
)

target_link_libraries(test_spi
    m
)

# Installation
install(TARGETS wallet_app DESTINATION /usr/local/bin)
install(TARGETS test_display DESTINATION /usr/local/bin)

