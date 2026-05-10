#include "include/matrix.h"
#include "matrix.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUCKETS 10
#define BUCKET_SIZE 10.0
#define VALUE_MAX 100.0

#define SHAPE_DEPTH 2
#define SHAPE_ROWS 6
#define SHAPE_COLS 10

int example(void);

int main(void)
{
    return example();
}

int example(void)
{
    // For demonstration purposes only.
    srand((unsigned)time(nullptr));

    // Allocates data and frequency matrix (histogram 1×1×BUCKETS)
    Matrix* m = matrix_create(SHAPE_DEPTH, SHAPE_ROWS, SHAPE_COLS);
    Matrix* freq = matrix_create(1, 1, BUCKETS);
    if (!m || !freq) {
        fprintf(stderr, "Error: Allocation failed.\n");
        matrix_free(m);
        matrix_free(freq);
        return 1;
    }

    // Zero frequency explicitly
    for (size_t b = 0; b < BUCKETS; b++)
        matrix_set(freq, 0, 0, b, 0.0);

    size_t capacity = m->depth * m->rows * m->cols;
    double sum = 0.0, min_val = VALUE_MAX, max_val = 0.0;

    // Loop 1: Simultaneous Writing, Reading, and Accumulation
    for (size_t z = 0; z < m->depth; z++)
        for (size_t y = 0; y < m->rows; y++)
            for (size_t x = 0; x < m->cols; x++) {
                double stored_value = ((double)rand() / RAND_MAX) * VALUE_MAX;
                matrix_set(m, z, y, x, stored_value);

                double read_value;
                matrix_get(&read_value, m, z, y, x);

                sum += read_value;
                min_val = read_value < min_val ? read_value : min_val;
                max_val = read_value > max_val ? read_value : max_val;

                // Increments the corresponding bucket in the frequency matrix.
                size_t bucket = (size_t)(read_value / BUCKET_SIZE);
                if (bucket >= BUCKETS)
                    bucket = BUCKETS - 1; // clamp to VALUE_MAX exactly
                double freq_val;
                matrix_get(&freq_val, freq, 0, 0, bucket);
                matrix_set(freq, 0, 0, bucket, freq_val + 1.0);
            }

    double mean = sum / (double)capacity;
    double variance = 0.0;

    // Full data display (Optional, indirect loop)
    matrix_print(m);

    // Loop 2: simultaneous display and variance
    printf("Matrix (shape: %zu × %zu × %zu)\n", m->depth, m->rows, m->cols);
    printf("[");
    for (size_t z = 0; z < m->depth; z++) {
        if (z != 0)
            printf("\n\n ");
        else
            printf("\n ");

        for (size_t y = 0; y < m->rows; y++) {
            if (y != 0)
                printf("\n ");
            for (size_t x = 0; x < m->cols; x++) {
                double val;
                matrix_get(&val, m, z, y, x);

                printf("%8.2f", val);
                if (x + 1 < m->cols)
                    printf(", ");

                double diff = val - mean;
                variance += diff * diff;
            }
        }
    }
    printf("\n]\n");

    variance /= (double)capacity;
    double standard_deviation = sqrt(variance);

    // Histogram
    freq_print(freq, BUCKET_SIZE, capacity);

    // Statistical Details
    printf("\n── Details ─────────────────────\n");
    printf("  Shape   : %zu × %zu × %zu\n", m->depth, m->rows, m->cols);
    printf("  Elements: %zu\n", capacity);
    printf("  Sum     : %.4f\n", sum);
    printf("  Mean    : %.4f\n", mean);
    printf("  Minimum : %.4f\n", min_val);
    printf("  Maximum : %.4f\n", max_val);
    printf("  Variance: %.4f\n", variance);
    printf("  Std Dev : %.4f\n", standard_deviation);
    printf("─────────────────────────────────\n");

    matrix_free(m);
    matrix_free(freq);

    return 0;
}
