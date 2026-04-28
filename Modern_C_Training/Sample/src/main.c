// Modern C23
// References: Modern C (Jens Gustedt)

#include <stdio.h>
#include <stdlib.h>

int main(int argc, [[maybe_unused]] char* argv[argc + 1])
{
    // Declarations
    double A[5] = {
        [0] = 9.0,
        [1] = 2.9,
        [4] = 3.E+25,
        [3] = .0007,
    };

    for (size_t i = 0; i < 5; ++i) {
        printf("Element %zu is %g \tit's square is %g\n", i, A[i], A[i] * A[i]);
    }

    return EXIT_SUCCESS;
}

/*
 * cmake -B build
 * cmake --build build
 * ./build/test
 */
