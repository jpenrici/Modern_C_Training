#pragma once

#include <stdbool.h>
#include <stddef.h>

#define HIST_MAX_BARS 20.0

typedef struct {
    size_t depth; // 1 for 2D, > 1 for 3D
    size_t rows;
    size_t cols;
    double data[];
} Matrix;

// Lifecycle
[[nodiscard]] Matrix* matrix_create(size_t depth, size_t rows, size_t cols);
void matrix_free(Matrix* matrix);

// Operations
bool matrix_set(Matrix* matrix, size_t z, size_t y, size_t x, double value);
bool matrix_get(double* result, const Matrix* matrix, size_t z, size_t y, size_t x);

bool matrix_scale(Matrix* matrix, double scalar);
bool matrix_fill(Matrix* matrix, double value);

bool matrix_sum(double* result, const Matrix* matrix);

// Display
#define MATRIX_PRINT_MAX 6
void matrix_print(const Matrix* matrix);

void freq_print(const Matrix* freq, double bucket_size, size_t total);

// Variable behavior
#ifdef UNIT_TESTING
#define STATIC
#else
#define STATIC static
#endif

STATIC size_t matrix_capacity(const Matrix* matrix);
STATIC size_t matrix_get_index(const Matrix* matrix, size_t z, size_t y, size_t x);
