#include "device_binding.h"
#include "tropic_auth.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

int device_binding_init(device_binding_t *binding) {
    if (!binding) {
        return -1;
    }
    
    memset(binding, 0, sizeof(device_binding_t));
    
    // Generate device ID from Tropic01
    if (device_binding_generate_id(binding, binding->device_id, DEVICE_ID_SIZE) < 0) {
        return -1;
    }
    
    binding->is_bound = false;
    return 0;
}

int device_binding_generate_id(device_binding_t *binding, uint8_t *device_id, size_t device_id_size) {
    if (!binding || !device_id || device_id_size < DEVICE_ID_SIZE) {
        return -1;
    }
    
    // Use Tropic01 to generate device certificate
    uint8_t cert[256];
    if (tropic_auth_generate_cert(cert, sizeof(cert)) < 0) {
        return -1;
    }
    
    // Hash certificate to get device ID
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, cert, sizeof(cert));
    SHA256_Final(device_id, &ctx);
    
    return 0;
}

int device_binding_create(device_binding_t *binding, const char *base_station_id, uint8_t *binding_key) {
    if (!binding || !base_station_id || !binding_key) {
        return -1;
    }
    
    // Derive binding key from device ID and base station ID
    // Using HMAC-SHA256
    unsigned int key_len = BINDING_KEY_SIZE;
    HMAC(EVP_sha256(), binding->device_id, DEVICE_ID_SIZE,
         (const unsigned char *)base_station_id, strlen(base_station_id),
         binding_key, &key_len);
    
    memcpy(binding->binding_key, binding_key, BINDING_KEY_SIZE);
    binding->is_bound = true;
    
    return 0;
}

bool device_binding_verify(device_binding_t *binding, const char *base_station_id,
                          const uint8_t *challenge, size_t challenge_size,
                          const uint8_t *signature, size_t signature_size) {
    if (!binding || !base_station_id || !challenge || !signature) {
        return false;
    }
    
    // Verify signature using Tropic01
    return tropic_auth_verify(challenge, challenge_size, signature, signature_size);
}

int device_binding_sign(device_binding_t *binding, const uint8_t *challenge, size_t challenge_size,
                       uint8_t *signature, size_t signature_size) {
    if (!binding || !challenge || !signature) {
        return -1;
    }
    
    if (signature_size < SIGNATURE_SIZE) {
        return -1;
    }
    
    // Sign challenge using Tropic01
    return tropic_auth_sign(challenge, challenge_size, signature, signature_size);
}

int device_binding_get_service_token(device_binding_t *binding, const char *service_name,
                                    uint8_t *token, size_t token_size) {
    if (!binding || !service_name || !token) {
        return -1;
    }
    
    // Derive service-specific key from binding key
    uint8_t service_key[32];
    if (tropic_auth_derive_key(service_name, service_key, sizeof(service_key)) < 0) {
        return -1;
    }
    
    // Create token: HMAC(binding_key, service_name || device_id)
    size_t data_len = strlen(service_name) + DEVICE_ID_SIZE;
    uint8_t *data = malloc(data_len);
    if (!data) {
        return -1;
    }
    
    memcpy(data, service_name, strlen(service_name));
    memcpy(data + strlen(service_name), binding->device_id, DEVICE_ID_SIZE);
    
    unsigned int token_len = token_size;
    HMAC(EVP_sha256(), service_key, sizeof(service_key),
         data, data_len, token, &token_len);
    
    free(data);
    return 0;
}

