#include "wallet_ui.h"
#include "display_fbdev.h"
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static lv_obj_t *balance_label = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *main_screen = NULL;
static bool transaction_confirmed = false;

static void btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        transaction_confirmed = true;
    }
}

static void cancel_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        transaction_confirmed = false;
    }
}

int wallet_ui_init(void) {
    // Create main screen
    main_screen = lv_obj_create(NULL);
    lv_scr_load(main_screen);
    
    // Create balance label
    balance_label = lv_label_create(main_screen);
    lv_obj_align(balance_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_label_set_text(balance_label, "Balance: 0.000000000 XMR");
    lv_obj_set_style_text_font(balance_label, &lv_font_montserrat_18, 0);
    
    // Create status label
    status_label = lv_label_create(main_screen);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
    
    return 0;
}

void wallet_ui_deinit(void) {
    if (main_screen) {
        lv_obj_del(main_screen);
        main_screen = NULL;
    }
    balance_label = NULL;
    status_label = NULL;
}

void wallet_ui_update_balance(uint64_t balance) {
    if (!balance_label) return;
    
    // Convert atomic units to XMR (12 decimal places)
    char balance_str[64];
    snprintf(balance_str, sizeof(balance_str), "Balance: %.12f XMR", balance / 1e12);
    lv_label_set_text(balance_label, balance_str);
}

int wallet_ui_confirm_transaction(uint64_t amount, const char *address) {
    if (!main_screen) return 0;
    
    transaction_confirmed = false;
    
    // Create confirmation screen
    lv_obj_t *confirm_screen = lv_obj_create(NULL);
    lv_scr_load(confirm_screen);
    
    // Amount label
    char amount_str[64];
    snprintf(amount_str, sizeof(amount_str), "Amount: %.12f XMR", amount / 1e12);
    lv_obj_t *amount_label = lv_label_create(confirm_screen);
    lv_obj_align(amount_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_label_set_text(amount_label, amount_str);
    lv_obj_set_style_text_font(amount_label, &lv_font_montserrat_16, 0);
    
    // Address label (truncated)
    char addr_display[32];
    if (address && strlen(address) > 0) {
        snprintf(addr_display, sizeof(addr_display), "To: %.12s...", address);
    } else {
        strcpy(addr_display, "To: Unknown");
    }
    lv_obj_t *addr_label = lv_label_create(confirm_screen);
    lv_obj_align(addr_label, LV_ALIGN_CENTER, 0, -20);
    lv_label_set_text(addr_label, addr_display);
    lv_obj_set_style_text_font(addr_label, &lv_font_montserrat_14, 0);
    
    // Confirm button
    lv_obj_t *confirm_btn = lv_btn_create(confirm_screen);
    lv_obj_align(confirm_btn, LV_ALIGN_BOTTOM_LEFT, 20, -20);
    lv_obj_t *confirm_label = lv_label_create(confirm_btn);
    lv_label_set_text(confirm_label, "Confirm");
    lv_obj_add_event_cb(confirm_btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Cancel button
    lv_obj_t *cancel_btn = lv_btn_create(confirm_screen);
    lv_obj_align(cancel_btn, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_obj_t *cancel_label = lv_label_create(cancel_btn);
    lv_label_set_text(cancel_label, "Cancel");
    lv_obj_add_event_cb(cancel_btn, cancel_btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Wait for user input (in real implementation, this would be event-driven)
    // For now, return immediately (actual implementation would use input handling)
    
    // Clean up
    lv_obj_del(confirm_screen);
    lv_scr_load(main_screen);
    
    return transaction_confirmed ? 1 : 0;
}

void wallet_ui_show_error(const char *message) {
    if (!status_label) return;
    
    char error_msg[128];
    snprintf(error_msg, sizeof(error_msg), "Error: %s", message);
    lv_label_set_text(status_label, error_msg);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0xFF0000), 0);
}

void wallet_ui_show_status(const char *message) {
    if (!status_label) return;
    
    lv_label_set_text(status_label, message);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x000000), 0);
}

