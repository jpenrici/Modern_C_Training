#include "matrix.h"

#include <assert.h>
#include <stdio.h>

bool check(size_t* n, bool cond, const char* msg);

int main()
{
    size_t n = 0;

    printf("Start testing...\n");

    // Check Matrix Structure
    static_assert(sizeof(Matrix) == 24, "Unexpected Matrix structure size");

    // Check nullptr return
    Matrix* invalid = matrix_create(0, 5, 5);
    check(&n, invalid == nullptr, "Check nullptr");

    // Chech 2D Matrix
    Matrix* m2 = matrix_create(1, 3, 4);
    check(&n, m2 != nullptr, "Check 2D matrix (depth equals 1)");

    // matrix_create(1, 3, 4) -> 1 * 3 * 4 = 12
    check(&n, m2->depth * m2->rows * m2->cols == 12, "Check capacity");
    check(&n, matrix_capacity(m2) == 12, "Check matrix_capacity function");

    // (0 * 3 * 4) + (1 * 4) + 1 = 5
    check(&n, matrix_get_index(m2, 0, 1, 1) == 5, "Check matrix_get_index function");

    bool set_ok = matrix_set(m2, 0, 2, 3, 42.0);
    check(&n, set_ok == true, "Check matrix_set function");

    double val = 0.0;
    bool get_ok = matrix_get(&val, m2, 0, 2, 3);
    check(&n, get_ok == true, "Check matrix_get function");
    check(&n, val == 42.0, "Check result");

    matrix_scale(m2, 2.0);
    matrix_get(&val, m2, 0, 2, 3);
    check(&n, val == 84.0, "Check matrix_scale function");

    matrix_free(m2);

    // Chech 3D Matrix
    Matrix* m3 = matrix_create(2, 2, 2);
    check(&n, m3 != nullptr, "Check 3D matrix (depth not equals 1)");

    check(&n, m3->depth * m3->rows * m3->cols == 8, "Check capacity");

    check(&n, matrix_capacity(m3) == 8, "Check matrix_capacity function");

    // (1 * 2 * 2) + (0 * 2) + 0 = 4
    check(&n, matrix_get_index(m3, 1, 0, 0) == 4, "Check matrix_get_index function");

    set_ok = matrix_set(m3, 1, 1, 1, 42.0);
    check(&n, set_ok == true, "Check matrix_set function");

    val = 0.0;
    get_ok = matrix_get(&val, m3, 1, 1, 1);
    check(&n, get_ok == true, "Check matrix_get function");
    check(&n, val == 42.0, "Check result");

    matrix_scale(m3, 2.0);
    matrix_get(&val, m3, 1, 1, 1);
    check(&n, val == 84.0, "Check matrix_scale function");

    matrix_fill(m3, 1000.0);
    matrix_get(&val, m3, 1, 0, 1);
    check(&n, val == 1000.0, "Check matrix_fill function");

    matrix_fill(m3, 5.0);
    matrix_sum(&val, m3);
    // check(&n, val == matrix_capacity(m3) * 5.0, "Check matrix_sum function");
    check(&n, val == (2.0 * 2.0 * 2.0) * 5.0, "Check matrix_sum function");

    matrix_free(m3);

    printf("\nAll tests passed successfully.\n");

    return 0;
}

bool check(size_t* n, bool cond, const char* msg)
{
    assert(cond);
    printf("[%02d] %s: OK\n", ++(*n), msg);
    return cond;
}
