#ifndef EMU_CONFIG_CUH
#define EMU_CONFIG_CUH

#include <emu/config.h>

#ifdef __CUDA_ARCH__
    #define DEVICE_CONTEXT
#else
    #define HOST_CONTEXT
#endif

#endif //EMU_CONFIG_CUH