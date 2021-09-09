#ifndef EMU_CONFIG_H
#define EMU_CONFIG_H

#include <boost/config.hpp>

// False if using nvcc.
#define EMU_GCC BOOST_GCC

#ifdef __CUDACC__

#define EMU_CUDACC 1

#else

#define EMU_CUDACC 0

#endif

#define EMU_NO_CXX17 BOOST_NO_CXX17

#endif //EMU_CONFIG_H