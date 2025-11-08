#ifndef DEVICE_BINDING_H
#define DEVICE_BINDING_H

#include <stdint.h>
#include <stdbool.h>

#define DEVICE_ID_SIZE 32
#define BINDING_KEY_SIZE 32
#define CHALLENGE_SIZE 32
#define SIGNATURE_SIZE 64

/**
 * Device binding context
 */
typedef struct {
    uint8_t device_id[DEVICE_ID_SIZE];
    uint8_t binding_key[BINDING_KEY_SIZE];
    bool is_bound;
} device_binding_t;

/**
 * Initialize device binding using Tropic01
 * @param binding Binding context to initialize
 * @return 0 on success, negative on error
 */
int device_binding_init(device_binding_t *binding);

/**
 * Generate device identity from Tropic01
 * @param binding Binding context
 * @param device_id Output buffer for device ID
 * @param device_id_size Size of device_id buffer
 * @return 0 on success, negative on error
 */
int device_binding_generate_id(device_binding_t *binding, uint8_t *device_id, size_t device_id_size);

/**
 * Create binding with base station
 * @param binding Binding context
 * @param base_station_id Base station identifier
 * @param binding_key Output binding key
 * @return 0 on success, negative on error
 */
int device_binding_create(device_binding_t *binding, const char *base_station_id, uint8_t *binding_key);

/**
 * Verify binding with base station
 * @param binding Binding context
 * @param base_station_id Base station identifier
 * @param challenge Challenge data
 * @param challenge_size Size of challenge
 * @param signature Signature to verify
 * @param signature_size Size of signature
 * @return true if binding is valid, false otherwise
 */
bool device_binding_verify(device_binding_t *binding, const char *base_station_id,
                          const uint8_t *challenge, size_t challenge_size,
                          const uint8_t *signature, size_t signature_size);

/**
 * Sign challenge for base station authentication
 * @param binding Binding context
 * @param challenge Challenge data
 * @param challenge_size Size of challenge
 * @param signature Output signature buffer
 * @param signature_size Size of signature buffer
 * @return 0 on success, negative on error
 */
int device_binding_sign(device_binding_t *binding, const uint8_t *challenge, size_t challenge_size,
                       uint8_t *signature, size_t signature_size);

/**
 * Get unified authentication token for all services
 * @param binding Binding context
 * @param service_name Service name (e.g., "monerod", "tor", "rpc")
 * @param token Output token buffer
 * @param token_size Size of token buffer
 * @return 0 on success, negative on error
 */
int device_binding_get_service_token(device_binding_t *binding, const char *service_name,
                                    uint8_t *token, size_t token_size);

#endif // DEVICE_BINDING_H

