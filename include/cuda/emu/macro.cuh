#ifndef EMU_MACRO_CUH
#define EMU_MACRO_CUH

#include <emu/macro.h>

#include <emu/config.cuh>

#define TID (threadIdx.x)
#define TID0 (!threadIdx.x)

#define BID (blockIdx.x)
#define BID0 (!blockIdx.x)

#define ID (threadIdx.x + blockIdx.x * blockDim.x)
#define ID0 (!(threadIdx.x || blockIdx.x))

#define SIZE_B (blockDim.x)
#define SIZE_G (gridDim.x)

#define SIZE_TOT (blockDim.x * gridDim.x)

#endif //EMU_MACRO_CUH
