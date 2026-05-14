OPTION CASEMAP:NONE

PUBLIC asm_avx2_row_kernel

.code


asm_avx2_row_kernel PROC
    test    r9, r9
    jz      done

    vbroadcastss ymm0, DWORD PTR [r8]

    xor     r10, r10

vector_loop:
   
    mov     r11, r9
    sub     r11, r10
    cmp     r11, 8
    jb      scalar_loop

    vmovups ymm1, YMMWORD PTR [rcx + r10 * 4]
    vmovups ymm2, YMMWORD PTR [rdx + r10 * 4]

    vmulps  ymm1, ymm1, ymm0
    vaddps  ymm2, ymm2, ymm1

    vmovups YMMWORD PTR [rdx + r10 * 4], ymm2

    add     r10, 8
    cmp     r10, r9
    jb      vector_loop
    jmp     done

scalar_loop:
    cmp     r10, r9
    jae     done

    vmovss  xmm1, DWORD PTR [rcx + r10 * 4]
    vmulss  xmm1, xmm1, DWORD PTR [r8]
    vaddss  xmm1, xmm1, DWORD PTR [rdx + r10 * 4]
    vmovss  DWORD PTR [rdx + r10 * 4], xmm1

    inc     r10
    jmp     scalar_loop

done:
    vzeroupper
    ret
asm_avx2_row_kernel ENDP

END
