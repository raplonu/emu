#include <emu/cusolver.hpp>
#include <emu/cublas/type.hpp>

#include <fmt/core.h>

namespace emu::cusolver
{
    // gemm syrk trsm potrf (cusolver)

    template<typename T> struct CuSolver;

    template<>
    struct CuSolver<__half> {

    };

    template<>
    struct CuSolver<float> {
        constexpr static auto potrf_buffer_size = cusolverDnSpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnSpotrf;
        constexpr static auto potrs            = cusolverDnSpotrs;
        constexpr static auto getrf_buffer_size = cusolverDnSgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnSgetrf;
        constexpr static auto getrs            = cusolverDnSgetrs;
        constexpr static auto gesvd_buffer_size = cusolverDnSgesvd_bufferSize;
        constexpr static auto gesvd            = cusolverDnSgesvd;

    };

    template<>
    struct CuSolver<double> {
        constexpr static auto potrf_buffer_size = cusolverDnDpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnDpotrf;
        constexpr static auto potrs            = cusolverDnDpotrs;
        constexpr static auto getrf_buffer_size = cusolverDnDgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnDgetrf;
        constexpr static auto getrs            = cusolverDnDgetrs;
        constexpr static auto gesvd_buffer_size = cusolverDnDgesvd_bufferSize;
        constexpr static auto gesvd            = cusolverDnDgesvd;

    };

    template<>
    struct CuSolver<cuComplex> {
        constexpr static auto potrf_buffer_size = cusolverDnCpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnCpotrf;
        constexpr static auto potrs            = cusolverDnCpotrs;
        constexpr static auto getrf_buffer_size = cusolverDnCgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnCgetrf;
        constexpr static auto getrs            = cusolverDnCgetrs;
        constexpr static auto gesvd_buffer_size = cusolverDnCgesvd_bufferSize;
        constexpr static auto gesvd            = cusolverDnCgesvd;

    };

    template<>
    struct CuSolver<cuDoubleComplex> {
        constexpr static auto potrf_buffer_size = cusolverDnZpotrf_bufferSize;
        constexpr static auto potrf            = cusolverDnZpotrf;
        constexpr static auto potrs            = cusolverDnZpotrs;
        constexpr static auto getrf_buffer_size = cusolverDnZgetrf_bufferSize;
        constexpr static auto getrf            = cusolverDnZgetrf;
        constexpr static auto getrs            = cusolverDnZgetrs;
        constexpr static auto gesvd_buffer_size = cusolverDnZgesvd_bufferSize;
        constexpr static auto gesvd            = cusolverDnZgesvd;

    };

// NOLINTBEGIN(readability-identifier-naming)

    template<typename T>
    void potrf_buffer_size(const handle_t & handle, FillMode uplo, int n, T *A, int lda, int *lwork) {
        throw_if_error(CuSolver<T>::potrf_buffer_size(handle.id(), convert(uplo), n, A, lda, lwork));
    }

    template void potrf_buffer_size<float>          (const handle_t & handle, FillMode uplo, int n, float          *A, int lda, int *lwork);
    template void potrf_buffer_size<double>         (const handle_t & handle, FillMode uplo, int n, double         *A, int lda, int *lwork);
    template void potrf_buffer_size<cuComplex>      (const handle_t & handle, FillMode uplo, int n, cuComplex      *A, int lda, int *lwork);
    template void potrf_buffer_size<cuDoubleComplex>(const handle_t & handle, FillMode uplo, int n, cuDoubleComplex*A, int lda, int *lwork);

    template<typename T>
    void potrf(const handle_t & handle, FillMode uplo, int n, T *A, int lda, T *workspace, int lwork, int *dev_info) {
        throw_if_error(CuSolver<T>::potrf(handle.id(), convert(uplo), n, A, lda, workspace, lwork, dev_info));
    }

    template void potrf<float>          (const handle_t & handle, FillMode uplo, int n, float           *A, int lda, float           *workspace, int lwork, int *dev_info);
    template void potrf<double>         (const handle_t & handle, FillMode uplo, int n, double          *A, int lda, double          *workspace, int lwork, int *dev_info);
    template void potrf<cuComplex>      (const handle_t & handle, FillMode uplo, int n, cuComplex       *A, int lda, cuComplex       *workspace, int lwork, int *dev_info);
    template void potrf<cuDoubleComplex>(const handle_t & handle, FillMode uplo, int n, cuDoubleComplex *A, int lda, cuDoubleComplex *workspace, int lwork, int *dev_info);

    template<typename T>
    void potrs(const handle_t & handle, FillMode uplo, int n, int nrhs, T *A, int lda, T *B, int ldb, int *dev_info) {
        throw_if_error(CuSolver<T>::potrs(handle.id(), convert(uplo), n, nrhs, A, lda, B, ldb, dev_info));
    }

    template void potrs<float>          (const handle_t & handle, FillMode uplo, int n, int nrhs, float           *A, int lda, float           *B, int ldb, int *dev_info);
    template void potrs<double>         (const handle_t & handle, FillMode uplo, int n, int nrhs, double          *A, int lda, double          *B, int ldb, int *dev_info);
    template void potrs<cuComplex>      (const handle_t & handle, FillMode uplo, int n, int nrhs, cuComplex       *A, int lda, cuComplex       *B, int ldb, int *dev_info);
    template void potrs<cuDoubleComplex>(const handle_t & handle, FillMode uplo, int n, int nrhs, cuDoubleComplex *A, int lda, cuDoubleComplex *B, int ldb, int *dev_info);

    template<typename T>
    void getrf_buffer_size(const handle_t & handle, int m, int n, T *A, int lda, int *lwork ){
        throw_if_error(CuSolver<T>::getrf_buffer_size(handle.id(), m, n, A, lda, lwork));
    }

    template void getrf_buffer_size<float>          (const handle_t & handle, int m, int n, float           *A, int lda, int *lwork);
    template void getrf_buffer_size<double>         (const handle_t & handle, int m, int n, double          *A, int lda, int *lwork);
    template void getrf_buffer_size<cuComplex>      (const handle_t & handle, int m, int n, cuComplex       *A, int lda, int *lwork);
    template void getrf_buffer_size<cuDoubleComplex>(const handle_t & handle, int m, int n, cuDoubleComplex *A, int lda, int *lwork);

    template<typename T>
    void getrf(const handle_t & handle, int m, int n, T *A, int lda, T *workspace, int *dev_ipiv, int *dev_info){
        throw_if_error(CuSolver<T>::getrf(handle.id(), m, n, A, lda, workspace, dev_ipiv, dev_info));
    }

    template void  getrf<float>          (const handle_t & handle, int m, int n, float           *A, int lda, float           *workspace, int *dev_ipiv, int *dev_info);
    template void  getrf<double>         (const handle_t & handle, int m, int n, double          *A, int lda, double          *workspace, int *dev_ipiv, int *dev_info);
    template void  getrf<cuComplex>      (const handle_t & handle, int m, int n, cuComplex       *A, int lda, cuComplex       *workspace, int *dev_ipiv, int *dev_info);
    template void  getrf<cuDoubleComplex>(const handle_t & handle, int m, int n, cuDoubleComplex *A, int lda, cuDoubleComplex *workspace, int *dev_ipiv, int *dev_info);

    template<typename T>
    void getrs(const handle_t & handle, Operation uplo,int n, int nrhs, const T *A, int lda, const int *dev_ipiv, T *B, int ldb, int *dev_info){
        throw_if_error(CuSolver<T>::getrs(handle.id(),convert(uplo), n, nrhs, A, lda, dev_ipiv, B, ldb, dev_info));
    }

    template void getrs<float>          (const handle_t & handle, Operation uplo, int n, int nrhs, const float           *A, int lda, const int *dev_ipiv, float           *B, int ldb, int *dev_info);
    template void getrs<double>         (const handle_t & handle, Operation uplo, int n, int nrhs, const double          *A, int lda, const int *dev_ipiv, double          *B, int ldb, int *dev_info);
    template void getrs<cuComplex>      (const handle_t & handle, Operation uplo, int n, int nrhs, const cuComplex       *A, int lda, const int *dev_ipiv, cuComplex       *B, int ldb, int *dev_info);
    template void getrs<cuDoubleComplex>(const handle_t & handle, Operation uplo, int n, int nrhs, const cuDoubleComplex *A, int lda, const int *dev_ipiv, cuDoubleComplex *B, int ldb, int *dev_info);


    template<typename T>
    int gesvd_buffer_size(const handle_t & handle, int m, int n){
        int lwork = 0;
        throw_if_error(CuSolver<T>::gesvd_buffer_size(handle.id(), m, n, &lwork));
        return lwork;
    }
    template int gesvd_buffer_size<float>          (const handle_t & handle, int m, int n);
    template int gesvd_buffer_size<double>         (const handle_t & handle, int m, int n);
    template int gesvd_buffer_size<cuComplex>      (const handle_t & handle, int m, int n);
    template int gesvd_buffer_size<cuDoubleComplex>(const handle_t & handle, int m, int n);

    template<typename T,typename F>
    void gesvd(const handle_t & handle, char jobu, char jobvt, int m, int n, T *A, int lda, F *S, T *U, int ldu, T *VT, int ldvt, T *work, int lwork, F *rwork, int *dev_info){
            throw_if_error(CuSolver<T>::gesvd(handle.id(),jobu, jobvt, m, n, A, lda, S, U, ldu, VT, ldvt, work, lwork, rwork, dev_info));
    }

    template void gesvd<float,float>           (const handle_t & handle, char jobu, char jobvt, int m, int n, float           *A, int lda, float  *S, float           *U, int ldu, float           *VT, int ldvt, float           *work, int lwork, float  *rwork, int *dev_info);
    template void gesvd<double,double>         (const handle_t & handle, char jobu, char jobvt, int m, int n, double          *A, int lda, double *S, double          *U, int ldu, double          *VT, int ldvt, double          *work, int lwork, double *rwork, int *dev_info);
    template void gesvd<cuComplex,float>       (const handle_t & handle, char jobu, char jobvt, int m, int n, cuComplex       *A, int lda, float  *S, cuComplex       *U, int ldu, cuComplex       *VT, int ldvt, cuComplex       *work, int lwork, float  *rwork, int *dev_info);
    template void gesvd<cuDoubleComplex,double>(const handle_t & handle, char jobu, char jobvt, int m, int n, cuDoubleComplex *A, int lda, double *S, cuDoubleComplex *U, int ldu, cuDoubleComplex *VT, int ldvt, cuDoubleComplex *work, int lwork, double *rwork, int *dev_info);

// NOLINTEND(readability-identifier-naming)

} // namespace emu::cusolver
