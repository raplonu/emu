#ifndef EMU_MACRO_H
#define EMU_MACRO_H

#include <emu/config.h>

/// Specify that a function may throw or not. Do nothing if noexcept is not available.
#define EMU_NOEXCEPT(cond) noexcept(cond)

/// Specify that a function may throw if the given expression may throw.
/// Do nothing if noexcept is not available.
#define EMU_NOEXCEPT_EXPR(expr) noexcept(noexcept(expr))

#if EMU_CUDA

#define EMU_GLOB __global__
#define EMU_HOST __host__
#define EMU_DEVICE __device__
#define EMU_HODE __host__ __device__

#ifdef __CUDA_ARCH__
    #define DEVICE_CONTEXT 1
    #define HOST_CONTEXT   0
#else
    #define DEVICE_CONTEXT 0
    #define HOST_CONTEXT   1
#endif

#else

#define DEVICE_CONTEXT     0
#define HOST_CONTEXT       1

#define EMU_GLOB
#define EMU_HOST
#define EMU_DEVICE
#define EMU_HODE

#endif

#if defined __GNUC__ or defined __CUDACC__
#define EMU_COLD_PATH __attribute__((noinline,cold))

#else // compilator is neither gcc nor nvcc
#define EMU_COLD_PATH
#error

#endif

#define EMU_UNREACHABLE __builtin_unreachable();

#endif //EMU_MACRO_H
