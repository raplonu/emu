#ifndef EMU_TYPE_TRAITS_H
#define EMU_TYPE_TRAITS_H

#include <type_traits>
#include <iterator>

namespace emu
{

    struct use_default{};

    template<typename T, typename Other>
    using NotDefaultOr = std::conditional_t<std::is_same<T, use_default>::value, Other, T>;

    template<bool B>
    using EnableIf = std::enable_if_t<B, bool>;

    template<typename T1, typename T2>
    using Equivalent = std::is_same<std::decay_t<T1>, std::decay_t<T2>>;

    // Behave exactly as the std::remove_cvref_t of C++20.
    template< class T >
    using RemoveCVRef = typename std::remove_cv_t<std::remove_reference_t<T>>;

    template<typename It>
    using IteratorValue = typename std::iterator_traits<It>::value_type;

    template<typename It>
    using IteratorDifference = typename std::iterator_traits<It>::difference_type ;

    template<typename It>
    using IteratorCategory = typename std::iterator_traits<It>::category ;

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

}

#endif //EMU_TYPE_TRAITS_H