#ifndef EMU_MACRO_H
#define EMU_MACRO_H

#include <emu/config.h>

#include <boost/preprocessor.hpp>

/// Specify that a function may throw or not. Do nothing if noexcept is not available.
#define EMU_NOEXCEPT(cond) BOOST_NOEXCEPT_IF(cond)

/// Specify that a function may throw if the given expression may throw.
/// Do nothing if noexcept is not available.
#define EMU_NOEXCEPT_EXPR(expr) BOOST_NOEXCEPT_IF(BOOST_NOEXCEPT_EXPR(expr))

#if EMU_CUDA

#define EMU_HOST __host__
#define EMU_DEVICE __device__
#define EMU_HODE __host__ __device__

#else

#define EMU_HOST
#define EMU_DEVICE
#define EMU_HODE

#endif

#endif //EMU_MACRO_H
