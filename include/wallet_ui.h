#ifndef WALLET_UI_H
#define WALLET_UI_H

#include <lvgl.h>

/**
 * Initialize the wallet UI
 * @return 0 on success, negative on error
 */
int wallet_ui_init(void);

/**
 * Deinitialize the wallet UI
 */
void wallet_ui_deinit(void);

/**
 * Update the wallet balance display
 * @param balance Balance in atomic units
 */
void wallet_ui_update_balance(uint64_t balance);

/**
 * Show transaction confirmation screen
 * @param amount Amount in atomic units
 * @param address Destination address
 * @return 1 if confirmed, 0 if cancelled
 */
int wallet_ui_confirm_transaction(uint64_t amount, const char *address);

/**
 * Show error message
 * @param message Error message to display
 */
void wallet_ui_show_error(const char *message);

/**
 * Show status message
 * @param message Status message to display
 */
void wallet_ui_show_status(const char *message);

#endif // WALLET_UI_H

