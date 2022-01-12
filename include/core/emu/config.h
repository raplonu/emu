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

#define EMU_FROM_CXX14 __cplusplus >= 201402L
#define EMU_UNTIL_CXX14 __cplusplus < 201402L

#define EMU_FROM_CXX17 __cplusplus >= 201703L
#define EMU_UNTIL_CXX17 __cplusplus < 201703L

#define EMU_FROM_CXX20 __cplusplus >= 202002L
#define EMU_UNTIL_CXX20 __cplusplus < 202002L


#endif //EMU_CONFIG_H