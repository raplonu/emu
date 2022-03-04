#ifndef EMU_CUFFT_H
#define EMU_CUFFT_H

#include <emu/cufft/handle.h>
#include <emu/cublas/type.h>

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

#endif //EMU_CUFFT_H