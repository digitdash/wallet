#ifndef TROPIC_AUTH_H
#define TROPIC_AUTH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Initialize Tropic01 authentication
 * @return 0 on success, negative on error
 */
int tropic_auth_init(void);

/**
 * Deinitialize Tropic01 authentication
 */
void tropic_auth_deinit(void);

/**
 * Generate device certificate using Tropic01
 * @param cert Output buffer for certificate
 * @param cert_size Size of cert buffer
 * @return 0 on success, negative on error
 */
int tropic_auth_generate_cert(uint8_t *cert, size_t cert_size);

/**
 * Sign data using Tropic01 device key
 * @param data Data to sign
 * @param data_size Size of data
 * @param signature Output signature buffer
 * @param signature_size Size of signature buffer
 * @return 0 on success, negative on error
 */
int tropic_auth_sign(const uint8_t *data, size_t data_size,
                    uint8_t *signature, size_t signature_size);

/**
 * Verify signature using Tropic01
 * @param data Data that was signed
 * @param data_size Size of data
 * @param signature Signature to verify
 * @param signature_size Size of signature
 * @return true if signature is valid, false otherwise
 */
bool tropic_auth_verify(const uint8_t *data, size_t data_size,
                       const uint8_t *signature, size_t signature_size);

/**
 * Derive key from Tropic01 for service authentication
 * @param service_name Service identifier
 * @param key Output key buffer
 * @param key_size Size of key buffer
 * @return 0 on success, negative on error
 */
int tropic_auth_derive_key(const char *service_name, uint8_t *key, size_t key_size);

#endif // TROPIC_AUTH_H

