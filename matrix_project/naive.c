#include "matrix_utils.h"

void multiply_naive(const float* a, const float* b, float* c, size_t n)
{
    size_t i;
    size_t k;
    size_t j;

    matrix_zero(c, n);

  
    for (i = 0; i < n; ++i) {
        for (k = 0; k < n; ++k) {
            const float a_value = a[i * n + k];

            for (j = 0; j < n; ++j) {
                c[i * n + j] += a_value * b[k * n + j];
            }
        }
    }
}
