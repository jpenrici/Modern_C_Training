#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

bool matrix_fill(Matrix* matrix, double value)
{
    if (matrix == nullptr)
        return false;

    size_t capacity = matrix_capacity(matrix);
    if (capacity == 0)
        return false;

    if (value == 0.0) {
        memset(matrix->data, 0, capacity * sizeof(double));
        return true;
    }

    for (size_t i = 0; i < capacity; i++) {
        matrix->data[i] = value;
    }

    return true;
}

bool matrix_sum(double* result, const Matrix* matrix)
{
    if (matrix == nullptr)
        return false;

    size_t capacity = matrix_capacity(matrix);
    if (capacity == 0)
        return false;

    *result = 0;
    for (size_t i = 0; i < capacity; i++) {
        *result += matrix->data[i];
    }

    return true;
}

static bool should_show(size_t i, size_t total, size_t max)
{
    if (total <= max)
        return true;
    size_t half = max / 2;
    return i < half || i >= total - half;
}

static void print_row(const Matrix* matrix, size_t z, size_t y)
{
    size_t cols = matrix->cols;
    size_t max = MATRIX_PRINT_MAX;
    size_t half = max / 2;
    bool trunc = cols > max;

    printf(" ");
    for (size_t x = 0; x < cols; x++) {
        if (trunc && x == half) {
            printf("  ...");
            x = cols - half - 1;
            continue;
        }

        double val;
        matrix_get(&val, matrix, z, y, x);
        printf("%8.2f", val);
        if (x + 1 < cols && !(trunc && x == cols - half - 1))
            printf(",");
    }
}

void matrix_print(const Matrix* matrix)
{
    if (matrix == nullptr) {
        printf("[nullptr]\n");
        return;
    }

    size_t depth = matrix->depth;
    size_t rows = matrix->rows;
    size_t max = MATRIX_PRINT_MAX;
    size_t half = max / 2;

    bool trunc_z = depth > max;
    bool trunc_y = rows > max;

    printf("Matrix (shape: %zu × %zu × %zu)\n", depth, rows, matrix->cols);
    printf("[");

    for (size_t z = 0; z < depth; z++) {
        if (!should_show(z, depth, max))
            continue;

        if (z != 0)
            printf("\n");
        printf("\n");

        if (trunc_z && z == half)
            printf("  ...\n\n");

        for (size_t y = 0; y < rows; y++) {
            if (!should_show(y, rows, max))
                continue;
            if (trunc_y && y == half)
                printf("   ...\n");
            print_row(matrix, z, y);
            printf("\n");
        }
    }

    printf("]\n");
}

void freq_print(const Matrix* freq, double bucket_size, size_t total)
{
    const size_t MAX_DISPLAY = 8;
    size_t buckets = freq->cols;
    bool truncated = buckets > MAX_DISPLAY;
    size_t half = MAX_DISPLAY / 2;

    printf("\nFrequency histogram:\n");
    for (size_t b = 0; b < buckets; b++) {

        if (truncated && b == half) {
            printf("    ...           |    ...\n");
            continue;
        }
        if (truncated && b > half && b < buckets - half)
            continue;

        double low = b * bucket_size;
        double high = low + bucket_size;
        double count;
        matrix_get(&count, freq, 0, 0, b);

        double pct = (count / (double)total) * 100.0;
        int bars = (int)((count / (double)total) * HIST_MAX_BARS);

        printf("[%7.1f-%7.1f) |", low, high);
        for (int k = 0; k < bars; k++)
            printf("█");
        printf(" %.0f (%.1f%%)\n", count, pct);
    }

    if (truncated)
        printf("\n(%zu omitted tracks)\n", buckets - MAX_DISPLAY);
}
