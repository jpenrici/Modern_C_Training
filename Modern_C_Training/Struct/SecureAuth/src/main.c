#include "secure_auth.h"

#include <stdio.h>

int main()
{
    auto session = auth_create("admin");
    if (session == nullptr)
        return 1;

    RawPassword pwd = { "1234567" };

    auto status = auth_set_password(session, pwd);
    if (!status) {
        fprintf(stderr, "Error setting password! Error: %d\n", status);
        auth_destroy(session);
        return 1;
    }

    if (auth_verify(session, pwd)) {
        printf("Access granted!\n");
    } else {
        printf("Access denied!\n");
    }

    auth_destroy(session);

    return 0;
}
