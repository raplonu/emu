#pragma once

#include <emu/config.hpp>
#include <emu/macro.hpp>
#include <emu/type_traits.hpp>

#include <type_traits>
#include <algorithm>
#include <functional>

#include <utility>
#include <memory>
#include <cstddef>
#include <string_view>

#include <span>

namespace emu
{

    using std::size_t, std::ptrdiff_t, std::byte;

    using std::string_view;

    template<typename T>
    inline byte* b_ptr_of(const T* t) {
        return const_cast<byte*>(reinterpret_cast<const byte*>(t));
    }

    template<typename T, size_t Extent>
    inline byte* b_ptr_of(std::span<T, Extent> s) {
        return b_ptr_of(s.data());
    }

    template<typename T>
    inline void* rest(const T* t) {
        return const_cast<void*>(reinterpret_cast<const void*>(t));
    }

    template<typename T>
    inline void* v_ptr_of(const T* t) {
        return const_cast<void*>(reinterpret_cast<const void*>(t));
    }

    template<typename T, size_t Extent>
    inline void* v_ptr_of(std::span<T, Extent> s) {
        return v_ptr_of(s.data());
    }

    template<typename T, typename Alloc>
    constexpr auto rebind_alloc(const Alloc& alloc) {
        return typename std::allocator_traits<Alloc>::template rebind_alloc<T>(alloc);
    }

namespace detail
{

    template<typename... TypeLists>
    struct apply_impl;

    template<typename TypeList, typename... TypeLists>
    struct apply_impl<TypeList, TypeLists...> {

        template<typename... ActualTypes, typename Caller, typename... Args>
        static void product(Caller c, Args... args) {
            [&] <size_t... I> (std::index_sequence<I...>) {
                (apply_impl<TypeLists...>::template product<ActualTypes..., std::tuple_element_t<I, TypeList>>(c, args...), ...);
            }(std::make_index_sequence<std::tuple_size_v<TypeList>>{});
        }
    };

    template<>
    struct apply_impl<> {
        template<typename... ActualTypes, typename Caller, typename... Args>
        static void product(Caller c, Args... args) {
            if constexpr(sizeof...(ActualTypes) > 0)
                c.template operator()<ActualTypes...>(args...);
            else
                // Allow special case with 0 type arguments.
                c.operator()(args...);
        }
    };

}

    /**
     * @brief Call a template callable with each combination of each type list.
     *
     * @tparam TypeLists A variadic number of type list or value list (i.e. using `emu::v_t`).
     * @tparam Caller A callable type
     * @tparam Args
     * @param c a callable object that may accept the template arguments.
     * @param args additional argument to be forwarded to the function.
     */
    template<typename... TypeLists, typename Caller, typename... Args>
    void product(Caller&& c, Args&&... args) {
        detail::apply_impl<TypeLists...>::product(EMU_FWD(c), EMU_FWD(args)...);
    }

    template<auto V>
    struct v_t {
        using type = decltype(V);
        static constexpr type value = V;
    };

} // namespace emu
