#include "matrix_utils.h"

static size_t min_size(size_t a, size_t b)
{
    return a < b ? a : b;
}

void multiply_blocked(const float* a, const float* b, float* c, size_t n, size_t block_size)
{
    size_t ii;
    size_t kk;
    size_t jj;

    matrix_zero(c, n);

  
    for (ii = 0; ii < n; ii += block_size) {
        for (kk = 0; kk < n; kk += block_size) {
            for (jj = 0; jj < n; jj += block_size) {
                const size_t i_end = min_size(ii + block_size, n);
                const size_t k_end = min_size(kk + block_size, n);
                const size_t j_end = min_size(jj + block_size, n);
                size_t i;
                size_t k;
                size_t j;

                for (i = ii; i < i_end; ++i) {
                    for (k = kk; k < k_end; ++k) {
                        const float a_value = a[i * n + k];

                        for (j = jj; j < j_end; ++j) {
                            c[i * n + j] += a_value * b[k * n + j];
                        }
                    }
                }
            }
        }
    }
}
