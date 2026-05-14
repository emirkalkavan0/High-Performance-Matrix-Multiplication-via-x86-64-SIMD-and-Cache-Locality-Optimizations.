#include "matrix_utils.h"

#if defined(_M_X64)
void asm_avx2_row_kernel(const float* b_row, float* c_row, const float* a_value, size_t n);
#endif

void multiply_assembly_avx2(const float* a, const float* b, float* c, size_t n)
{
    size_t i;
    size_t k;

    matrix_zero(c, n);

    for (i = 0; i < n; ++i) {
        for (k = 0; k < n; ++k) {
#if defined(_M_X64)
            asm_avx2_row_kernel(&b[k * n], &c[i * n], &a[i * n + k], n);
#else
            size_t j;
            const float a_value = a[i * n + k];

            for (j = 0; j < n; ++j) {
                c[i * n + j] += a_value * b[k * n + j];
            }
#endif
        }
    }
}
