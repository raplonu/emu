#include <emu/cusolver.h>
#include <emu/cublas/type.h>
#include <string>


namespace emu
{

namespace cusolver
{
    // gemm syrk trsm potrf (cusolver)

    template<typename T> struct CuSolver;

    template<>
    struct CuSolver<__half> {

    };

    template<>
    struct CuSolver<float> {
        constexpr static auto potrf_bufferSize = cusolverDnSpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnSpotrf;

    };

    template<>
    struct CuSolver<double> {
        constexpr static auto potrf_bufferSize = cusolverDnDpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnDpotrf;

    };

    template<>
    struct CuSolver<cuComplex> {
        constexpr static auto potrf_bufferSize = cusolverDnCpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnCpotrf;

    };

    template<>
    struct CuSolver<cuDoubleComplex> {
        constexpr static auto potrf_bufferSize = cusolverDnZpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnZpotrf;

    };

    template<typename T>
    void potrf_bufferSize(const handle_t & handle, cublas::FillMode uplo, int N, T *A, int lda, int *lwork) {
        throw_if_error(CuSolver<T>::potrf_bufferSize(handle.id(), convert(uplo), N, A, lda, lwork));
    }

    template void potrf_bufferSize<float>          (const handle_t & handle, cublas::FillMode uplo, int N, float          *A, int lda, int *lwork);
    template void potrf_bufferSize<double>         (const handle_t & handle, cublas::FillMode uplo, int N, double         *A, int lda, int *lwork);
    template void potrf_bufferSize<cuComplex>      (const handle_t & handle, cublas::FillMode uplo, int N, cuComplex      *A, int lda, int *lwork);
    template void potrf_bufferSize<cuDoubleComplex>(const handle_t & handle, cublas::FillMode uplo, int N, cuDoubleComplex*A, int lda, int *lwork);

    template<typename T>
    void potrf(const handle_t & handle, cublas::FillMode uplo, int N, T *A, int lda, T *workspace, int lwork, int *devInfo) {
        throw_if_error(CuSolver<T>::potrf(handle.id(), convert(uplo), N, A, lda, workspace, lwork, devInfo));
    }

    template void potrf<float>          (const handle_t & handle, cublas::FillMode uplo, int N, float           *A, int lda, float           *workspace, int lwork, int *devInfo);
    template void potrf<double>         (const handle_t & handle, cublas::FillMode uplo, int N, double          *A, int lda, double          *workspace, int lwork, int *devInfo);
    template void potrf<cuComplex>      (const handle_t & handle, cublas::FillMode uplo, int N, cuComplex       *A, int lda, cuComplex       *workspace, int lwork, int *devInfo);
    template void potrf<cuDoubleComplex>(const handle_t & handle, cublas::FillMode uplo, int N, cuDoubleComplex *A, int lda, cuDoubleComplex *workspace, int lwork, int *devInfo);



} // namespace cusolver

} // namespace emu
