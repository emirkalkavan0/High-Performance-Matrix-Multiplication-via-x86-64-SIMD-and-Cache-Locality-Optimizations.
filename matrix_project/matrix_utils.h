#pragma once

#include <stddef.h>


float* matrix_allocate(size_t n);
void matrix_free(float* matrix);

void matrix_initialize(float* matrix, size_t n);
void matrix_zero(float* matrix, size_t n);
int matrix_compare(const float* a, const float* b, size_t n, float epsilon);
void matrix_print(const float* matrix, size_t n);

void multiply_naive(const float* a, const float* b, float* c, size_t n);
void multiply_blocked(const float* a, const float* b, float* c, size_t n, size_t block_size);
void multiply_simd_avx2(const float* a, const float* b, float* c, size_t n);
void multiply_assembly_avx2(const float* a, const float* b, float* c, size_t n);

double benchmark_multiply(void (*multiply)(const float*, const float*, float*, size_t),
    const float* a,
    const float* b,
    float* c,
    size_t n);

double benchmark_blocked(const float* a,
    const float* b,
    float* c,
    size_t n,
    size_t block_size);

void run_benchmarks(void);
