#ifndef EMU_UTILITY_H
#define EMU_UTILITY_H

#include <emu/macro.h>

#include <type_traits>
#include <utility>

#include <thrust/tuple.h>

namespace emu
{
    template<typename T>
    EMU_HODE constexpr T&& fwd(std::remove_reference_t<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T>
    EMU_HODE constexpr T&& fwd(std::remove_reference_t<T>&& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T> constexpr
    EMU_HODE auto mv(T&& t) noexcept -> std::remove_reference_t<T> {
        return static_cast<std::remove_reference_t<T>>(t);
    }

    //###################### INVOKE ######################

    // C++14 workaround from https://stackoverflow.com/questions/38288042/c11-14-invoke-workaround
    // TODO : Delete when C++17 and cuda std provide it.
    template<typename Fn, typename... Args, 
    std::enable_if_t<std::is_member_pointer<std::decay_t<Fn>>{}, int> = 0 >
    EMU_HODE constexpr decltype(auto) invoke(Fn&& f, Args&&... args)
        EMU_NOEXCEPT_EXPR( std::mem_fn(f)(fwd<Args>(args)...) )
    {
        return std::mem_fn(f)(fwd<Args>(args)...);
    }

    template<typename Fn, typename... Args, 
            std::enable_if_t<!std::is_member_pointer<std::decay_t<Fn>>{}, int> = 0>
    EMU_HODE constexpr decltype(auto) invoke(Fn&& f, Args&&... args)
        EMU_NOEXCEPT_EXPR( fwd<Fn>(f)(fwd<Args>(args)...) )
    {
        return fwd<Fn>(f)(fwd<Args>(args)...);
    }

    //###################### APPLY ######################

    namespace detail {
        template <class F, class Tuple, std::size_t... I>
        EMU_HODE constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
        {
            return invoke(fwd<F>(f), thrust::get<I>(fwd<Tuple>(t))...);
        }
    }  // namespace detail

    // Behave exactly as the std::apply of C++17 with device support.
    // Implementation: https://en.cppreference.com/w/cpp/utility/apply.
    // TODO : Delete when C++17 and cuda std provide it.
    template <class F, class Tuple>
    EMU_HODE constexpr decltype(auto) apply(F&& f, Tuple&& t)
    {
        return detail::apply_impl(
            fwd<F>(f), fwd<Tuple>(t),
            std::make_index_sequence<thrust::tuple_size<std::remove_reference_t<Tuple>>::value>{});
    }
}

#endif //EMU_UTILITY_H