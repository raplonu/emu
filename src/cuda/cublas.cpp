#include <emu/cublas.h>

namespace emu
{

namespace cublas
{

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

    };

    template<>
    struct CuBLAS<double> {
        constexpr static auto amax = cublasIdamax;
        constexpr static auto amin = cublasIdamin;
        constexpr static auto asum = cublasDasum;
        constexpr static auto axpy = cublasDaxpy;

        constexpr static auto gemv = cublasDgemv;

    };

    template<>
    struct CuBLAS<cuComplex> {
        constexpr static auto amax = cublasIcamax;
        constexpr static auto amin = cublasIcamin;
        constexpr static auto asum = cublasScasum;
        constexpr static auto axpy = cublasCaxpy;

        constexpr static auto gemv = cublasCgemv;

    };

    template<>
    struct CuBLAS<cuDoubleComplex> {
        constexpr static auto amax = cublasIzamax;
        constexpr static auto amin = cublasIzamin;
        constexpr static auto asum = cublasDzasum;
        constexpr static auto axpy = cublasZaxpy;

        constexpr static auto gemv = cublasZgemv;

    };


    template<typename T>
    void amax(handle_t handle, int n, const T *x, int incx, int *result) {
        throw_if_error(CuBLAS<T>::amax(handle.id(), n, x, incx, result));
    }

    template void amax<float>          (handle_t handle, int n, const float           *x, int incx, int *result);
    template void amax<double>         (handle_t handle, int n, const double          *x, int incx, int *result);
    template void amax<cuComplex>      (handle_t handle, int n, const cuComplex       *x, int incx, int *result);
    template void amax<cuDoubleComplex>(handle_t handle, int n, const cuDoubleComplex *x, int incx, int *result);

    template<typename T>
    void amin(handle_t handle, int n, const T *x, int incx, int *result) {
        throw_if_error(CuBLAS<T>::amin(handle.id(), n, x, incx, result));
    }

    template void amin<float>          (handle_t handle, int n, const float           *x, int incx, int *result);
    template void amin<double>         (handle_t handle, int n, const double          *x, int incx, int *result);
    template void amin<cuComplex>      (handle_t handle, int n, const cuComplex       *x, int incx, int *result);
    template void amin<cuDoubleComplex>(handle_t handle, int n, const cuDoubleComplex *x, int incx, int *result);

    template<typename T>
    void asum(handle_t handle, int n, const T *x, int incx, AssociatedT<T> *result) {
        throw_if_error(CuBLAS<T>::asum(handle.id(), n, x, incx, result));
    }

    template void asum<float>          (handle_t handle, int n, const float           *x, int incx, float  *result);
    template void asum<double>         (handle_t handle, int n, const double          *x, int incx, double *result);
    template void asum<cuComplex>      (handle_t handle, int n, const cuComplex       *x, int incx, float  *result);
    template void asum<cuDoubleComplex>(handle_t handle, int n, const cuDoubleComplex *x, int incx, double *result);

    template<typename T>
    void axpy(handle_t handle, int n, const T *alpha, const T *x, int incx, T *y, int incy) {
        throw_if_error(CuBLAS<T>::axpy(handle.id(), n, alpha, x, incx, y, incy));
    }

    template void axpy<float>          (handle_t handle, int n, const float           *alpha, const float           *x, int incx, float           *y, int incy);
    template void axpy<double>         (handle_t handle, int n, const double          *alpha, const double          *x, int incx, double          *y, int incy);
    template void axpy<cuComplex>      (handle_t handle, int n, const cuComplex       *alpha, const cuComplex       *x, int incx, cuComplex       *y, int incy);
    template void axpy<cuDoubleComplex>(handle_t handle, int n, const cuDoubleComplex *alpha, const cuDoubleComplex *x, int incx, cuDoubleComplex *y, int incy);



    template<typename T>
    void gemv(handle_t handle, Operation trans, int m, int n, const T *alpha, const T *A, int lda, const T *x, int incx, const T *beta, T *y, int incy) {
        throw_if_error(CuBLAS<T>::gemv(handle.id(), convert(trans), m, n, alpha, A, lda, x, incx, beta, y, incy));
    }

    template void gemv<float>          (handle_t handle, Operation trans, int m, int n, const float           *alpha, const float           *A, int lda, const float           *x, int incx, const float           *beta, float           *y, int incy);
    template void gemv<double>         (handle_t handle, Operation trans, int m, int n, const double          *alpha, const double          *A, int lda, const double          *x, int incx, const double          *beta, double          *y, int incy);
    template void gemv<cuComplex>      (handle_t handle, Operation trans, int m, int n, const cuComplex       *alpha, const cuComplex       *A, int lda, const cuComplex       *x, int incx, const cuComplex       *beta, cuComplex       *y, int incy);
    template void gemv<cuDoubleComplex>(handle_t handle, Operation trans, int m, int n, const cuDoubleComplex *alpha, const cuDoubleComplex *A, int lda, const cuDoubleComplex *x, int incx, const cuDoubleComplex *beta, cuDoubleComplex *y, int incy);



} // namespace cublas

} // namespace emu
