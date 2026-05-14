#include "matrix_utils.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <time.h>
#endif

static double current_time_seconds(void)
{
#ifdef _WIN32
    static LARGE_INTEGER frequency;
    static int initialized = 0;
    LARGE_INTEGER counter;

    if (!initialized) {
        QueryPerformanceFrequency(&frequency);
        initialized = 1;
    }

    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
#else
    return (double)clock() / (double)CLOCKS_PER_SEC;
#endif
}

double benchmark_multiply(void (*multiply)(const float*, const float*, float*, size_t),
    const float* a,
    const float* b,
    float* c,
    size_t n)
{
    const double start = current_time_seconds();
    multiply(a, b, c, n);
    return current_time_seconds() - start;
}

double benchmark_blocked(const float* a,
    const float* b,
    float* c,
    size_t n,
    size_t block_size)
{
    const double start = current_time_seconds();
    multiply_blocked(a, b, c, n, block_size);
    return current_time_seconds() - start;
}

static int run_single_size(size_t n)
{
    const size_t block_size = 32;
    float* a = matrix_allocate(n);
    float* b = matrix_allocate(n);
    float* c_naive = matrix_allocate(n);
    float* c_blocked = matrix_allocate(n);
    float* c_simd = matrix_allocate(n);
    float* c_assembly = matrix_allocate(n);
    double naive_seconds;
    double blocked_seconds;
    double simd_seconds;
    double assembly_seconds;
    double blocked_speedup;
    double simd_speedup;
    double assembly_speedup;
    int blocked_correct;
    int simd_correct;
    int assembly_correct;

    if (a == NULL || b == NULL || c_naive == NULL || c_blocked == NULL || c_simd == NULL ||
        c_assembly == NULL) {
        printf("N = %zu: allocation failed\n", n);
        matrix_free(a);
        matrix_free(b);
        matrix_free(c_naive);
        matrix_free(c_blocked);
        matrix_free(c_simd);
        matrix_free(c_assembly);
        return 0;
    }

    matrix_initialize(a, n);
    matrix_initialize(b, n);

    naive_seconds = benchmark_multiply(multiply_naive, a, b, c_naive, n);
    blocked_seconds = benchmark_blocked(a, b, c_blocked, n, block_size);
    simd_seconds = benchmark_multiply(multiply_simd_avx2, a, b, c_simd, n);
    assembly_seconds = benchmark_multiply(multiply_assembly_avx2, a, b, c_assembly, n);
    blocked_correct = matrix_compare(c_naive, c_blocked, n, 0.001f);
    simd_correct = matrix_compare(c_naive, c_simd, n, 0.001f);
    assembly_correct = matrix_compare(c_naive, c_assembly, n, 0.001f);
    blocked_speedup = blocked_seconds > 0.0 ? naive_seconds / blocked_seconds : 0.0;
    simd_speedup = simd_seconds > 0.0 ? naive_seconds / simd_seconds : 0.0;
    assembly_speedup = assembly_seconds > 0.0 ? naive_seconds / assembly_seconds : 0.0;

    printf("%6zu | %-20s | %12.6f | %9s | %11s\n",
        n,
        "Naive",
        naive_seconds,
        "1.00x",
        "Reference");
    printf("%6zu | %-20s | %12.6f | %8.2fx | %11s\n",
        n,
        "Blocked/tiled",
        blocked_seconds,
        blocked_speedup,
        blocked_correct ? "PASS" : "FAIL");
    printf("%6zu | %-20s | %12.6f | %8.2fx | %11s\n",
        n,
        "AVX2 SIMD",
        simd_seconds,
        simd_speedup,
        simd_correct ? "PASS" : "FAIL");
    printf("%6zu | %-20s | %12.6f | %8.2fx | %11s\n",
        n,
        "Assembly AVX2 Kernel",
        assembly_seconds,
        assembly_speedup,
        assembly_correct ? "PASS" : "FAIL");

    matrix_free(a);
    matrix_free(b);
    matrix_free(c_naive);
    matrix_free(c_blocked);
    matrix_free(c_simd);
    matrix_free(c_assembly);

    return blocked_correct && simd_correct && assembly_correct;
}

static int tune_block_size(size_t n)
{
    static const size_t block_sizes[] = { 8, 16, 32, 64 };
    const size_t block_size_count = sizeof(block_sizes) / sizeof(block_sizes[0]);
    float* a = matrix_allocate(n);
    float* b = matrix_allocate(n);
    float* c_naive = matrix_allocate(n);
    float* c_blocked = matrix_allocate(n);
    double naive_seconds;
    double best_seconds = 0.0;
    size_t best_block_size = 0;
    size_t i;

    if (a == NULL || b == NULL || c_naive == NULL || c_blocked == NULL) {
        printf("N = %zu: allocation failed during block-size tuning\n", n);
        matrix_free(a);
        matrix_free(b);
        matrix_free(c_naive);
        matrix_free(c_blocked);
        return 0;
    }

    matrix_initialize(a, n);
    matrix_initialize(b, n);

    naive_seconds = benchmark_multiply(multiply_naive, a, b, c_naive, n);

    printf("N = %zu\n", n);
    printf("%10s | %12s | %9s\n", "Block size", "Time (sec)", "Speedup");
    printf("-----------+--------------+----------\n");

    for (i = 0; i < block_size_count; ++i) {
        const size_t block_size = block_sizes[i];
        const double blocked_seconds = benchmark_blocked(a, b, c_blocked, n, block_size);
        const double speedup = blocked_seconds > 0.0 ? naive_seconds / blocked_seconds : 0.0;

        if (best_block_size == 0 || blocked_seconds < best_seconds) {
            best_seconds = blocked_seconds;
            best_block_size = block_size;
        }

        printf("%10zu | %12.6f | %8.2fx\n", block_size, blocked_seconds, speedup);
    }

    printf("Selected optimal block size for N = %zu: %zu\n\n", n, best_block_size);

    matrix_free(a);
    matrix_free(b);
    matrix_free(c_naive);
    matrix_free(c_blocked);

    return 1;
}

void run_benchmarks(void)
{
    static const size_t sizes[] = { 128, 256, 512, 1024 };
    static const size_t tuning_sizes[] = { 512, 1024 };
    const size_t size_count = sizeof(sizes) / sizeof(sizes[0]);
    const size_t tuning_size_count = sizeof(tuning_sizes) / sizeof(tuning_sizes[0]);
    size_t i;

    printf("High-Performance Matrix Multiplication\n");
    printf("Float square matrices, row-major layout\n");
    printf("Correctness is checked against the naive reference result.\n");
    printf("Blocked/tiled uses a block size of 32.\n\n");

    printf("%6s | %-20s | %12s | %9s | %11s\n",
        "N",
        "Implementation",
        "Time (sec)",
        "Speedup",
        "Correctness");
    printf("-------+----------------------+--------------+-----------+-------------\n");

    for (i = 0; i < size_count; ++i) {
        if (!run_single_size(sizes[i])) {
            exit(EXIT_FAILURE);
        }

        if (i + 1 < size_count) {
            printf("-------+----------------------+--------------+-----------+-------------\n");
        }
    }

    printf("\nBlocked/Tiled Block-Size Tuning\n");
    printf("Naive timing is used as the speedup baseline for each matrix size.\n\n");

    for (i = 0; i < tuning_size_count; ++i) {
        if (!tune_block_size(tuning_sizes[i])) {
            exit(EXIT_FAILURE);
        }
    }
}
