#ifndef EMU_TUPLE_H
#define EMU_TUPLE_H

#include <emu/macro.h>
#include <emu/type_traits.h>
#include <emu/utility.h>
#include <emu/functional/invoke.h>

#include <tuple>

#if EMU_CUDACC
#include <thrust/tuple.h>
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
    using Size = detail::SizeImpl<RemoveCVRef<T>>;

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
        EMU_NOEXCEPT_EXPR( detail::MakeBased<RemoveCVRef<Tuple>>::make(EMU_FWD(args)...) )
    {
        return detail::MakeBased<RemoveCVRef<Tuple>>::make(EMU_FWD(args)...);
    }

namespace detail
{

    template<std::size_t I, typename Fn, typename Tuple, typename... Tuples>
    constexpr EMU_HODE
    auto transform_impl_at(Fn && fn, Tuple && tuple, Tuples &&... tuples)
    {
        using std::get;
        return functional::invoke(EMU_FWD(fn), EMU_FWD(get<I>(tuple)), EMU_FWD(get<I>(tuples))...);
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

#endif //EMU_TUPLE_H