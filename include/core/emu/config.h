#ifndef EMU_CONFIG_H
#define EMU_CONFIG_H

#ifdef __CUDACC__

#define EMU_CUDA 1

#else

#define EMU_CUDA 0

#endif

#ifdef __CUDA_ARCH__
    #define D_CONTEXT
#else
    #define H_CONTEXT
#endif

#endif //EMU_CONFIG_H