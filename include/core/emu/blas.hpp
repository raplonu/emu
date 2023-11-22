#pragma once

namespace emu
{

namespace blas
{

    template<typename ExecutionPolicy, typename T>
    void axpy(int n, T alpha, T *x, int incx, T *y, int incy);

} // namespace blas

} // namespace emu
