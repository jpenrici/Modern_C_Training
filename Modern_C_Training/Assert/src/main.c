// Reference: https://cppreference.com/c/language/_Static_assert
#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS
#include <assert.h>

int main()
{
    constexpr int _42 = 2 * 3 * 2 * 3 + 2 * 3;

    static_assert((2 + 2) % 3 == 1, "Optional error message!");
    static_assert(_42 == 42);

    printf("Finished!\n");

    return EXIT_SUCCESS;
}
