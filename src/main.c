#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <lvgl.h>
#include "display_fbdev.h"
#include "wallet_ui.h"
#include "device_binding.h"
#include "tropic_auth.h"

static volatile bool running = true;

static void signal_handler(int sig) {
    (void)sig;
    running = false;
}

static int tick_thread(void *data) {
    (void)data;
    while (running) {
        usleep(5000); // 5ms tick
        lv_tick_inc(5);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    printf("Monero Hardware Wallet - Starting...\n");
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize Tropic01 authentication
    if (tropic_auth_init() < 0) {
        fprintf(stderr, "Failed to initialize Tropic01 authentication\n");
        return 1;
    }
    
    // Initialize device binding
    device_binding_t binding;
    if (device_binding_init(&binding) < 0) {
        fprintf(stderr, "Failed to initialize device binding\n");
        tropic_auth_deinit();
        return 1;
    }
    
    // Initialize LVGL
    lv_init();
    
    // Initialize display
    if (display_fbdev_init() < 0) {
        fprintf(stderr, "Failed to initialize display\n");
        device_binding_init(&binding); // Cleanup
        tropic_auth_deinit();
        return 1;
    }
    
    // Initialize wallet UI
    if (wallet_ui_init() < 0) {
        fprintf(stderr, "Failed to initialize wallet UI\n");
        display_fbdev_deinit();
        tropic_auth_deinit();
        return 1;
    }
    
    // Start tick thread (in real implementation, use proper threading)
    // For now, we'll handle ticks in the main loop
    // TODO: Implement proper threading with pthread or similar
    
    printf("Wallet initialized. Entering main loop...\n");
    
    // Main loop
    while (running) {
        lv_tick_inc(5);
        lv_timer_handler();
        usleep(5000);
    }
    
    printf("Shutting down...\n");
    
    // Cleanup
    wallet_ui_deinit();
    display_fbdev_deinit();
    tropic_auth_deinit();
    
    printf("Goodbye!\n");
    return 0;
}

