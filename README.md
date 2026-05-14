# High-Performance Matrix Multiplication via x86-64 SIMD and Cache-Locality Optimizations

## Project Overview

This project investigates different optimization strategies for square matrix multiplication on modern x86-64 processors.

The goal is to analyze how architectural optimizations such as cache blocking, SIMD vectorization, and low-level assembly kernels affect execution performance.

The project compares four implementations:

1. Naive matrix multiplication
2. Blocked/Tiled matrix multiplication
3. AVX2 SIMD implementation using intrinsics
4. x86-64 Assembly AVX2 kernel

---

## Implemented Optimizations

### 1. Naive Implementation

The baseline implementation uses the traditional triple nested loop approach.

### 2. Blocked/Tiled Multiplication

Matrices are divided into smaller blocks to improve cache locality and reduce cache misses.

### 3. AVX2 SIMD Vectorization

AVX2 intrinsics from `<immintrin.h>` are used to process multiple float values simultaneously using 256-bit SIMD registers.

### 4. Assembly AVX2 Kernel

A low-level x86-64 assembly kernel was implemented using MASM and AVX2 instructions for the performance-critical inner computation.

---

## Benchmark Features

The benchmark system measures:

- Execution time
- Speedup versus naive baseline
- Correctness validation

Matrix sizes tested:

- 128 x 128
- 256 x 256
- 512 x 512
- 1024 x 1024

The blocked/tiled implementation also includes automatic block-size tuning.

---

## Technologies Used

- C
- x86-64 Assembly
- AVX2 SIMD
- MASM
- Visual Studio 2026
- MSBuild

---

## Build Configuration

The project is configured for:

- Release mode
- x64 architecture
- AVX2 enabled (`/arch:AVX2`)

---

## Running the Project

1. Open the project in Visual Studio.
2. Select:
   - Release
   - x64
3. Build and run the project.

The console output will display:
- execution times
- speedups
- correctness checks
- block-size tuning results

---

## Example Results

The AVX2 SIMD implementation achieved over 5x speedup compared to the naive baseline on some matrix sizes.

All optimized implementations passed correctness validation against the naive reference implementation.

---

## Authors

- Ahmet Uğur Gültekin
- Emir Kalkavan