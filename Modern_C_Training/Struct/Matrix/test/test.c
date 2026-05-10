#include "matrix.h"
#include <assert.h>
#include <stdio.h>

int main()
{
    printf("Start testing...\n");
    const int TOTAL = 18;

    // Check Matrix Structure
    static_assert(sizeof(Matrix) == 24, "Unexpected Matrix structure size");
    printf("[1/%d] Check Matrix Structure: OK\n", TOTAL);

    // Check nullptr return
    Matrix* invalid = matrix_create(0, 5, 5);
    assert(invalid == nullptr);
    printf("[2/%d] Check nullptr: OK\n", TOTAL);

    // Chech 2D Matrix
    Matrix* m2 = matrix_create(1, 3, 4);
    assert(m2 != nullptr);
    printf("[3/%d] Check 2D matrix (depth equals 1): OK\n", TOTAL);

    assert(m2->depth * m2->rows * m2->cols == 12); // 1 * 3 * 4 = 12
    printf("[4/%d] Check capacity: OK\n", TOTAL);

    assert(matrix_capacity(m2) == 12);
    printf("[5/%d] Check matrix_capacity function: OK\n", TOTAL);

    assert(matrix_get_index(m2, 0, 1, 1) == 5); // (0 * 3 * 4) + (1 * 4) + 1 = 5
    printf("[6/%d] Check matrix_get_index function: OK\n", TOTAL);

    bool set_ok = matrix_set(m2, 0, 2, 3, 42.0);
    assert(set_ok == true);
    printf("[7/%d] Check matrix_set function: OK\n", TOTAL);

    double val = 0.0;
    bool get_ok = matrix_get(&val, m2, 0, 2, 3);
    assert(get_ok == true);
    assert(val == 42.0);
    printf("[8/%d] Check matrix_get function: OK\n", TOTAL);

    matrix_scale(m2, 2.0);
    matrix_get(&val, m2, 0, 2, 3);
    assert(val == 84.0);
    printf("[9/%d] Check scale function: OK\n", TOTAL);

    matrix_free(m2);
    printf("[10/%d] Check 2D Matrix: OK\n", TOTAL);

    // Chech 3D Matrix
    Matrix* m3 = matrix_create(2, 2, 2);
    assert(m3 != nullptr);
    printf("[11/%d] Check 3D matrix (depth not equals 1): OK\n", TOTAL);

    assert(m3->depth * m3->rows * m3->cols == 8);
    printf("[12/%d] Check capacity: OK\n", TOTAL);

    assert(matrix_capacity(m3) == 8);
    printf("[13/%d] Check matrix_capacity function: OK\n", TOTAL);

    assert(matrix_get_index(m3, 1, 0, 0) == 4); // (1 * 2 * 2) + (0 * 2) + 0 = 4
    printf("[14/%d] Check matrix_get_index function: OK\n", TOTAL);

    set_ok = matrix_set(m3, 1, 1, 1, 42.0);
    assert(set_ok == true);
    printf("[15/%d] Check matrix_set function: OK\n", TOTAL);

    val = 0.0;
    get_ok = matrix_get(&val, m3, 1, 1, 1);
    assert(get_ok == true);
    assert(val == 42.0);
    printf("[16/%d] Check matrix_get function: OK\n", TOTAL);

    matrix_scale(m3, 2.0);
    matrix_get(&val, m3, 1, 1, 1);
    assert(val == 84.0);
    printf("[17/%d] Check scale function: OK\n", TOTAL);

    matrix_free(m3);
    printf("[18/%d] Check 3D Matrix: OK\n", TOTAL);

    printf("\nAll tests passed successfully.\n");

    return 0;
}
