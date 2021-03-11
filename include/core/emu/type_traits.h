#ifndef EMU_TYPE_TRAITS_H
#define EMU_TYPE_TRAITS_H

#include <emu/macro.h>

#if EMU_CUDA
#include <thrust/tuple.h>
#endif

#include <type_traits>
#include <iterator>
#include <tuple>

namespace emu
{

namespace detail
{

    template<typename T>
    struct IdentityImpl { using type = T; };

} // namespace detail


    template<typename T>
    using Identity = typename detail::IdentityImpl<T>::type;

    struct use_default{};

    template<typename T, typename Other>
    using NotDefaultOr = std::conditional_t<std::is_same<T, use_default>::value, Other, T>;

    template<bool B>
    using EnableIf = std::enable_if_t<B, bool>;

    template<typename T1, typename T2>
    using Equivalent = std::is_same<std::decay_t<T1>, std::decay_t<T2>>;

    // Behave exactly as the  std::remove_cvref_t of C++20.
    template< class T >
    using RemoveCVRef = typename std::remove_cv_t<std::remove_reference_t<T>>;

    template<typename T>
    using ValueType      = typename RemoveCVRef<T>::value_type;
    template<typename T>
    using DifferenceType = typename RemoveCVRef<T>::difference_type;
    template<typename T>
    using Category       = typename RemoveCVRef<T>::category;

    template<typename It>
    using IteratorValue      = ValueType<std::iterator_traits<RemoveCVRef<It>>>;
    template<typename It>
    using IteratorDifference = DifferenceType<std::iterator_traits<RemoveCVRef<It>>>;
    template<typename It>
    using IteratorCategory   = Category<std::iterator_traits<RemoveCVRef<It>>>;

// use a specific namespace name in order to avoid namespace polution with `using std::begin;`
namespace detail_begin
{
    using std::begin;
    template<typename Rg>
    using IteratorTypeImpl = decltype(begin(std::declval<Rg>()));

} // namespace detail_begin

    template<typename Rg>
    using IteratorType = detail_begin::IteratorTypeImpl<Rg>;

    template<typename Rg>
    using RangeValue      = IteratorValue<IteratorType<RemoveCVRef<Rg>>>;
    template<typename Rg>
    using RangeDifference = IteratorDifference<IteratorType<RemoveCVRef<Rg>>>;
    template<typename Rg>
    using RangeCategory   = IteratorCategory<IteratorType<RemoveCVRef<Rg>>>;

    struct AutoTag{};

    template<typename T>
    using IsAutoTag = std::is_same<AutoTag, std::remove_cv_t<T>>;

    template<typename T, typename U>
    using IfNotAutoOr = std::conditional_t<IsAutoTag<T>::value, U, T>;

namespace detail
{
    template<typename T>
    struct SizeImpl {
        static constexpr std::size_t value = std::tuple_size<T>::value;
    };

#if EMU_CUDA
    template<typename... Ts>
    struct SizeImpl<::thrust::tuple<Ts...>> {
        static constexpr std::size_t value = ::thrust::tuple_size<::thrust::tuple<Ts...>>::value;
    };
#endif
} // namespace detail

    template<typename T>
    using Size = detail::SizeImpl<RemoveCVRef<T>>;
}

#endif //EMU_TYPE_TRAITS_H