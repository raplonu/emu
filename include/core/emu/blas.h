#ifndef EMU_BLAS_H
#define EMU_BLAS_H

namespace emu
{

namespace blas
{

    template<typename ExecutionPolicy, typename T>
    void axpy(int n, T alpha, T *x, int incx, T *y, int incy);

} // namespace bla

} // namespace emu

#endif //EMU_BLAS_H