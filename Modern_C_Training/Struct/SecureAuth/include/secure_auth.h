#pragma once

#include <stdbool.h>
#include <stddef.h>

// Wrapper Types (String Typing)
typedef struct {
    char raw[32];
} RawPassword;

typedef struct {
    unsigned char hash[64];
} HashedPassword;

// Opaque Pointer
typedef struct AuthContext AuthContext;

// API
[[nodiscard]] AuthContext* auth_create(const char* username);
void auth_destroy(AuthContext* ctx);

[[nodiscard]] bool auth_set_password(AuthContext* ctx, RawPassword pwd);
[[nodiscard]] bool auth_verify(AuthContext* ctx, RawPassword pwd);
