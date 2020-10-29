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
        constexpr static auto getrf_bufferSize = cusolverDnSgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnSgetrf;
        constexpr static auto getrs            = cusolverDnSgetrs;

    };

    template<>
    struct CuSolver<double> {
        constexpr static auto potrf_bufferSize = cusolverDnDpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnDpotrf;
        constexpr static auto getrf_bufferSize = cusolverDnDgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnDgetrf;
        constexpr static auto getrs            = cusolverDnDgetrs;

    };

    template<>
    struct CuSolver<cuComplex> {
        constexpr static auto potrf_bufferSize = cusolverDnCpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnCpotrf;
        constexpr static auto getrf_bufferSize = cusolverDnCgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnCgetrf;
        constexpr static auto getrs            = cusolverDnCgetrs;

    };

    template<>
    struct CuSolver<cuDoubleComplex> {
        constexpr static auto potrf_bufferSize = cusolverDnZpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnZpotrf;
        constexpr static auto getrf_bufferSize = cusolverDnZgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnZgetrf;
        constexpr static auto getrs            = cusolverDnZgetrs;

    };

    template<typename T>
    void potrf_bufferSize(const handle_t & handle, FillMode uplo, int N, T *A, int lda, int *lwork) {
        throw_if_error(CuSolver<T>::potrf_bufferSize(handle.id(), convert(uplo), N, A, lda, lwork));
    }

    template void potrf_bufferSize<float>          (const handle_t & handle, FillMode uplo, int N, float          *A, int lda, int *lwork);
    template void potrf_bufferSize<double>         (const handle_t & handle, FillMode uplo, int N, double         *A, int lda, int *lwork);
    template void potrf_bufferSize<cuComplex>      (const handle_t & handle, FillMode uplo, int N, cuComplex      *A, int lda, int *lwork);
    template void potrf_bufferSize<cuDoubleComplex>(const handle_t & handle, FillMode uplo, int N, cuDoubleComplex*A, int lda, int *lwork);

    template<typename T>
    void potrf(const handle_t & handle, FillMode uplo, int N, T *A, int lda, T *workspace, int lwork, int *devInfo) {
        throw_if_error(CuSolver<T>::potrf(handle.id(), convert(uplo), N, A, lda, workspace, lwork, devInfo));
    }

    template void potrf<float>          (const handle_t & handle, FillMode uplo, int N, float           *A, int lda, float           *workspace, int lwork, int *devInfo);
    template void potrf<double>         (const handle_t & handle, FillMode uplo, int N, double          *A, int lda, double          *workspace, int lwork, int *devInfo);
    template void potrf<cuComplex>      (const handle_t & handle, FillMode uplo, int N, cuComplex       *A, int lda, cuComplex       *workspace, int lwork, int *devInfo);
    template void potrf<cuDoubleComplex>(const handle_t & handle, FillMode uplo, int N, cuDoubleComplex *A, int lda, cuDoubleComplex *workspace, int lwork, int *devInfo);

    template<typename T>
    void getrf_bufferSize(const handle_t & handle, int m, int n, T *A, int lda, int *Lwork ){
        throw_if_error(CuSolver<T>::getrf_bufferSize(handle.id(), m, n, A, lda, Lwork));
    }

    template void getrf_bufferSize<float>          (const handle_t & handle, int m, int n, float           *A, int lda, int *Lwork);
    template void getrf_bufferSize<double>         (const handle_t & handle, int m, int n, double          *A, int lda, int *Lwork);
    template void getrf_bufferSize<cuComplex>      (const handle_t & handle, int m, int n, cuComplex       *A, int lda, int *Lwork);
    template void getrf_bufferSize<cuDoubleComplex>(const handle_t & handle, int m, int n, cuDoubleComplex *A, int lda, int *Lwork);

    template<typename T>
    void getrf(const handle_t & handle, int m, int n, T *A, int lda, T *Workspace, int *devIpiv, int *devInfo){
        throw_if_error(CuSolver<T>::getrf(handle.id(), m, n, A, lda, Workspace, devIpiv, devInfo));
    }

    template void  getrf<float>          (const handle_t & handle, int m, int n, float           *A, int lda, float           *Workspace, int *devIpiv, int *devInfo);
    template void  getrf<double>         (const handle_t & handle, int m, int n, double          *A, int lda, double          *Workspace, int *devIpiv, int *devInfo);
    template void  getrf<cuComplex>      (const handle_t & handle, int m, int n, cuComplex       *A, int lda, cuComplex       *Workspace, int *devIpiv, int *devInfo);
    template void  getrf<cuDoubleComplex>(const handle_t & handle, int m, int n, cuDoubleComplex *A, int lda, cuDoubleComplex *Workspace, int *devIpiv, int *devInfo);

    template<typename T>
    void getrs(const handle_t & handle, Operation uplo,int n, int nrhs, const T *A, int lda, const int *devIpiv, T *B, int ldb, int *devInfo){
        throw_if_error(CuSolver<T>::getrs(handle.id(),convert(uplo), n, nrhs, A, lda, devIpiv, B, ldb, devInfo));
    }

    template void getrs<float>          (const handle_t & handle, Operation uplo,int n, int nrhs, const float           *A, int lda, const int *devIpiv, float           *B, int ldb, int *devInfo);
    template void getrs<double>         (const handle_t & handle, Operation uplo,int n, int nrhs, const double          *A, int lda, const int *devIpiv, double          *B, int ldb, int *devInfo);
    template void getrs<cuComplex>      (const handle_t & handle, Operation uplo,int n, int nrhs, const cuComplex       *A, int lda, const int *devIpiv, cuComplex       *B, int ldb, int *devInfo);
    template void getrs<cuDoubleComplex>(const handle_t & handle, Operation uplo,int n, int nrhs, const cuDoubleComplex *A, int lda, const int *devIpiv, cuDoubleComplex *B, int ldb, int *devInfo);


} // namespace cusolver

} // namespace emu
