#include <emu/cublas.hpp>
#include <emu/cublas/error.hpp>

#include <cuda_fp16.h>

namespace emu::cublas
{

    // gemm syrk trsm potrf (cusolver)

    template<typename T> struct CuBLAS;

    template<>
    struct CuBLAS<__half> {

    };

    template<>
    struct CuBLAS<float> {
        // Level 1
        constexpr static auto amax = cublasIsamax;
        constexpr static auto amin = cublasIsamin;
        constexpr static auto asum = cublasSasum;
        constexpr static auto axpy = cublasSaxpy;
        constexpr static auto scal = cublasSscal;

        // Level 2
        constexpr static auto gemv = cublasSgemv;

        // Level 3
        constexpr static auto gemm = cublasSgemm;
        constexpr static auto syrk = cublasSsyrk;
        constexpr static auto trsm = cublasStrsm;
        constexpr static auto getrf_batched = cublasSgetrfBatched;
        constexpr static auto getri_batched = cublasSgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasSdgmm;
    };

    template<>
    struct CuBLAS<double> {
        // Level 1
        constexpr static auto amax = cublasIdamax;
        constexpr static auto amin = cublasIdamin;
        constexpr static auto asum = cublasDasum;
        constexpr static auto axpy = cublasDaxpy;
        constexpr static auto scal = cublasDscal;

        //  Level 2
        constexpr static auto gemv = cublasDgemv;

        // Level 3
        constexpr static auto gemm = cublasDgemm;
        constexpr static auto syrk = cublasDsyrk;
        constexpr static auto trsm = cublasDtrsm;
        constexpr static auto getrf_batched = cublasDgetrfBatched;
        constexpr static auto getri_batched = cublasDgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasDdgmm;
    };

    template<>
    struct CuBLAS<cuComplex> {
        // Level 1
        constexpr static auto amax = cublasIcamax;
        constexpr static auto amin = cublasIcamin;
        constexpr static auto asum = cublasScasum;
        constexpr static auto axpy = cublasCaxpy;
        constexpr static auto scal = cublasCscal;

        // Level 2
        constexpr static auto gemv = cublasCgemv;

        // Level 3
        constexpr static auto gemm = cublasCgemm;
        constexpr static auto syrk = cublasCsyrk;
        constexpr static auto trsm = cublasCtrsm;
        constexpr static auto getrf_batched = cublasCgetrfBatched;
        constexpr static auto getri_batched = cublasCgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasCdgmm;
    };

    template<>
    struct CuBLAS<cuDoubleComplex> {
        // Level 1
        constexpr static auto amax = cublasIzamax;
        constexpr static auto amin = cublasIzamin;
        constexpr static auto asum = cublasDzasum;
        constexpr static auto axpy = cublasZaxpy;
        constexpr static auto scal = cublasZscal;

        // Level 2
        constexpr static auto gemv = cublasZgemv;

        // Level 3
        constexpr static auto gemm = cublasZgemm;
        constexpr static auto syrk = cublasZsyrk;
        constexpr static auto trsm = cublasZtrsm;
        constexpr static auto getrf_batched = cublasZgetrfBatched;
        constexpr static auto getri_batched = cublasZgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasZdgmm;
    };

// NOLINTBEGIN(readability-identifier-naming)

    template<typename T>
    void amax(const handle_t & handle, int n, const T *x, int incx, int *result) {
        throw_if_error(CuBLAS<T>::amax(handle.enable().id(), n, x, incx, result));
    }

    template void amax<float>          (const handle_t & handle, int n, const float           *x, int incx, int *result);
    template void amax<double>         (const handle_t & handle, int n, const double          *x, int incx, int *result);
    template void amax<cuComplex>      (const handle_t & handle, int n, const cuComplex       *x, int incx, int *result);
    template void amax<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *x, int incx, int *result);

    template<typename T>
    void amin(const handle_t & handle, int n, const T *x, int incx, int *result) {
        throw_if_error(CuBLAS<T>::amin(handle.enable().id(), n, x, incx, result));
    }

    template void amin<float>          (const handle_t & handle, int n, const float           *x, int incx, int *result);
    template void amin<double>         (const handle_t & handle, int n, const double          *x, int incx, int *result);
    template void amin<cuComplex>      (const handle_t & handle, int n, const cuComplex       *x, int incx, int *result);
    template void amin<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *x, int incx, int *result);

    template<typename T>
    void asum(const handle_t & handle, int n, const T *x, int incx, AssociatedT<T> *result) {
        throw_if_error(CuBLAS<T>::asum(handle.enable().id(), n, x, incx, result));
    }

    template void asum<float>          (const handle_t & handle, int n, const float           *x, int incx, float  *result);
    template void asum<double>         (const handle_t & handle, int n, const double          *x, int incx, double *result);
    template void asum<cuComplex>      (const handle_t & handle, int n, const cuComplex       *x, int incx, float  *result);
    template void asum<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *x, int incx, double *result);

    template<typename T>
    void axpy(const handle_t & handle, int n, const T *alpha, const T *x, int incx, T *y, int incy) {
        throw_if_error(CuBLAS<T>::axpy(handle.enable().id(), n, alpha, x, incx, y, incy));
    }

    template void axpy<float>          (const handle_t & handle, int n, const float           *alpha, const float           *x, int incx, float           *y, int incy);
    template void axpy<double>         (const handle_t & handle, int n, const double          *alpha, const double          *x, int incx, double          *y, int incy);
    template void axpy<cuComplex>      (const handle_t & handle, int n, const cuComplex       *alpha, const cuComplex       *x, int incx, cuComplex       *y, int incy);
    template void axpy<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *alpha, const cuDoubleComplex *x, int incx, cuDoubleComplex *y, int incy);

    template<typename T>
    void scal(const handle_t & handle, int n, const T *alpha, T *x, int incx) {
        throw_if_error(CuBLAS<T>::scal(handle.enable().id(), n, alpha, x, incx));
    }

    template void scal<float>          (const handle_t & handle, int n, const float           *alpha, float           *x, int incx);
    template void scal<double>         (const handle_t & handle, int n, const double          *alpha, double          *x, int incx);
    template void scal<cuComplex>      (const handle_t & handle, int n, const cuComplex       *alpha, cuComplex       *x, int incx);
    template void scal<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *alpha, cuDoubleComplex *x, int incx);


    template<typename T>
    void dgmm(const handle_t & handle, SideMode side, int m, int n, const T *A, int lda, const T *x, int incx, T *C, int ldc) {
        throw_if_error(CuBLAS<T>::dgmm(handle.enable().id(), convert(side), m,n, A, lda, x, incx, C, ldc));
    }

    template void dgmm<float>          (const handle_t & handle, SideMode side, int m, int n, const float           *A, int lda, const float           *x, int incx, float           *C, int ldc);
    template void dgmm<double>         (const handle_t & handle, SideMode side, int m, int n, const double          *A, int lda, const double          *x, int incx, double          *C, int ldc);
    template void dgmm<cuComplex>      (const handle_t & handle, SideMode side, int m, int n, const cuComplex       *A, int lda, const cuComplex       *x, int incx, cuComplex       *C, int ldc);
    template void dgmm<cuDoubleComplex>(const handle_t & handle, SideMode side, int m, int n, const cuDoubleComplex *A, int lda, const cuDoubleComplex *x, int incx, cuDoubleComplex *C, int ldc);

    template<typename T>
    void gemv(const handle_t & handle, Operation trans, int m, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy) {
        throw_if_error(CuBLAS<T>::gemv(handle.enable().id(), convert(trans), m, n, alpha, A, lda, x, incx, beta, y, incy));
    }

    template void gemv<float>          (const handle_t & handle, Operation trans, int m, int n, const float           *alpha, const float           *A, int lda, const float           *x, int incx, const float           *beta, float           *y, int incy);
    template void gemv<double>         (const handle_t & handle, Operation trans, int m, int n, const double          *alpha, const double          *A, int lda, const double          *x, int incx, const double          *beta, double          *y, int incy);
    template void gemv<cuComplex>      (const handle_t & handle, Operation trans, int m, int n, const cuComplex       *alpha, const cuComplex       *A, int lda, const cuComplex       *x, int incx, const cuComplex       *beta, cuComplex       *y, int incy);
    template void gemv<cuDoubleComplex>(const handle_t & handle, Operation trans, int m, int n, const cuDoubleComplex *alpha, const cuDoubleComplex *A, int lda, const cuDoubleComplex *x, int incx, const cuDoubleComplex *beta, cuDoubleComplex *y, int incy);

    template<typename T>
    void gemm(const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc){
        throw_if_error(CuBLAS<T>::gemm(handle.enable().id(), convert(transa), convert(transb), m, n, k, alpha, A, lda, B, ldb, beta, C, ldc));
    }

    template void gemm<float>          (const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const float           *alpha, const float           *A, int lda, const float           *B, int ldb,const float           *beta, float           *C, int ldc);
    template void gemm<double>         (const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const double          *alpha, const double          *A, int lda, const double          *B, int ldb,const double          *beta, double          *C, int ldc);
    template void gemm<cuComplex>      (const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const cuComplex       *alpha, const cuComplex       *A, int lda, const cuComplex       *B, int ldb,const cuComplex       *beta, cuComplex       *C, int ldc);
    template void gemm<cuDoubleComplex>(const handle_t & handle, Operation transa, Operation transb, int m, int n, int k, const cuDoubleComplex *alpha, const cuDoubleComplex *A, int lda, const cuDoubleComplex *B, int ldb,const cuDoubleComplex *beta, cuDoubleComplex *C, int ldc);

    template<typename T>
    void syrk(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha, const T *A, int lda, const T *beta, T *C, int ldc){
        throw_if_error(CuBLAS<T>::syrk(handle.enable().id(), convert(uplo), convert(trans), n, k, alpha, A, lda, beta, C, ldc));
    }

    template void syrk<float>          (const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const float           *alpha, const float           *A, int lda, const float           *beta, float           *C, int ldc);
    template void syrk<double>         (const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const double          *alpha, const double          *A, int lda, const double          *beta, double          *C, int ldc);
    template void syrk<cuComplex>      (const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const cuComplex       *alpha, const cuComplex       *A, int lda, const cuComplex       *beta, cuComplex       *C, int ldc);
    template void syrk<cuDoubleComplex>(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const cuDoubleComplex *alpha, const cuDoubleComplex *A, int lda, const cuDoubleComplex *beta, cuDoubleComplex *C, int ldc);

    template<typename T>
    void trsm(const handle_t & handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const T *alpha, const T *A, int lda, T *B, int ldb){
        throw_if_error(CuBLAS<T>::trsm(handle.enable().id(), convert(side), convert(uplo), convert(trans), convert(diag), m, n, alpha, A, lda, B, ldb));
    }

    template void trsm<float>          (const handle_t & handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const float           *alpha, const float           *A, int lda, float           *B, int ldb);
    template void trsm<double>         (const handle_t & handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const double          *alpha, const double          *A, int lda, double          *B, int ldb);
    template void trsm<cuComplex>      (const handle_t & handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const cuComplex       *alpha, const cuComplex       *A, int lda, cuComplex       *B, int ldb);
    template void trsm<cuDoubleComplex>(const handle_t & handle, SideMode side, FillMode uplo, Operation trans, DiagonalType diag, int m, int n, const cuDoubleComplex *alpha, const cuDoubleComplex *A, int lda, cuDoubleComplex *B, int ldb);

    template<typename T>
    void getrf_batched(const handle_t & handle, int n, T *A_array[], int lda, int *pivot_array, int *info_array, int batch_size) {
        throw_if_error(CuBLAS<T>::getrf_batched(handle.enable().id(), n, A_array, lda, pivot_array,info_array, batch_size));
    }

    template void getrf_batched<float>          (const handle_t & handle, int n, float           *a_array[], int lda, int *pivot_array, int *info_array, int batch_size);
    template void getrf_batched<double>         (const handle_t & handle, int n, double          *a_array[], int lda, int *pivot_array, int *info_array, int batch_size);
    template void getrf_batched<cuComplex>      (const handle_t & handle, int n, cuComplex       *a_array[], int lda, int *pivot_array, int *info_array, int batch_size);
    template void getrf_batched<cuDoubleComplex>(const handle_t & handle, int n, cuDoubleComplex *a_array[], int lda, int *pivot_array, int *info_array, int batch_size);

    template<typename T>
    void getri_batched(const handle_t & handle, int n, T *A_array[], int lda, int *pivot_array, T *c_array[], int ldc, int *info_array, int batch_size){
         throw_if_error(CuBLAS<T>::getri_batched(handle.enable().id(), n, A_array, lda, pivot_array, c_array, ldc, info_array, batch_size));
    }

    template void getri_batched<float>          (const handle_t & handle, int n, float           *a_array[], int lda, int *pivot_array, float           *c_array[], int ldc, int *info_array, int batch_size);
    template void getri_batched<double>         (const handle_t & handle, int n, double          *a_array[], int lda, int *pivot_array, double          *c_array[], int ldc, int *info_array, int batch_size);
    template void getri_batched<cuComplex>      (const handle_t & handle, int n, cuComplex       *a_array[], int lda, int *pivot_array, cuComplex       *c_array[], int ldc, int *info_array, int batch_size);
    template void getri_batched<cuDoubleComplex>(const handle_t & handle, int n, cuDoubleComplex *a_array[], int lda, int *pivot_array, cuDoubleComplex *c_array[], int ldc, int *info_array, int batch_size);

// NOLINTEND(readability-identifier-naming)

} // namespace emu::cublas
