#include "matrix_utils.h"

#include <immintrin.h>

void multiply_simd_avx2(const float* a, const float* b, float* c, size_t n)
{
    size_t i;
    size_t k;

    matrix_zero(c, n);

    for (i = 0; i < n; ++i) {
        for (k = 0; k < n; ++k) {
            size_t j = 0;
            const __m256 a_values = _mm256_set1_ps(a[i * n + k]);

            for (; j + 8 <= n; j += 8) {
                const __m256 b_values = _mm256_loadu_ps(&b[k * n + j]);
                const __m256 c_values = _mm256_loadu_ps(&c[i * n + j]);
                const __m256 product = _mm256_mul_ps(a_values, b_values);
                const __m256 sum = _mm256_add_ps(c_values, product);

                _mm256_storeu_ps(&c[i * n + j], sum);
            }

          
            for (; j < n; ++j) {
                c[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }
}
