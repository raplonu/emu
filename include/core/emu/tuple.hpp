#pragma once

#include <emu/macro.hpp>
#include <emu/type_traits.hpp>
#include <emu/utility.hpp>

#include <tuple>
#include <functional>

#if EMU_CUDACC
#include <thrust/tuple.h>

template<typename... T>
struct std::tuple_size<::thrust::tuple<T...>>
    : std::integral_constant<std::size_t, ::thrust::tuple_size<::thrust::tuple<T...>>::value > {};

template<std::size_t I, typename... T>
struct std::tuple_element<I, ::thrust::tuple<T...>> { using type = ::thrust::tuple_element<I, ::thrust::tuple<T...>>::type; };

namespace thrust
{

    template<std::size_t I, typename... T>
    constexpr auto get(::thrust::tuple<T...> & t) -> decltype(auto) { return ::thrust::get<I>(t); }

    template<std::size_t I, typename... T>
    constexpr auto get(const ::thrust::tuple<T...> & t) -> decltype(auto) { return ::thrust::get<I>(t); }

} // namespace std



#endif

namespace emu
{

namespace tuple
{

namespace detail
{

    template<typename T>
    struct SizeImpl {
        static constexpr std::size_t value = std::tuple_size<T>::value;
    };

#if EMU_CUDACC
    template<typename... Ts>
    struct SizeImpl<::thrust::tuple<Ts...>> {
        static constexpr std::size_t value = ::thrust::tuple_size<::thrust::tuple<Ts...>>::value;
    };
#endif

} // namespace detail

    template<typename T>
    using Size = detail::SizeImpl<std::remove_cvref_t<T>>;

namespace detail
{
    template<typename T>
    struct MakeBased {

        template<typename... Types >
        static constexpr EMU_HODE
        auto make( Types&&... args )
            EMU_NOEXCEPT_EXPR( std::make_tuple(EMU_FWD(args)...) )
        {
            return std::make_tuple(EMU_FWD(args)...);
        }
    };

#if EMU_CUDACC
    template<typename... Ts>
    struct MakeBased<::thrust::tuple<Ts...>> {

        template<typename... Types >
        static constexpr EMU_HODE
        auto make( Types&&... args )
            EMU_NOEXCEPT_EXPR( ::thrust::make_tuple(EMU_FWD(args)...) )
        {
            return ::thrust::make_tuple(EMU_FWD(args)...);
        }
    };
#endif
} // namespace detail


    template<typename Tuple, typename... Types >
    constexpr EMU_HODE
    auto make_tuple_base_on( Types&&... args )
        EMU_NOEXCEPT_EXPR( detail::MakeBased<std::remove_cvref_t<Tuple>>::make(EMU_FWD(args)...) )
    {
        return detail::MakeBased<std::remove_cvref_t<Tuple>>::make(EMU_FWD(args)...);
    }

namespace detail
{

    template<std::size_t I, typename Fn, typename Tuple, typename... Tuples>
    constexpr EMU_HODE
    auto transform_impl_at(Fn && fn, Tuple && tuple, Tuples &&... tuples)
    {
        using std::get;
        return std::invoke(EMU_FWD(fn), EMU_FWD(get<I>(tuple)), EMU_FWD(get<I>(tuples))...);
    }

    template<typename Fn, typename Tuple, std::size_t... I, typename... Tuples>
    constexpr EMU_HODE
    auto transform_impl(std::index_sequence<I...>, Fn && fn, Tuple && tuple, Tuples &&... tuples)
        EMU_NOEXCEPT_EXPR( make_tuple_base_on<Tuple>(transform_impl_at<I>(fn, tuple, tuples...)...) )
    {
        return make_tuple_base_on<Tuple>(transform_impl_at<I>(fn, tuple, tuples...)...);
    }

} // namespace detail

    template<typename Fn, typename Tuple, typename... Tuples>
    constexpr EMU_HODE
    auto transform(Fn && fn, Tuple && tuple, Tuples &&... tuples)
        EMU_NOEXCEPT_EXPR(
            detail::transform_impl(std::make_index_sequence<Size<Tuple>::value>{}, EMU_FWD(fn), EMU_FWD(tuple), EMU_FWD(tuples)...)
        )
    {
        return detail::transform_impl(
            std::make_index_sequence<Size<Tuple>::value>{},
            EMU_FWD(fn), EMU_FWD(tuple), fwd(tuples)...
        );
    }

} // namespace tuple

} // namespace emu
