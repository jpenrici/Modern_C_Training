#include "secure_auth.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct AuthContext {
    char username[32];
    HashedPassword password_hash;
    bool is_locked;
};

// Design by Contract
static_assert(sizeof(struct AuthContext) <= 128, "AuthContext exceeded the memory safety limit.");
static_assert(offsetof(struct AuthContext, is_locked) > offsetof(struct AuthContext, password_hash),
    "Insecure memory layout: lock flag must come after the hash.");

// A hash "mock"
static HashedPassword mock_hash(RawPassword p)
{
    HashedPassword h = { 0 };
    strncpy((char*)h.hash, p.raw, 64);
    return h;
}

AuthContext* auth_create(const char* username)
{
    AuthContext* ctx = malloc(sizeof(struct AuthContext));
    if (ctx == nullptr)
        return nullptr;

    strncpy(ctx->username, username, 31);
    ctx->is_locked = false;
    return ctx;
}

bool auth_set_password(AuthContext* ctx, RawPassword pwd)
{
    if (ctx == nullptr || ctx->is_locked)
        return false;

    ctx->password_hash = mock_hash(pwd);
    return true;
}

bool auth_verify(AuthContext* ctx, RawPassword pwd)
{
    HashedPassword check = mock_hash(pwd);
    return memcmp(ctx->password_hash.hash, check.hash, 64) == 0;
}

void auth_destroy(AuthContext* ctx)
{
    if (ctx != nullptr) {
        memset(ctx, 0, sizeof(struct AuthContext));
        free(ctx);
    }
}
