#ifndef EMU_CUBLAS_H
#define EMU_CUBLAS_H

#include <emu/cublas/handle.h>
#include <emu/cublas/type.h>

#include <cublas_v2.h>

namespace emu
{

namespace cublas
{

namespace detail
{
    template<typename T> struct AssociatedType;

    template<> struct AssociatedType<float>           { using type = float;  };
    template<> struct AssociatedType<double>          { using type = double; };
    template<> struct AssociatedType<cuComplex>       { using type = float;  };
    template<> struct AssociatedType<cuDoubleComplex> { using type = double; };

} // namespace detail

    template<typename T>
    using AssociatedT = typename detail::AssociatedType<T>::type;

    // Blas level 1

    template<typename T>
    void amax(handle_t handle, int n, const T *x, int incx, int *result);

    template<typename T>
    void amin(handle_t handle, int n, const T *x, int incx, int *result);

    template<typename T>
    void asum(handle_t handle, int n, const T *x, int incx, AssociatedT<T> *result);

    template<typename T>
    void axpy(handle_t handle, int n, const T *alpha, const T *x, int incx, T *y, int incy);

    template<typename T>
    void copy(handle_t handle, int n, const T *x, int incx, T *y, int incy);

    template<typename T>
    void dot (handle_t handle, int n, const T *x, int incx, const T *y, int incy, T *result);

    template<typename T>
    void dotu(handle_t handle, int n, const T *x, int incx, const T *y, int incy, T *result);

    template<typename T>
    void dotc(handle_t handle, int n, const T *x, int incx, const T *y, int incy, T *result);

    template<typename T>
    void nrm2(handle_t handle, int n, const T *x, int incx, T  *result);

    // template<typename TIn, typename TOut>
    // void rot (handle_t handle, int n, TOut *x, int incx, TOut *y, int incy, const TIn *c, const TIn *s);

    template<typename T>
    void rotg(handle_t handle, T *a, T *b, T  *c, T *s);

    template<typename T>
    void rotm(handle_t handle, int n, T  *x, int incx, T  *y, int incy, const T*  param);

    template<typename T>
    void rotmg(handle_t handle, T  *d1, T  *d2, T  *x1, const T  *y1, T  *param);

    template<typename T>
    void scal(handle_t handle, int n, const T *alpha, T *x, int incx);

    template<typename T>
    void swap(handle_t handle, int n, T *x, int incx, T *y, int incy);

    // Blas level 2

    template<typename T>
    void gbmv(handle_t handle, Operation trans, int m, int n, int kl, int ku, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void gemv(handle_t handle, Operation trans, int m, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void ger (handle_t handle, int m, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *A, int lda);

    template<typename T>
    void sbmv(handle_t handle, FillMode uplo, int n, int k, const T  *alpha, const T  *A, int lda, const T  *x, int incx, const T  *beta, T *y, int incy);

    template<typename T>
    void spmv(handle_t handle, FillMode uplo, int n, const T  *alpha, const T  *AP, const T  *x, int incx, const T  *beta, T  *y, int incy);

    template<typename T>
    void spr (handle_t handle, FillMode uplo, int n, const T  *alpha, const T  *x, int incx, T  *AP);

    template<typename T>
    void spr2(handle_t handle, FillMode uplo, int n, const T  *alpha, const T  *x, int incx, const T  *y, int incy, T  *AP);

    template<typename T>
    void symv(handle_t handle, FillMode uplo, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void syr (handle_t handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, T *A, int lda);

    template<typename T>
    void syr2(handle_t handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *A, int lda);

    template<typename T>
    void tbmv(handle_t handle, FillMode uplo, Operation trans, DiagonalType diag, int n, int k, const T *A, int lda, T *x, int incx);

    template<typename T>
    void tbsv(handle_t handle, FillMode uplo, Operation trans, DiagonalType diag, int n, int k, const T *A, int lda, T *x, int incx);

    template<typename T>
    void tpmv(handle_t handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *AP, T *x, int incx);

    template<typename T>
    void tpsv(handle_t handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *AP, T *x, int incx);

    template<typename T>
    void trmv(handle_t handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *A, int lda, T *x, int incx);

    template<typename T>
    void trsv(handle_t handle, FillMode uplo, Operation trans, DiagonalType diag, int n, const T *A, int lda, T *x, int incx);

    template<typename T>
    void hemv(handle_t handle, FillMode uplo, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void hbmv(handle_t handle, FillMode uplo, int n, int k, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void hpmv(handle_t handle, FillMode uplo, int n, const T *alpha, const T *AP, const T *x, int incx, const T *beta, T *y, int incy);

    template<typename T>
    void her (handle_t handle, FillMode uplo, int n, const T  *alpha, const T *x, int incx, T *A, int lda);

    template<typename T>
    void her2(handle_t handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *A, int lda);

    template<typename T>
    void hpr(handle_t handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, T *AP);

    template<typename T>
    void hpr2(handle_t handle, FillMode uplo, int n, const T *alpha, const T *x, int incx, const T *y, int incy, T *AP);

    // Blas level 2

    template<typename T>
    void gemm(handle_t handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void gemm3m(handle_t handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void gemmBatched(handle_t handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *Aarray[], int lda, const T *Barray[], int ldb, const T *beta, T *Carray[], int ldc, int batchCount);

    template<typename T>
    void tridedBatched(handle_t handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *A, int lda, long long int strideA, const T *B, int ldb, long long int strideB, const T *beta, T *C, int ldc, long long int strideC, int batchCount);

    template<typename T>
    void symm(handle_t handle, SideMode side, FillMode uplo, int m, int n, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void syrk(handle_t handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *beta, T *C, int ldc);

    template<typename T>
    void syr2k(handle_t handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void syrkx(handle_t handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void trmm(handle_t handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const T *alpha, const T *A, int lda, const T *B, int ldb, T *C, int ldc);

    template<typename T>
    void trsm(handle_t handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const T *alpha, const T *A, int lda, T *B, int ldb);

    template<typename T>
    void trsmBatched(handle_t handle, SideMode  side, FillMode uplo, Operation trans, DiagonalType  diag, int m, int n, const T *alpha, T *A[], int lda, T *B[], int ldb, int batchCount);

    template<typename T>
    void hemm(handle_t handle, SideMode side, FillMode uplo, int m, int n, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void herk(handle_t handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *beta, T *C, int ldc);

    template<typename T>
    void her2k(handle_t handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc);

    template<typename T>
    void herkx(handle_t handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T  *beta, T *C, int ldc);

    // Blas like extension

    // Coming soon...

} // namespace cublas

} // namespace emu

#endif // EMU_CUBLAS_H
