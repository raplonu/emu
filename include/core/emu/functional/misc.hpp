#pragma once

#include <emu/macro.h>
#include <emu/tuple.h>
#include <emu/utility.hpp>

namespace emu
{

namespace functional
{
    template<std::size_t KeyId>
    struct max_at_t {
        template<typename Tuple>
        EMU_HODE constexpr
        const Tuple & operator()(const Tuple & t1, const Tuple & t2) const noexcept {
            return max<KeyId>(t1, t2);
        }
    };

    template<std::size_t KeyId>
    EMU_HODE constexpr
    auto max_at() noexcept {
        return max_at_t<KeyId>{};
    }


    template<std::size_t KeyId>
    struct min_at_t {
        template<typename Tuple>
        EMU_HODE constexpr
        const Tuple & operator()(const Tuple & t1, const Tuple & t2) const noexcept {
            return min<KeyId>(t1, t2);
        }
    };

    template<std::size_t KeyId>
    EMU_HODE constexpr
    auto min_at() noexcept {
        return min_at_t<KeyId>{};
    }
    template<typename T>
    struct div_by_t {
        T n;

        EMU_HODE constexpr
        T operator()(const T & v) const noexcept {
            return v / n;
        }
    };

    template<typename T>
    EMU_HODE constexpr
    auto div_by(const T & t) noexcept {
        return div_by_t<T>{t};
    }


    template<typename T>
    struct mod_by_t {
        T n;

        EMU_HODE constexpr
        T operator()(const T & v) const noexcept {
            return v % n;
        }
    };

    template<typename T>
    EMU_HODE constexpr
    auto mod_by(const T & t) noexcept {
        return mod_by_t<T>{t};
    }

    struct Adder {
        template<typename T1, typename T2>
        EMU_HODE constexpr
        auto operator()(const T1 & t1, const T2 & t2) const noexcept {
            return t1 + t2;
        }
    };

    struct Mult {
        template<typename T1, typename T2>
        EMU_HODE constexpr
        auto operator()(const T1 & t1, const T2 & t2) const noexcept {
            return t1 * t2;
        }
    };

    struct xy_to_pixel_pos {
        std::size_t frame_size;

        EMU_HODE constexpr
        std::size_t operator()(std::size_t x, std::size_t y) const noexcept {
            return x + y * frame_size;
        }
    };

    template<typename Dest>
    struct cast_to {
        template<typename Src>
        EMU_HODE constexpr
        Dest operator()(Src src) const noexcept {
            return static_cast<Dest>(src);
        }

    };

    struct TupleAdd
    {
        template<typename Tuple>
        constexpr EMU_HODE
        auto operator()(const Tuple & t1, const Tuple & t2) const noexcept {
            return tuple::transform(Adder{}, t1, t2);
        }
    };

} // namespace functional


} // namespace emu
