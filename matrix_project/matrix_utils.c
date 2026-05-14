#include "matrix_utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float* matrix_allocate(size_t n)
{
    const size_t element_count = n * n;
    return (float*)malloc(element_count * sizeof(float));
}

void matrix_free(float* matrix)
{
    free(matrix);
}

void matrix_initialize(float* matrix, size_t n)
{
    size_t row;
    size_t col;

 
    for (row = 0; row < n; ++row) {
        for (col = 0; col < n; ++col) {
            matrix[row * n + col] = (float)(((row + 1) * (col + 3)) % 17) / 17.0f;
        }
    }
}

void matrix_zero(float* matrix, size_t n)
{
    memset(matrix, 0, n * n * sizeof(float));
}

int matrix_compare(const float* a, const float* b, size_t n, float epsilon)
{
    size_t i;
    const size_t element_count = n * n;

    for (i = 0; i < element_count; ++i) {
        if (fabsf(a[i] - b[i]) > epsilon) {
            return 0;
        }
    }

    return 1;
}

void matrix_print(const float* matrix, size_t n)
{
    size_t row;
    size_t col;

    for (row = 0; row < n; ++row) {
        for (col = 0; col < n; ++col) {
            printf("%8.3f ", matrix[row * n + col]);
        }
        printf("\n");
    }
}
