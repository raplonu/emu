#pragma once

#include <emu/cusolver/handle.hpp>
#include <emu/cublas/type.hpp>

#include <cusolverDn.h>

namespace emu::cusolver
{
    using emu::cublas::Operation;
    using emu::cublas::SideMode;
    using emu::cublas::FillMode;
    using emu::cublas::DiagonalType;

    using emu::cublas::convert;

// follow cusolver naming convention for readability. lint naming checks are disabled for this section
// NOLINTBEGIN(readability-identifier-naming)

    template<typename T>
    void potrf_buffer_size(const handle_t & handle, FillMode uplo, int N, T *A, int lda, int *lwork);

    template<typename T>
    void potrf(const handle_t & handle, FillMode uplo, int N, T *A, int lda, T *workspace, int lwork, int *dev_info);

    template<typename T>
    void potrs(const handle_t & handle, FillMode uplo, int N, int nrhs, T *A, int lda, T *B, int ldb, int *dev_info);

    template<typename T>
    void getrf_buffer_size(const handle_t & handle, int m, int n, T *A, int lda, int *Lwork );

    template<typename T>
    void getrf(const handle_t & handle, int m, int n, T *A, int lda, T *workspace, int *dev_ipiv, int *dev_info);

    template<typename T>
    void getrs(const handle_t & handle, Operation uplo,int n, int nrhs, const T *A, int lda, const int *dev_ipiv, T *B, int ldb, int *dev_info);

    template<typename T>
    int gesvd_buffer_size(const handle_t & handle, int m, int n);

    template<typename T, typename F>
    void gesvd(const handle_t & handle, char jobu, char jobvt, int m, int n, T *A, int lda, F *S, T *U, int ldu, T *VT, int ldvt, T *work, int lwork, F *rwork, int *dev_info);

// NOLINTEND(readability-identifier-naming)

} // namespace emu::cusolver
