# Unified Authentication and Device Binding

This document describes the unified authentication and binding mechanism between the wallet device (Radxa Zero 3w) and the base station (Cubie A5E).

## Overview

The authentication system uses the **Tropic01 secure element** to provide hardware-backed device identity and cryptographic operations. This ensures that:

1. **Device Identity**: Each wallet device has a unique, hardware-backed identity derived from the Tropic01 secure element
2. **Unified Authentication**: A single authentication mechanism is used for all services (Monero RPC, Tor, Web UI)
3. **Strong Binding**: The wallet and base station are cryptographically bound together
4. **No Key Management Overhead**: Keys are managed by the Tropic01, eliminating the need for multiple key files

## Architecture

### Device Identity

The device identity is generated from the Tropic01 secure element:

```
Device ID = SHA256(Tropic01 Device Certificate)
```

The Tropic01 device certificate is generated internally by the secure element and cannot be extracted. This provides a hardware-backed, tamper-resistant device identity.

### Binding Process

1. **Initial Setup**:
   - Wallet device generates its device ID from Tropic01
   - Base station generates its own identity
   - Both devices exchange their identities securely (via QR code, USB, or initial pairing)

2. **Binding Key Derivation**:
   ```
   Binding Key = HMAC-SHA256(Device ID, Base Station ID)
   ```

3. **Service Token Generation**:
   For each service (monerod, tor, rpc, web), a service-specific token is derived:
   ```
   Service Key = HMAC-SHA256(Master Key, Service Name)
   Service Token = HMAC-SHA256(Service Key, Device ID || Service Name)
   ```

### Authentication Flow

#### 1. Wallet to Base Station (Monero RPC)

```
1. Wallet generates challenge: Challenge = random(32 bytes)
2. Wallet signs challenge with Tropic01: Signature = Tropic01_Sign(Challenge)
3. Wallet sends to base station:
   - Device ID
   - Challenge
   - Signature
4. Base station verifies:
   - Verifies signature using device's public key
   - Checks device ID is in authorized list
   - Derives service token for "monerod"
5. Base station responds with service token
6. Wallet uses service token for all subsequent RPC calls
```

#### 2. Wallet to Base Station (Tor Hidden Service)

```
1. Wallet generates service token for "tor" service
2. Wallet includes token in Tor authentication cookie
3. Base station verifies token before allowing connection
```

#### 3. Web UI Authentication

```
1. Wallet generates service token for "web" service
2. Web UI uses token for session authentication
3. Token is included in HTTP headers or cookies
```

## Implementation

### Tropic01 Integration

The `tropic_auth.c` module provides:

- `tropic_auth_init()`: Initialize Tropic01 connection
- `tropic_auth_generate_cert()`: Generate device certificate
- `tropic_auth_sign()`: Sign data using device key
- `tropic_auth_verify()`: Verify signatures
- `tropic_auth_derive_key()`: Derive service-specific keys

### Device Binding

The `device_binding.c` module provides:

- `device_binding_init()`: Initialize binding context
- `device_binding_generate_id()`: Generate device ID from Tropic01
- `device_binding_create()`: Create binding with base station
- `device_binding_verify()`: Verify binding
- `device_binding_sign()`: Sign challenge for authentication
- `device_binding_get_service_token()`: Get unified token for service

## Security Properties

1. **Hardware-Backed Identity**: Device identity is derived from Tropic01, making it tamper-resistant
2. **No Key Extraction**: Private keys never leave the Tropic01 secure element
3. **Unified Authentication**: Single mechanism for all services reduces attack surface
4. **Binding Verification**: Base station verifies device identity before allowing access
5. **Service Isolation**: Each service has its own derived key, limiting impact of compromise

## Usage Example

```c
// Initialize device binding
device_binding_t binding;
device_binding_init(&binding);

// Create binding with base station
uint8_t binding_key[BINDING_KEY_SIZE];
device_binding_create(&binding, "base_station_id", binding_key);

// Get service token for Monero RPC
uint8_t token[32];
device_binding_get_service_token(&binding, "monerod", token, sizeof(token));

// Use token for RPC authentication
// (token is included in RPC requests)
```

## Base Station Integration

The base station must:

1. Maintain a list of authorized device IDs
2. Verify device signatures using device public keys
3. Derive service tokens using the same algorithm
4. Use tokens for service authentication

## Future Enhancements

- **Mutual Authentication**: Base station also authenticates to wallet
- **Key Rotation**: Periodic key rotation for enhanced security
- **Revocation**: Mechanism to revoke compromised devices
- **Audit Logging**: Log all authentication events for security monitoring

