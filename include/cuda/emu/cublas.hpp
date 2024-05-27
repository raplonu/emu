#pragma once

#include <emu/cublas/handle.hpp>
#include <emu/cublas/type.hpp>

#include <cublas_v2.h>

namespace emu::cublas
{

// follow blas naming convention for readability. lint naming checks are disabled for this section
// NOLINTBEGIN(readability-identifier-naming)

    // Blas level 1

    template<typename T>
    void amax(const handle_t & handle, int n, const T *x, int incx, int *result);

    template<typename T>
    void amin(const handle_t & handle, int n, const T *x, int incx, int *result);

    template<typename T>
    void asum(const handle_t & handle, int n, const T *x, int incx, AssociatedT<T> *result);

    template<typename T>
    void axpy(const handle_t & handle, int n, const T *alpha, const T *x, int incx, T *y, int incy);

    template<typename T>
    void copy(const handle_t & handle, int n, const T *x, int incx, T *y, int incy);

    template<typename T>
    void dot (const handle_t & handle, int n, const T *x, int incx, const T *y, int incy, T *result);

    template<typename T>
    void dotu(const handle_t & handle, int n, const T *x, int incx, const T *y, int incy, T *result);

    template<typename T>
    void dotc(const handle_t & handle, int n, const T *x, int incx, const T *y, int incy, T *result);

    template<typename T>
    void nrm2(const handle_t & handle, int n, const T *x, int incx, T  *result);

    // template<typename TIn, typename TOut>
    // void rot (const handle_t & handle, int n, TOut *x, int incx, TOut *y, int incy, const TIn *c, const TIn *s);

    template<typename T>
    void rotg(const handle_t & handle, T *a, T *b, T  *c, T *s);

    template<typename T>
    void rotm(const handle_t & handle, int n, T  *x, int incx, T  *y, int incy, const T*  param);

    template<typename T>
    void rotmg(const handle_t & handle, T  *d1, T  *d2, T  *x1, const T  *y1, T  *param);

    template<typename T>
    void scal(const handle_t & handle, int n, const T *alpha, T *x, int incx);

    template<typename T>
    void swap(const handle_t & handle, int n, T *x, int incx, T *y, int incy);

    // Blas level 2

    template<typename T>
    void gbmv(const handle_t & handle, Operation trans, int m, int n, int kl, int ku, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void gemv(const handle_t & handle, Operation trans, int m, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void ger (const handle_t & handle, int m, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *A, int lda);

    template<typename T>
    void sbmv(const handle_t & handle, FillMode uplo, int n, int k, const T  *alpha, const T  *A, int lda, const T  *x, int incx, const T  *beta, T *y, int incy);

    template<typename T>
    void spmv(const handle_t & handle, FillMode uplo, int n, const T  *alpha, const T  *AP, const T  *x, int incx, const T  *beta, T  *y, int incy);

    template<typename T>
    void spr (const handle_t & handle, FillMode uplo, int n, const T  *alpha, const T  *x, int incx, T  *AP);

    template<typename T>
    void spr2(const handle_t & handle, FillMode uplo, int n, const T  *alpha, const T  *x, int incx, const T  *y, int incy, T  *AP);

    template<typename T>
    void symv(const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void syr (const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, T *A, int lda);

    template<typename T>
    void syr2(const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *A, int lda);

    template<typename T>
    void tbmv(const handle_t & handle, FillMode uplo, Operation trans, DiagonalType diag, int n, int k, const T *A, int lda, T *x, int incx);

    template<typename T>
    void tbsv(const handle_t & handle, FillMode uplo, Operation trans, DiagonalType diag, int n, int k, const T *A, int lda, T *x, int incx);

    template<typename T>
    void tpmv(const handle_t & handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *AP, T *x, int incx);

    template<typename T>
    void tpsv(const handle_t & handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *AP, T *x, int incx);

    template<typename T>
    void trmv(const handle_t & handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *A, int lda, T *x, int incx);

    template<typename T>
    void trsv(const handle_t & handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *A, int lda, T *x, int incx);

    template<typename T>
    void hemv(const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void hbmv(const handle_t & handle, FillMode uplo, int n, int k, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void hpmv(const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *AP, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void her (const handle_t & handle, FillMode uplo, int n, const T  *alpha, const T *x, int incx, T *A, int lda);

    template<typename T>
    void her2(const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *A, int lda);

    template<typename T>
    void hpr(const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, T *AP);

    template<typename T>
    void hpr2(const handle_t & handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *AP);

    // Blas level 3

    template<typename T>
    void gemm(const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void gemm3m(const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void gemmBatched(const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *Aarray[], int lda, const T *Barray[], int ldb, const T *beta, T *Carray[], int ldc, int batchCount);

    template<typename T>
    void tridedBatched(const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *A, int lda, long long int strideA, const T *B, int ldb, long long int strideB, const T *beta, T *C, int ldc, long long int strideC, int batchCount);

    template<typename T>
    void symm(const handle_t & handle, SideMode side, FillMode uplo, int m, int n, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void syrk(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *beta, T *C, int ldc);

    template<typename T>
    void syr2k(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void syrkx(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    // Blas-like extension

    template<typename T>
    void dgmm(const handle_t & handle, SideMode side, int m, int n, const T *A, int lda, const T *x, int incx, T *C, int ldc);

    template<typename T>
    void trmm(const handle_t & handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const T *alpha, const T *A, int lda, const T *B, int ldb, T *C, int ldc);

    template<typename T>
    void trsm(const handle_t & handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const T *alpha, const T *A, int lda, T *B, int ldb);

    template<typename T>
    void trsmBatched(const handle_t & handle, SideMode  side, FillMode uplo, Operation trans, DiagonalType  diag, int m, int n, const T *alpha, T *A[], int lda, T *B[], int ldb, int batchCount);

    template<typename T>
    void hemm(const handle_t & handle, SideMode side, FillMode uplo, int m, int n, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void herk(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *beta, T *C, int ldc);

    template<typename T>
    void her2k(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void herkx(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T  *beta, T *C, int ldc);

    template<typename T>
    void getrf_batched(const handle_t & handle, int n, T *A_array[], int lda, int *pivot_array, int *info_array, int batch_size);

    template<typename T>
    void getri_batched(const handle_t & handle, int n, T *A_array[], int lda, int *pivot_array, T *c_array[], int ldc, int *info_array, int batch_size);

// NOLINTEND(readability-identifier-naming)

} // namespace emu::cublas
