#ifndef EMU_CUSOLVER_H
#define EMU_CUSOLVER_H

#include <emu/cusolver/handle.h>
#include <emu/cublas/type.h>

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
    void potrf_bufferSize(const handle_t & handle, cublas::FillMode uplo, int N, T *A, int lda, int *lwork);

    template<typename T>
    void potrf(const handle_t & handle, FillMode uplo, int N, T *A, int lda, T *workspace, int lwork, int *devInfo);


} // namespace cusolver

} // namespace emu

#endif // EMU_CUSOLVER_H
