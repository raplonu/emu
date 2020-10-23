#include <emu/cublas.h>

namespace emu
{

namespace cublas
{

    // gemm syrk trsm potrf (cusolver)

    template<typename T> struct CuBLAS;

    template<>
    struct CuBLAS<__half> {

    };

    template<>
    struct CuBLAS<float> {
        constexpr static auto amax = cublasIsamax;
        constexpr static auto amin = cublasIsamin;
        constexpr static auto asum = cublasSasum;
        constexpr static auto axpy = cublasSaxpy;

        constexpr static auto gemv = cublasSgemv;
        constexpr static auto gemm = cublasSgemm;
        constexpr static auto syrk = cublasSsyrk;
        constexpr static auto trsm = cublasStrsm;
        constexpr static auto getrf = cublasSgetrfBatched;
        constexpr static auto getri = cublasSgetriBatched;

    };

    template<>
    struct CuBLAS<double> {
        constexpr static auto amax = cublasIdamax;
        constexpr static auto amin = cublasIdamin;
        constexpr static auto asum = cublasDasum;
        constexpr static auto axpy = cublasDaxpy;

        constexpr static auto gemv = cublasDgemv;
        constexpr static auto gemm = cublasDgemm;
        constexpr static auto syrk = cublasDsyrk;
        constexpr static auto trsm = cublasDtrsm;
        constexpr static auto getrf = cublasDgetrfBatched;
        constexpr static auto getri = cublasDgetriBatched;

    };

    template<>
    struct CuBLAS<cuComplex> {
        constexpr static auto amax = cublasIcamax;
        constexpr static auto amin = cublasIcamin;
        constexpr static auto asum = cublasScasum;
        constexpr static auto axpy = cublasCaxpy;

        constexpr static auto gemv = cublasCgemv;
        constexpr static auto gemm = cublasCgemm;
        constexpr static auto syrk = cublasCsyrk;
        constexpr static auto trsm = cublasCtrsm;
        constexpr static auto getrf = cublasCgetrfBatched;
        constexpr static auto getri = cublasCgetriBatched;

    };

    template<>
    struct CuBLAS<cuDoubleComplex> {
        constexpr static auto amax = cublasIzamax;
        constexpr static auto amin = cublasIzamin;
        constexpr static auto asum = cublasDzasum;
        constexpr static auto axpy = cublasZaxpy;

        constexpr static auto gemv = cublasZgemv;
        constexpr static auto gemm = cublasZgemm;
        constexpr static auto syrk = cublasZsyrk;
        constexpr static auto trsm = cublasZtrsm;
        constexpr static auto getrf = cublasZgetrfBatched;
        constexpr static auto getri = cublasZgetriBatched;

    };


    template<typename T>
    void amax(const handle_t & handle, int n, const T *x, int incx, int *result) {
        throw_if_error(CuBLAS<T>::amax(handle.id(), n, x, incx, result));
    }

    template void amax<float>          (const handle_t & handle, int n, const float           *x, int incx, int *result);
    template void amax<double>         (const handle_t & handle, int n, const double          *x, int incx, int *result);
    template void amax<cuComplex>      (const handle_t & handle, int n, const cuComplex       *x, int incx, int *result);
    template void amax<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *x, int incx, int *result);

    template<typename T>
    void amin(const handle_t & handle, int n, const T *x, int incx, int *result) {
        throw_if_error(CuBLAS<T>::amin(handle.id(), n, x, incx, result));
    }

    template void amin<float>          (const handle_t & handle, int n, const float           *x, int incx, int *result);
    template void amin<double>         (const handle_t & handle, int n, const double          *x, int incx, int *result);
    template void amin<cuComplex>      (const handle_t & handle, int n, const cuComplex       *x, int incx, int *result);
    template void amin<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *x, int incx, int *result);

    template<typename T>
    void asum(const handle_t & handle, int n, const T *x, int incx, AssociatedT<T> *result) {
        throw_if_error(CuBLAS<T>::asum(handle.id(), n, x, incx, result));
    }

    template void asum<float>          (const handle_t & handle, int n, const float           *x, int incx, float  *result);
    template void asum<double>         (const handle_t & handle, int n, const double          *x, int incx, double *result);
    template void asum<cuComplex>      (const handle_t & handle, int n, const cuComplex       *x, int incx, float  *result);
    template void asum<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *x, int incx, double *result);

    template<typename T>
    void axpy(const handle_t & handle, int n, const T *alpha, const T *x, int incx, T *y, int incy) {
        throw_if_error(CuBLAS<T>::axpy(handle.id(), n, alpha, x, incx, y, incy));
    }

    template void axpy<float>          (const handle_t & handle, int n, const float           *alpha, const float           *x, int incx, float           *y, int incy);
    template void axpy<double>         (const handle_t & handle, int n, const double          *alpha, const double          *x, int incx, double          *y, int incy);
    template void axpy<cuComplex>      (const handle_t & handle, int n, const cuComplex       *alpha, const cuComplex       *x, int incx, cuComplex       *y, int incy);
    template void axpy<cuDoubleComplex>(const handle_t & handle, int n, const cuDoubleComplex *alpha, const cuDoubleComplex *x, int incx, cuDoubleComplex *y, int incy);



    template<typename T>
    void gemv(const handle_t & handle, Operation trans, int m, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy) {
        throw_if_error(CuBLAS<T>::gemv(handle.id(), convert(trans), m, n, alpha, A, lda, x, incx, beta, y, incy));
    }

    template void gemv<float>          (const handle_t & handle, Operation trans, int m, int n, const float           *alpha, const float           *A, int lda, const float           *x, int incx, const float           *beta, float           *y, int incy);
    template void gemv<double>         (const handle_t & handle, Operation trans, int m, int n, const double          *alpha, const double          *A, int lda, const double          *x, int incx, const double          *beta, double          *y, int incy);
    template void gemv<cuComplex>      (const handle_t & handle, Operation trans, int m, int n, const cuComplex       *alpha, const cuComplex       *A, int lda, const cuComplex       *x, int incx, const cuComplex       *beta, cuComplex       *y, int incy);
    template void gemv<cuDoubleComplex>(const handle_t & handle, Operation trans, int m, int n, const cuDoubleComplex *alpha, const cuDoubleComplex *A, int lda, const cuDoubleComplex *x, int incx, const cuDoubleComplex *beta, cuDoubleComplex *y, int incy);



    template<typename T>
    void gemm(const handle_t & handle, Operation transa, Operation transb, int m, int n, int k,
        const T *alpha, const T *A, int lda, const T *B, int ldb, const T *beta, T *C, int ldc){
        throw_if_error(CuBLAS<T>::gemm(handle.id(), convert(transa), convert(transb), m, n, k,
        alpha, A, lda, B, ldb, beta, C, ldc));
    }
    template void gemm<float>(const handle_t & handle, Operation transa, Operation transb, int m,
        int n, int k, const float *alpha, const float *A, int lda, const float *B, int ldb,
        const float *beta, float *C, int ldc);
    template void gemm<double>(const handle_t & handle, Operation transa, Operation transb, int m,
        int n, int k, const double *alpha, const double *A, int lda, const double *B, int ldb,
        const double *beta, double *C, int ldc);



    template<typename T>
    void syrk(const handle_t & handle, FillMode uplo, Operation trans, int n, int k, const T *alpha,
        const T *A, int lda, const T *beta, T *C, int ldc){
        throw_if_error(CuBLAS<T>::syrk(handle.id(), convert(uplo), convert(trans), n, k,
        alpha, A, lda, beta, C, ldc));
    }
    template void syrk<float>(const handle_t & handle, FillMode uplo, Operation trans, int n,
        int k, const float *alpha, const float *A, int lda, const float *beta, float *C, int ldc);
    template void syrk<double>(const handle_t & handle, FillMode uplo, Operation trans, int n,
        int k, const double *alpha, const double *A, int lda, const double *beta, double *C,
        int ldc);



    template<typename T>
    void trsm(const handle_t & handle, SideMode side, FillMode uplo, Operation trans,
        DiagonalType diag, int m, int n, const T *alpha, const T *A, int lda, T *B, int ldb){
        throw_if_error(CuBLAS<T>::trsm(handle.id(), convert(side), convert(uplo), convert(trans),
        convert(diag), m, n, alpha, A, lda, B, ldb));
    }
    template void trsm<float>(const handle_t & handle, SideMode side, FillMode uplo,
        Operation trans, DiagonalType diag, int m, int n, const float *alpha, const float *A,
        int lda, float *B, int ldb);
    template void trsm<double>(const handle_t & handle, SideMode side, FillMode uplo,
        Operation trans, DiagonalType diag, int m, int n, const double *alpha, const double *A,
        int lda, double *B, int ldb);


    template<typename T>
    void getrf_batched(const handle_t & handle, int n, T *Aarray[], int lda, int *PivotArray, int *infoArray, int batchSize){
        throw_if_error(CuBLAS<T>::getrf(handle.id(), n, Aarray, lda, PivotArray,infoArray, batchSize));
    }
    template void getrf_batched<float>(const handle_t & handle, int n, float *Aarray[], int lda, int *PivotArray, int *infoArray, int batchSize);
    template void getrf_batched<double>(const handle_t & handle, int n, double *Aarray[], int lda, int *PivotArray, int *infoArray, int batchSize);

    template<typename T>
    void getrf(const handle_t & handle, int n, T *A, int lda, int *Pivot, int *info){
        throw_if_error(CuBLAS<T>::getrf(handle.id(), n, &A, lda, Pivot, info, 1));
    }
    template void getrf<float>(const handle_t & handle, int n, float *A, int lda, int *Pivot, int *info);
    template void getrf<double>(const handle_t & handle, int n, double *A, int lda, int *Pivot, int *info);

    template<typename T>
    void getri_batched(const handle_t & handle, int n, T *Aarray[], int lda, int *PivotArray, T *Carray[], int ldc, int *infoArray, int batchSize){
         throw_if_error(CuBLAS<T>::getri(handle.id(), n, Aarray, lda, PivotArray, Carray, ldc, infoArray, batchSize));
    }
    template void getri_batched<float>(const handle_t & handle, int n, float *Aarray[], int lda, int *PivotArray, float *Carray[], int ldc, int *infoArray, int batchSize);
    template void getri_batched<double>(const handle_t & handle, int n, double *Aarray[], int lda, int *PivotArray, double *Carray[], int ldc, int *infoArray, int batchSize);

    template<typename T>
    void getri(const handle_t & handle, int n, T *A, int lda, int *Pivot, T *C, int ldc, int *info){
         throw_if_error(CuBLAS<T>::getri(handle.id(), n, &A, lda, Pivot, &C, ldc, info, 1));
    }
    template void getri<float>(const handle_t & handle, int n, float *A, int lda, int *Pivot, float *C, int ldc, int *info );
    template void getri<double>(const handle_t & handle, int n, double *A, int lda, int *Pivot, double *C, int ldc, int *info);

} // namespace cublas

} // namespace emu
