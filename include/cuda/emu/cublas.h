#ifndef EMU_CUBLAS_H
#define EMU_CUBLAS_H

#include <emu/cublas/handle.h>

#include <cublas_v2.h>

namespace emu
{

namespace cublas
{

    enum class Operation {
        N,
        T,
        C,
        Hermitan,
        Conjg
    };

    enum class SideMode {
        Left,
        Right
    };

    enum class FillMode {
        Lower,
        Upper,
        Full
    };

    enum class DiagonalType {
        NonUnit,
        Unit
    };

    // Blas level 1

    template<typename T>
    void amax(handle_t handle, int n, const T *x, int incx, int *result);

    template<typename T>
    void amin(handle_t handle, int n, const T *x, int incx, int *result);

    template<typename T>
    void asum(handle_t handle, int n, const T *x, int incx, T  *result);

    template<typename T>
    void axpy(handle_t handle, int n, const T *alpha, const T *x, int incx, T *y, int incy);

    template<typename T>
    void copy(handle_t handle, int n, const T *x, int incx, T *y, int incy);

    template<typename T>
    void dot (handle_t handle, int n, const T *x, int incx, const T *y, int incy, T *result);

    template<typename T>
    void nrm2(handle_t handle, int n, const T *x, int incx, T  *result);

    template<typename T>
    void rot (handle_t handle, int n, T *x, int incx, T *y, int incy, const T  *c, const T *s);

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
    void hemv(handle_t handle, FillMode uplo int n, const T *alpha const T *A, int lda const T *x, int incx const T *beta T *y, int incy);

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


















template <class T_data>
int where_amax(cublasHandle_t cublas_handle, int n, const T_data *vect,
                     int incx);

template <class T_data>
int where_amin(cublasHandle_t cublas_handle, int n, const T_data *vect,
                     int incx);

template <class T_data>
T_data getasum(cublasHandle_t cublas_handle, int n, const T_data *vect,
                     int incx);

template <class T_data>
cublasStatus_t axpy(cublasHandle_t cublas_handle, int n,
                          const T_data alpha, const T_data *vectx, int incx,
                          T_data *vecty, int incy);

template <class T_data>
T_data dot(cublasHandle_t cublas_handle, int n, T_data *vectx, int incx,
                 T_data *vecty, int incy);

template <class T_data>
T_data nrm2(cublasHandle_t cublas_handle, int n, T_data *vect, int incx);

template <class T_data>
cublasStatus_t rot(cublasHandle_t cublas_handle, int n, T_data *vectx,
                         int incx, T_data *vecty, int incy, T_data sc,
                         T_data ss);

template <class T_data>
cublasStatus_t scal(cublasHandle_t cublas_handle, int n, T_data alpha,
                          T_data *vectx, int incx);

template <class T_data>
cublasStatus_t swap(cublasHandle_t cublas_handle, int n, T_data *vectx,
                          int incx, T_data *vecty, int incy);

template <class T_data>
cublasStatus_t copy(cublasHandle_t cublas_handle, int n,
                          const T_data *vectx, int incx, T_data *vecty,
                          int incy);

template <class T_data>
cublasStatus_t gemv(cublasHandle_t cublas_handle, char trans, int m,
                          int n, T_data alpha, T_data *matA, int lda,
                          T_data *vectx, int incx, T_data beta, T_data *vecty,
                          int incy);

template <class T_data>
cublasStatus_t symv(cublasHandle_t cublas_handle, char uplo, int n,
                          T_data alpha, T_data *matA, int lda, T_data *vectx,
                          int incx, T_data beta, T_data *vecty, int incy);

template <class T_data>
cublasStatus_t ger(cublasHandle_t cublas_handle, int m, int n,
                         T_data alpha, T_data *vectx, int incx, T_data *vecty,
                         int incy, T_data *matA, int lda);

template <class T_data>
cublasStatus_t gemm(cublasHandle_t cublas_handle, char transa,
                          char transb, int m, int n, int k, T_data alpha,
                          T_data *matA, int lda, T_data *matB, int ldb,
                          T_data beta, T_data *matC, int ldc);

template <class T_data>
cublasStatus_t symm(cublasHandle_t cublas_handle, char side, char uplo,
                          int m, int n, T_data alpha, T_data *matA, int lda,
                          T_data *matB, int ldb, T_data beta, T_data *matC,
                          int ldc);

template <class T_data>
cublasStatus_t syrk(cublasHandle_t cublas_handle, char uplo, char transa,
                          int n, int k, T_data alpha, T_data *matA, int lda,
                          T_data beta, T_data *matC, int ldc);

template <class T_data>
cublasStatus_t syrkx(cublasHandle_t cublas_handle, char uplo, char transa,
                           int n, int k, T_data alpha, T_data *matA, int lda,
                           T_data *matB, int ldb, T_data beta, T_data *matC,
                           int ldc);

template <class T_data>
cublasStatus_t geam(cublasHandle_t cublas_handle, char transa,
                          char transb, int m, int n, T_data alpha, T_data *matA,
                          int lda, T_data beta, T_data *matB, int ldb,
                          T_data *matC, int ldc);

template <class T_data>
cublasStatus_t dgmm(cublasHandle_t cublas_handle, char side, int m, int n,
                          const T_data *matA, int lda, const T_data *vectx,
                          int incx, T_data *matC, int ldc);

} // namespace cublas

} // namespace emu

#endif // EMU_CUBLAS_H
