#ifndef EMU_TYPE_TRAITS_H
#define EMU_TYPE_TRAITS_H

#include <emu/macro.h>

#include <type_traits>
#include <iterator>

namespace emu
{

    /// Wraps type traits properties since helper variable template only available from C++17.

    /// Primary type categories
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

    /// Composite type categories
    template<typename T> constexpr auto IsFundamental   = std::is_fundamental<T>::value;
    template<typename T> constexpr auto IsArithmetic    = std::is_arithmetic<T>::value;
    template<typename T> constexpr auto IsScalar        = std::is_scalar<T>::value;
    template<typename T> constexpr auto IsObject        = std::is_object<T>::value;
    template<typename T> constexpr auto IsCompound      = std::is_compound<T>::value;
    template<typename T> constexpr auto IsReference     = std::is_reference<T>::value;
    template<typename T> constexpr auto IsMemberPointer = std::is_member_pointer<T>::value;

    /// Type properties
    template<typename T> constexpr auto IsConst             = std::is_const<T>::value;
    template<typename T> constexpr auto IsVolatile          = std::is_volatile<T>::value;
    template<typename T> constexpr auto IsTrivial           = std::is_trivial<T>::value;
    template<typename T> constexpr auto IsTriviallyCopyable = std::is_trivially_copyable<T>::value;
    template<typename T> constexpr auto IsStandardLayout    = std::is_standard_layout<T>::value;

    /// Type relationships
    template<typename T1, typename T2>        constexpr auto IsSame        = std::is_same<T1, T2>::value;
    template<typename Base, typename Derived> constexpr auto IsBaseOf      = std::is_base_of<Base, Derived>::value;
    template<typename From, typename To>      constexpr auto IsConvertible = std::is_convertible<From, To>::value;

    /// Supported operations
    template<typename T, typename... Args> constexpr auto IsConstructible          = std::is_constructible<T, Args...>::value;
    template<typename T, typename... Args> constexpr auto IsTriviallyConstructible = std::is_trivially_constructible<T, Args...>::value;
    template<typename T, typename... Args> constexpr auto IsNothrowConstructible   = std::is_nothrow_constructible<T, Args...>::value;
    template<typename T> constexpr auto IsDefaultConstructible                     = std::is_default_constructible<T>::value;
    template<typename T> constexpr auto IsTriviallyDefaultConstructible            = std::is_trivially_default_constructible<T>::value;
    template<typename T> constexpr auto IsNothrowDefaultConstructible              = std::is_nothrow_default_constructible<T>::value;
    template<typename T> constexpr auto IsCopyConstructible                        = std::is_copy_constructible<T>::value;
    template<typename T> constexpr auto IsTriviallyCopyConstructible               = std::is_trivially_copy_constructible<T>::value;
    template<typename T> constexpr auto IsNothrowCopyConstructible                 = std::is_nothrow_copy_constructible<T>::value;
    template<typename T> constexpr auto IsMoveConstructible                        = std::is_move_constructible<T>::value;
    template<typename T> constexpr auto IsTriviallyMoveConstructible               = std::is_trivially_move_constructible<T>::value;
    template<typename T> constexpr auto IsNothrowMoveConstructible                 = std::is_nothrow_move_constructible<T>::value;
    template<typename T, typename U> constexpr auto IsAssignable                   = std::is_assignable<T, U>::value;
    template<typename T, typename U> constexpr auto IsTriviallyAssignable          = std::is_trivially_assignable<T, U>::value;
    template<typename T, typename U> constexpr auto IsNothrowAssignable            = std::is_nothrow_assignable<T, U>::value;
    template<typename T> constexpr auto IsCopyAssignable                           = std::is_copy_assignable<T>::value;
    template<typename T> constexpr auto IsTriviallyCopyAssignable                  = std::is_trivially_copy_assignable<T>::value;
    template<typename T> constexpr auto IsNothrowCopyAssignable                    = std::is_nothrow_copy_assignable<T>::value;
    template<typename T> constexpr auto IsMoveAssignable                           = std::is_move_assignable<T>::value;
    template<typename T> constexpr auto IsTriviallyMoveAssignable                  = std::is_trivially_move_assignable<T>::value;
    template<typename T> constexpr auto IsNothrowMoveAssignable                    = std::is_nothrow_move_assignable<T>::value;
    template<typename T> constexpr auto IsDestructible                             = std::is_destructible<T>::value;
    template<typename T> constexpr auto IsTriviallyDestructible                    = std::is_trivially_destructible<T>::value;
    template<typename T> constexpr auto IsNothrowDestructible                      = std::is_nothrow_destructible<T>::value;
    template<typename T> constexpr auto HasVirtualDestructor                       = std::has_virtual_destructor<T>::value;

    /**
     * @brief Behave exactly as the std::enable_if_t but default output type as bool.
     *
     * Facilitates writing functions with similar domain but with different constraints.
     *
     * When testing using type template parameter, a simple approach would be to write this:
     * `template<typename T, typename = std::enable_if_t<condition_v<T> > >
     * void fun(T);`
     * But this approach wont compile with overload functions such as
     * `template<typename T, typename = std::enable_if_t<other_condition_v<T> > >
     * void fun(T);`
     *
     * Example: Two different functions, one taking integral, other floating point.
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * template<typename T, emu::EnableIf<emu::IsIntegral<T>> = true>
     * void fun(T);
     *
     * template<typename T, emu::EnableIf<emu::IsFloatingPoint<T>> = true>
     * void fun(T);
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     */
    template<bool B, typename T = bool>
    using EnableIf = std::enable_if_t<B, T>;

namespace detail
{

    template<typename T>
    struct IdentityImpl { using type = T; };

} // namespace detail

    /// Behave exactly as the std::type_identity_t of C++20.
    /// See also <a href="https://en.cppreference.com/w/cpp/language/template_argument_deduction#Non-deduced_contexts">non-deduced contexts</a>
    template<typename T>
    using Identity = typename detail::IdentityImpl<T>::type;

    struct use_default{};

    template<typename T, typename Other>
    using NotDefaultOr = std::conditional_t<std::is_same<T, use_default>::value, Other, T>;

    template<typename T1, typename T2>
    constexpr auto Equivalent = IsSame<std::decay_t<T1>, std::decay_t<T2>>;

    template< class T >
    using RemoveRef = std::remove_reference_t<T>;

    template< class T >
    using RemoveCV = std::remove_cv_t<T>;

    // Behave exactly as the std::remove_cvref_t of C++20.
    template< class T >
    using RemoveCVRef = RemoveCV<RemoveRef<T>>;

    template<typename T, typename U>
    using ForwardConst = std::conditional_t<IsConst<std::remove_reference_t<T>>, std::add_const_t<std::remove_reference_t<U>>, U>;

    template<typename T>
    using ValueType      = typename RemoveRef<T>::value_type;
    template<typename T>
    using DifferenceType = typename RemoveRef<T>::difference_type;
    template<typename T>
    using Category       = typename RemoveRef<T>::iterator_category;

    template<typename It>
    using IteratorValue      = ValueType<std::iterator_traits<RemoveRef<It>>>;
    template<typename It>
    using IteratorDifference = DifferenceType<std::iterator_traits<RemoveRef<It>>>;
    template<typename It>
    using IteratorCategory   = Category<std::iterator_traits<RemoveRef<It>>>;

// use a specific namespace name in order to avoid namespace polution with `using std::begin;`
namespace detail_begin
{

    using std::begin;
    template<typename Rg>
    using RangeIteratorTypeImpl = decltype(begin(std::declval<Rg>()));

} // namespace detail_begin

    template<typename Rg>
    using RangeIteratorType = detail_begin::RangeIteratorTypeImpl<Rg>;

    template<typename Rg>
    using RangeValue      = IteratorValue<RangeIteratorType<Rg>>;
    template<typename Rg>
    using RangeDifference = IteratorDifference<RangeIteratorType<Rg>>;
    template<typename Rg>
    using RangeCategory   = IteratorCategory<RangeIteratorType<Rg>>;

namespace detail
{

    template<typename Container>
    using ContainerIteratorTypeImpl = decltype(std::declval<Container>().data());

} // namespace detail

    template<typename Rg>
    using ContainerIteratorType = detail::ContainerIteratorTypeImpl<Rg>;

    template<typename Rg>
    using ContainerValue      = IteratorValue<ContainerIteratorType<Rg>>;
    template<typename Rg>
    using ContainerDifference = IteratorDifference<ContainerIteratorType<Rg>>;
    template<typename Rg>
    using ContainerCategory   = IteratorCategory<ContainerIteratorType<Rg>>;

    EMU_GENERATE_TRAITS_HAS(HasData, T, std::declval<T&>().data());
    EMU_GENERATE_TRAITS_HAS(HasSize, T, std::declval<T&>().size());

    template<typename Container>
    constexpr auto IsContainer = HasData<Container> and HasSize<Container> and IsSame<ContainerCategory<Container>, std::random_access_iterator_tag>;

    template<typename Location>
    struct is_location : std::false_type {};

    template<typename Location>
    constexpr auto IsLocation = is_location<Location>::value;

} // namespace emu

#endif //EMU_TYPE_TRAITS_H