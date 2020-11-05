#ifndef EMU_FUNCTIONAL_MISC_H
#define EMU_FUNCTIONAL_MISC_H

#include <emu/macro.h>
#include <emu/utility.h>

namespace emu
{

namespace functional
{
    template<std::size_t key_id>
    struct max_at {
        template<typename Tuple>
        EMU_HODE constexpr
        const Tuple & operator()(const Tuple & t1, const Tuple & t2) const noexcept {
            return max<key_id>(t1, t2);
        }
    };

    template<std::size_t key_id>
    struct min_at {
        template<typename Tuple>
        EMU_HODE constexpr
        const Tuple & operator()(const Tuple & t1, const Tuple & t2) const noexcept {
            return min<key_id>(t1, t2);
        }
    };

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

    struct xy_to_pixel_pos {
        std::size_t frame_size;

        EMU_HODE constexpr
        std::size_t operator()(std::size_t x, std::size_t y) const noexcept {
            return x + y * frame_size;
        }
    };

} // namespace functional


} // namespace emu

#endif //EMU_FUNCTIONAL_MISC_H