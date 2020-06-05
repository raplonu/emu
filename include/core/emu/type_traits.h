#ifndef EMU_TYPE_TRAITS_H
#define EMU_TYPE_TRAITS_H

#include <type_traits>
#include <iterator>

namespace emu
{
    template<typename It>
    using IteratorValue = typename std::iterator_traits<It>::value_type;

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