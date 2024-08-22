#pragma once

#include <emu/macro.hpp>
#include <emu/fwd.hpp>

#include <boost/logic/tribool.hpp>

#include <type_traits>
#include <ranges>
#include <tuple>
#include <array>

namespace emu
{

    template <typename T> using tid = std::type_identity_t<T>;

    template <typename T> using rm_cv = std::remove_cv_t<T>;
    template <typename T> using rm_const = std::remove_const_t<T>;
    template <typename T> using rm_volatile = std::remove_volatile_t<T>;
    template <typename T> using rm_ref = std::remove_reference_t<T>;
    template <typename T> using rm_cvref = rm_cv<rm_ref<T>>;
    template <typename T> using rm_ptr = std::remove_pointer_t<T>;
    template <typename T> using rm_extent = std::remove_extent_t<T>;
    template <typename T> using rm_all_extents = std::remove_all_extents_t<T>;
    template <typename T> using decay = std::decay_t<T>;

    template <typename T> using add_cv = std::add_cv_t<T>;
    template <typename T> using add_const = std::add_const_t<T>;
    template <typename T> using add_volatile = std::add_volatile_t<T>;
    template <typename T> using add_ptr = std::add_pointer_t<T>;
    template <typename T> using add_lref = std::add_lvalue_reference_t<T>;
    template <typename T> using add_rref = std::add_rvalue_reference_t<T>;

    template <typename T> constexpr bool is_const = std::is_const_v<T>;
    template <typename T> constexpr bool is_volatile = std::is_volatile_v<T>;
    template <typename T> constexpr bool is_ref = std::is_reference_v<T>;
    template <typename T> constexpr bool is_lref = std::is_lvalue_reference_v<T>;
    template <typename T> constexpr bool is_rref = std::is_rvalue_reference_v<T>;
    template <typename T> constexpr bool is_ptr = std::is_pointer_v<T>;

    struct use_default{};

    template <typename T, typename Other>
    using not_default_or = std::conditional_t<std::is_same_v<T, use_default>, Other, T>;

    template <class, template <class...> class>
    constexpr bool is_specialization = false;

    template <template <class...> class T, class... Args>
    constexpr bool is_specialization<T<Args...>, T> = true;

    template<typename T, typename Reference>
    using propagate_const = std::conditional_t<std::is_const_v<Reference>, std::add_const_t<T>, T>;

namespace detail
{

    template <typename T>
    struct is_extents : std::false_type {};

    template <typename IndexType, std::size_t... Extents>
    struct is_extents<std::experimental::extents<IndexType, Extents...>> : std::true_type {};

} // namespace detail

    template <typename T>
    constexpr bool is_extents = detail::is_extents<T>::value;

    template <typename... Ts>
    struct type_pack {};

    template <typename T>
    using make_type_pack_from_tuple = decltype([]<typename... Ts>(std::tuple<Ts...>) { return type_pack<Ts...>{}; }(std::declval<T>()));

    /**
     * @brief Utility to delay the evaluation of a static_assert.
     *
     * Example:
     *
     * template<typename T>
     * void toto() {
     *    //
     *    static_assert(dependent_false<T>, "T is not valid");
     * }
     *
     *
     * @tparam typename
     */
    template <typename>
    constexpr bool dependent_false = false;

    /**
     * @brief Utility to delay the evaluation of a static_assert.
     *
     * @tparam typename
     */
    template <auto>
    constexpr bool dependent_false_v = false;

namespace detail
{
    // General declaration
    template<bool Condition, auto IfTrue, auto IfFalse>
    struct conditional;

    // Definition if true
    template<auto IfTrue, auto IfFalse>
    struct conditional<true, IfTrue, IfFalse> {
        static constexpr auto value = IfTrue;
    };
    // Definition if false
    template<auto IfTrue, auto IfFalse>
    struct conditional<false, IfTrue, IfFalse> {
        static constexpr auto value = IfFalse;
    };
} // namespace detail

    template<bool B, auto IfTrue, auto IfFalse>
    constexpr auto conditional = detail::conditional<B, IfTrue, IfFalse>::value;

    /**
     * @brief Returns the actual type of the element in a range.
     *
     * Could not rely on std::ranges::range_value_t as it discards cv-qualifiers.
     *
     * Examples:
     * static_assert(std::is_same_v<range_cv_value<std::vector<int>>, int>);
     * static_assert(std::is_same_v<range_cv_value<std::vector<int> const>, const int>);
     *
     * static_assert(std::is_same_v<range_cv_value<std::span<int>>, int>);
     * static_assert(std::is_same_v<range_cv_value<const std::span<int>>, int>);
     * static_assert(std::is_same_v<range_cv_value<std::span<const int>>, const int>);
     *
     *
     * @tparam Range
     */
    template<std::ranges::range Range>
    using range_cv_value = rm_ref< std::ranges::range_reference_t< Range > >;

    /**
     * @brief Returns the actual type of the element dereferenceable by an iterator.
     *
     * @tparam It
     */
    template<std::input_iterator It>
    using iterator_cv_value = rm_ref< std::iter_reference_t< It > >;

namespace spe
{

    /**
     * @brief Host range use a blacklist approach to determine if a type is a valid source for a host range.
     *
     * This means that all types are valid sources for host ranges unless explicitly blacklisted.
     *
     * A host range is a range that can be accessed from the CPU host.
     *
     */
    template <typename>
    inline constexpr bool enable_host_range = true;

    /**
     * @brief Cuda device range use a whitelist approach to determine if a type is a valid source for a cuda device range.
     *
     * This means that all types are invalid valid sources for cuda device ranges unless explicitly whitelisted.
     *
     * A cuda device range is a range that can be accessed from the GPU device.
     *
     */
    template <typename>
    inline constexpr bool enable_cuda_device_range = false;


    /**
     * @brief Relocation range use a blacklist for rvalue refenrence and whitelist for lvalue reference
     * approach to determine if a type is a valid source for a relocatable range.
     *
     */
    template <typename T>
    inline constexpr boost::logic::tribool enable_relocatable_owning_range = boost::logic::indeterminate;

} // namespace spe

} // namespace emu
