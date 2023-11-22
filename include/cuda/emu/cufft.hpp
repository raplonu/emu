#pragma once

#include <emu/cufft/handle.hpp>
#include <emu/cublas/type.hpp>

#include <cufft.h>

namespace emu
{

namespace cufft
{

    template<typename In, typename Out>
    cufftType_t type();

    template<typename In, typename Out>
    void exec(const handle_t & handle, const In * in, Out * out, Direction dir);

} // namespace cufft

} // namespace emu
