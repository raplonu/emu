#ifndef EMU_CONFIG_H
#define EMU_CONFIG_H

#ifdef __CUDACC__

#define EMU_CUDACC 1

#else

#define EMU_CUDACC 0

#endif

#endif //EMU_CONFIG_H