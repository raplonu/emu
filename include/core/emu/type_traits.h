#ifndef EMU_TYPE_TRAITS_H
#define EMU_TYPE_TRAITS_H

#include <emu/macro.h>

#include <type_traits>
#include <iterator>

#include <tuple>

#if EMU_CUDA
#include <thrust/tuple.h>
#endif

namespace emu
{

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

    namespace detail
    {
        template<typename R>
        auto RangeValueTypeImpl(R r) {
            using std::begin;
            return *begin(r);
        }
    }

    template<typename R>
    using RangeValueType = decltype(detail::RangeValueTypeImpl(std::declval<R>()));

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