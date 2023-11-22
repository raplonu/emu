#pragma once

#include <emu/assert.hpp>

namespace emu
{

namespace cufft
{

    enum class Direction {
        forward,
        inverse,
    };

    enum class type_t {
        r2c = CUFFT_R2C,
        c2r = CUFFT_C2R,
        c2c = CUFFT_C2C,
        d2z = CUFFT_D2Z,
        z2d = CUFFT_Z2D,
        z2z = CUFFT_Z2Z
    };

    constexpr inline int convert(Direction dir) noexcept {
        switch (dir)
        {
        case Direction::forward:
            return CUFFT_FORWARD;
        case Direction::inverse:
            return CUFFT_INVERSE;
        }
        EMU_UNREACHABLE;
    }

    constexpr inline cufftType convert(type_t type) noexcept {
        switch (type)
        {
        case type_t::r2c:
            return CUFFT_R2C;
        case type_t::c2r:
            return CUFFT_C2R;
        case type_t::c2c:
            return CUFFT_C2C;
        case type_t::d2z:
            return CUFFT_D2Z;
        case type_t::z2d:
            return CUFFT_Z2D;
        case type_t::z2z:
            return CUFFT_Z2Z;
        }
        EMU_UNREACHABLE;
    }

} // namespace cufft

} // namespace emu
