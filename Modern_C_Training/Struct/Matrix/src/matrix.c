#include "matrix.h"

Matrix* matrix_create(size_t depth, size_t rows, size_t cols)
{
    size_t capacity = depth * rows * cols;
    if (capacity == 0)
        return nullptr;

    Matrix* matrix = malloc(sizeof(Matrix) + (sizeof(double) * capacity));
    if (matrix == nullptr)
        return nullptr;

    *matrix = (Matrix) { .depth = depth, .rows = rows, .cols = cols };

    return matrix;
}

void matrix_free(Matrix* matrix)
{
    free(matrix);
}

bool matrix_scale(Matrix* matrix, double scalar)
{
    if (matrix == nullptr)
        return false;

    size_t capacity = matrix_capacity(matrix);
    if (capacity == 0)
        return false;

    for (size_t i = 0; i < capacity; i++)
        matrix->data[i] *= scalar;

    return true;
}

STATIC size_t matrix_capacity(const Matrix* matrix)
{
    return matrix->depth * matrix->rows * matrix->cols;
}

STATIC size_t matrix_get_index(const Matrix* matrix, size_t z, size_t y, size_t x)
{
    return (z * matrix->rows * matrix->cols) + (y * matrix->cols) + x;
}

bool matrix_set(Matrix* matrix, size_t z, size_t y, size_t x, double value)
{
    if (matrix == nullptr)
        return false;

    size_t capacity = matrix_capacity(matrix);
    if (capacity == 0)
        return false;

    size_t index = matrix_get_index(matrix, z, y, x);
    if (index > capacity)
        return false;

    matrix->data[index] = value;

    return true;
}

bool matrix_get(double* result, const Matrix* matrix, size_t z, size_t y, size_t x)
{
    *result = 0.0;

    if (matrix == nullptr)
        return false;

    size_t capacity = matrix_capacity(matrix);
    if (capacity == 0)
        return false;

    if (z >= matrix->depth || y >= matrix->rows || x >= matrix->cols)
        return false;

    size_t index = matrix_get_index(matrix, z, y, x);
    *result = matrix->data[index];

    return true;
}
