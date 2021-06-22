#ifndef EMU_TYPE_TRAITS_H
#define EMU_TYPE_TRAITS_H

#include <emu/macro.h>

#if EMU_CUDACC
#include <thrust/tuple.h>
#endif

#include <type_traits>
#include <iterator>
#include <tuple>
#include <cstdint>

namespace emu
{

    ///  Primary type categories
    template<typename T> constexpr auto IsVoid                  = std::is_void<T>::value;
    template<typename T> constexpr auto IsNullPointer           = std::is_null_pointer<T>::value;
    template<typename T> constexpr auto IsIntegral              = std::is_integral<T>::value;
    template<typename T> constexpr auto IsFloatingPoint         = std::is_floating_point<T>::value;
    template<typename T> constexpr auto IsArray                 = std::is_array<T>::value;
    template<typename T> constexpr auto IsEnum                  = std::is_enum<T>::value;
    template<typename T> constexpr auto IsUnion                 = std::is_union<T>::value;
    template<typename T> constexpr auto IsClass                 = std::is_class<T>::value;
    template<typename T> constexpr auto IsFunction              = std::is_function<T>::value;
    template<typename T> constexpr auto IsPointer               = std::is_pointer<T>::value;
    template<typename T> constexpr auto IsLvalueReference       = std::is_lvalue_reference<T>::value;
    template<typename T> constexpr auto IsRvalueReference       = std::is_rvalue_reference<T>::value;
    template<typename T> constexpr auto IsMemberObjectPointer   = std::is_member_object_pointer<T>::value;
    template<typename T> constexpr auto IsMemberFunctionPointer = std::is_member_function_pointer<T>::value;

    ///  Composite type categories
    template<typename T> constexpr auto IsFundamental   = std::is_fundamental<T>::value;
    template<typename T> constexpr auto IsArithmetic    = std::is_arithmetic<T>::value;
    template<typename T> constexpr auto IsScalar        = std::is_scalar<T>::value;
    template<typename T> constexpr auto IsObject        = std::is_object<T>::value;
    template<typename T> constexpr auto IsCompound      = std::is_compound<T>::value;
    template<typename T> constexpr auto IsReference     = std::is_reference<T>::value;
    template<typename T> constexpr auto IsMemberPointer = std::is_member_pointer<T>::value;

    ///  Type properties
    template<typename T> constexpr auto IsConst             = std::is_const<T>::value;
    template<typename T> constexpr auto IsVolatile          = std::is_volatile<T>::value;
    template<typename T> constexpr auto IsTrivial           = std::is_trivial<T>::value;
    template<typename T> constexpr auto IsTriviallyCopyable = std::is_trivially_copyable<T>::value;
    template<typename T> constexpr auto IsStandardLayout    = std::is_standard_layout<T>::value;

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

    template<bool B, typename T = bool>
    using EnableIf = std::enable_if_t<B, T>;

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
    template<std::size_t S>
    struct matching_word;

#define EMU_MATCH_SIZE(TYPE, SIZE)                               \
    template<> struct matching_word<SIZE> { using type = TYPE; }

    EMU_MATCH_SIZE(std::uint8_t,  1);
    EMU_MATCH_SIZE(std::uint16_t, 2);
    EMU_MATCH_SIZE(std::uint32_t, 4);
    EMU_MATCH_SIZE(std::uint64_t, 8);

#undef EMU_MATCH_SIZE

} // namespace detail

template<typename T>
using matching_word = typename detail::matching_word<sizeof(T)>::type;

}

#endif //EMU_TYPE_TRAITS_H