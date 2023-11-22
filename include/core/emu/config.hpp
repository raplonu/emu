#pragma once

#include <boost/config.hpp>

// False if using nvcc.
#define EMU_GCC BOOST_GCC

#ifdef __CUDACC__

#define EMU_CUDACC 1

#else

#define EMU_CUDACC 0

#endif
