#pragma once

#include <emu/macro.hpp>

#include <emu/config.cuh>

#define EMU_GLOB   __global__
#define EMU_DEVICE __device__

#define EMU_TID (threadIdx.x)
#define EMU_TID0 (!threadIdx.x)

#define EMU_BID (blockIdx.x)
#define EMU_BID0 (!blockIdx.x)

#define EMU_ID (threadIdx.x + blockIdx.x * blockDim.x)
#define EMU_ID0 (!(threadIdx.x || blockIdx.x))

#define EMU_SIZE_B (blockDim.x)
#define EMU_SIZE_G (gridDim.x)

#define EMU_SIZE_TOT (blockDim.x * gridDim.x)
