#pragma once

#include <emu/config.hpp>
#include <emu/macro.hpp>
#include <emu/type_traits.hpp>

#include <boost/callable_traits/args.hpp>
#include <boost/hana/core/make.hpp>

#include <type_traits>
#include <algorithm>
#include <functional>
#include <ranges>
#include <utility>
#include <memory>
#include <cstddef>
#include <string_view>
#include <ranges>
#include <span>

namespace emu
{

    using std::size_t, std::ptrdiff_t, std::byte;

    using std::string_view;

    using boost::hana::make;

    template<typename T>
    inline byte* b_ptr_of(const T* t) {
        return const_cast<byte*>(reinterpret_cast<const byte*>(t));
    }

    template<typename T, size_t Extent>
    inline byte* b_ptr_of(std::span<T, Extent> s) {
        return b_ptr_of(s.data());
    }

    template<typename T>
    inline void* v_ptr_of(const T* t) {
        return const_cast<void*>(reinterpret_cast<const void*>(t));
    }

    template<typename T, size_t Extent>
    inline void* v_ptr_of(std::span<T, Extent> s) {
        return v_ptr_of(s.data());
    }

    // template<typename T>
    // auto data_handle(const T& t) noexcept {
    //     if      constexpr (std::is_pointer_v<T>)
    //         return t;
    //     else if constexpr (std::ranges::contiguous_range<T>)
    //         return t.data();
    //     else if constexpr (cpts::any_mdspan<T>)
    //         return t.data_handle();
    // }

    constexpr inline auto split_string(string_view words, string_view delim = " ") {
        return std::views::split(words, delim) | std::views::transform([](auto sub_str) {
                // sub_str is not categorized as contiguous range but it is.
                // We have to use the deref hack to construct the string_view.
                return string_view(&*sub_str.begin(), std::ranges::distance(sub_str));
            });
    }


    /**
    * Calculate the ceil result of a / b
    * @param  a numerator
    * @param  b denominator
    * @return   ceil division of a / b
    */
    template <typename T>
    EMU_HODE constexpr
    T ceil(T a, T b) noexcept { return (a + b - 1) / b; }

    template<typename T>
    EMU_HODE constexpr
    T next_mul(T a, T b) noexcept { return ( (a - 1) / b + 1) * b; }

    template<size_t key_id, typename Tuple>
    EMU_HODE constexpr
    const Tuple & min(const Tuple & t1, const Tuple & t2) noexcept {
        using std::get;
        return get<key_id>(t1) < get<key_id>(t2) ? t1 : t2;
    }

    template<size_t key_id, typename Tuple>
    EMU_HODE constexpr
    const Tuple & max(const Tuple & t1, const Tuple & t2) noexcept {
        using std::get;
        return get<key_id>(t1) < get<key_id>(t2) ? t2 : t1;
    }

    /**
     * @brief Return the generalized number of item per group. This number is the same for every group.
     * The sum of the result for every block may be equal or superior of item_nb
     * Esamples :
     * item_per_group_local_nb(100, 10) -> 10
     * item_per_group_local_nb(89, 10) -> 9
     * @param item_nb
     * @param group_size
     * @return int item_per_group_nb
     */
    EMU_HODE inline
    int item_per_group_nb(int item_nb, int group_size) { return ceil(item_nb, group_size); }

    /**
     * @brief Share a resource between a group. Guaranty to minimize the number of resource
     * for each group element and to handle exactly the right number of available item
     * Examples :
     * Item are perfectly shared between all groups
     * item_per_group_local_nb(100, [0, .., 9], 10) -> [10, .., 10]
     * The last group handle only 1 element to unsure item is not handled two times
     * item_per_group_local_nb(7, [0, 1, 2, 3], 4) -> [2, 2, 2, 1]
     * The last group remain idle because the is not element at all to handle
     * item_per_group_local_nb(5, [0, 1, 2, 3], 4) -> [2, 2, 1, 0]
     *
     * @param item_nb Total number of item to handle
     * @param group_id The id of the current group
     * @param group_size The number of element in the group
     * @return int item_per_group_local_nb
     */
    EMU_HODE inline
    int item_per_group_local_nb(int item_nb, int group_id, int group_size) {
        int item_pg = item_per_group_nb(item_nb, group_size);

        return std::max(0, std::min(item_nb, (group_id + 1) * item_pg) - group_id * item_pg);
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
        detail::apply_impl<TypeLists...>::template product(EMU_FWD(c), EMU_FWD(args)...);
    }

    template<auto V>
    struct v_t {
        using type = decltype(V);
        static constexpr type value = V;
    };

    /**
     * @brief Call the fn callable with an instance of emu::type_pack. The types
     * in type_pack are based on the domain (arguments) of the reference
     * callable.
     *
     * @tparam Fn A polymorphic callable object that takes one type_pack argument.
     * @tparam ReferenceFn The reference callable object.
     * @param fn The function to call.
     * @return The result of the function call with the type_pack.
     */
    template<typename Fn, typename ReferenceFn>
    constexpr decltype(auto) invoke_with_args(Fn&& fn, const ReferenceFn&) {
        namespace ct = boost::callable_traits;

        return std::invoke(EMU_FWD(fn), make_type_pack_from_tuple<ct::args_t<Fn>>{});
    }

} // namespace emu
