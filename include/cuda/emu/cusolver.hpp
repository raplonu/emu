#pragma once

#include <emu/cusolver/handle.hpp>
#include <emu/cublas/type.hpp>

#include <cusolverDn.h>

namespace emu
{

namespace cusolver
{
    using emu::cublas::Operation;
    using emu::cublas::SideMode;
    using emu::cublas::FillMode;
    using emu::cublas::DiagonalType;

    using emu::cublas::convert;

    template<typename T>
    void potrf_bufferSize(const handle_t & handle, FillMode uplo, int N, T *A, int lda, int *lwork);

    template<typename T>
    void potrf(const handle_t & handle, FillMode uplo, int N, T *A, int lda, T *workspace, int lwork, int *devInfo);

    template<typename T>
    void potrs(const handle_t & handle, FillMode uplo, int N, int nrhs, T *A, int lda, T *B, int ldb, int *devInfo);

    template<typename T>
    void getrf_bufferSize(const handle_t & handle, int m, int n, T *A, int lda, int *Lwork );

    template<typename T>
    void getrf(const handle_t & handle, int m, int n, T *A, int lda, T *Workspace, int *devIpiv, int *devInfo);

    template<typename T>
    void getrs(const handle_t & handle, Operation uplo,int n, int nrhs, const T *A, int lda, const int *devIpiv, T *B, int ldb, int *devInfo);

    template<typename T>
    int gesvd_bufferSize(const handle_t & handle, int m, int n);

    template<typename T, typename F>
    void gesvd(const handle_t & handle, char jobu, char jobvt, int m, int n, T *A, int lda, F *S, T *U, int ldu, T *VT, int ldvt, T *work, int lwork, F *rwork, int *devInfo);

} // namespace cusolver

} // namespace emu
