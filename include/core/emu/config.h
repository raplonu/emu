#ifndef EMU_CONFIG_H
#define EMU_CONFIG_H

#ifdef __CUDACC__

#define EMU_CUDA 1

#else

#define EMU_CUDA 0

#endif

template<auto V>
auto f(bool t)
{
    if constexpr (V)
        return t;
    else
        return 1;

}

inline auto ff(bool t)
{
    return f<true>(t);
}

#endif //EMU_CONFIG_H