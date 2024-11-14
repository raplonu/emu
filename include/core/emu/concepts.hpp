#pragma once

#include <emu/fwd.hpp>
#include <emu/type_traits.hpp>

#include <fmt/core.h>

#include <concepts>
#include <ranges>
#include <type_traits>
#include <span>
#include <ranges>

namespace emu
{

namespace cpts
{

    // #################
    // # misc concepts #
    // #################

    using std::same_as;

    template <typename T1, typename T2>
    concept not_same_as = not same_as<T1, T2>;

    template <typename T1, typename T2>
    concept equivalent = same_as<decay<T1>, decay<T2>>;

    template <typename T1, typename T2>
    concept not_equivalent = not equivalent<T1, T2>;

    template <class T, template <class...> class Template>
    concept specialization_of = is_specialization<T, Template>;

    template <typename Derived, typename Base>
    concept not_derived_from = not std::derived_from<Derived, Base>;

    template <typename T>
    concept ref = is_ref<T>;

    template <typename T>
    concept lref = is_lref<T>;

    template<typename T>
    concept rref = is_rref<T>;

    template <typename T>
    concept not_ref = not is_ref<T>;

    template <typename T>
    concept not_lref = not is_lref<T>;

    template<typename T>
    concept not_rref = not is_rref<T>;

    template<typename T>
    concept pointer = std::is_pointer_v<T>;

    template<typename T>
    concept truthy = std::convertible_to<T, bool>;

    // #########################
    // # utility type concepts #
    // #########################

    template <typename T>
    concept expected = specialization_of<T, tl::expected>;

    template <typename T>
    concept optional = specialization_of<T, tl::optional>
                    or specialization_of<T, std::optional>;

    template <typename T>
    concept opt_like = expected<T> or optional<T>;

    // ############
    // # std view #
    // ############

    template <typename T>
    concept std_span = same_as<T, std::span<typename T::element_type, T::extent>>;

    template <typename T>
    concept std_mdspan = specialization_of<T, std::experimental::mdspan>;

    // ############
    // # emu view #
    // ############

    //TODO: using derived_from allow container to be considered to be a span.
    // Which is true, but may not be what we want.

    template <typename T>
    concept emu_span = std::derived_from<T, detail::basic_span<
                    typename T::element_type, T::extent,
                    typename T::location_type, typename T::actual_type>>;

    template <typename T>
    concept emu_mdspan = std::derived_from<T, detail::basic_mdspan<
                    typename T::element_type, typename T::extents_type,
                    typename T::layout_type, typename T::accessor_type,
                    typename T::location_type, typename T::actual_type>>;

    // #################
    // # span concepts #
    // #################

    // cannot use is_specialization here because second span argument is not a type.
    template <typename T>
    concept span = std_span<T>
                or emu_span<T>;

    template <typename T>
    concept const_span = span<T> and is_const<typename T::element_type>;

    template <typename T>
    concept mutable_span = span<T> and (not is_const<typename T::element_type>);

    // ######################
    // # container concepts #
    // ######################

    template <typename T>
    concept container = std::derived_from<T, detail::basic_container<typename T::element_type, T::extent, typename T::location_type, typename T::actual_type>>;

    template <typename T>
    concept const_container = container<T> and std::is_const_v<typename T::element_type>;

    template <typename T>
    concept mutable_container = container<T> and (not std::is_const_v<typename T::element_type>);

    // ###################
    // # mdspan concepts #
    // ###################

    template <typename T>
    concept mdspan = std_mdspan<T>
                  or emu_mdspan<T>;

    template <typename T>
    concept const_mdspan = mdspan<T> and std::is_const_v<typename T::element_type>;

    template <typename T>
    concept mutable_mdspan = mdspan<T> and (not std::is_const_v<typename T::element_type>);

    // ########################
    // # mdcontainer concepts #
    // ########################

    template <typename T>
    concept mdcontainer = std::derived_from<T, detail::basic_mdcontainer<
                    typename T::element_type, typename T::extents_type,
                    typename T::layout_type, typename T::accessor_type,
                    typename T::location_type, typename T::actual_type>>;

    template <typename T>
    concept const_mdcontainer = mdspan<T> and std::is_const_v<typename T::element_type>;

    template <typename T>
    concept mutable_mdcontainer = mdspan<T> and (not std::is_const_v<typename T::element_type>);

    // #################
    // # view concepts #
    // #################

    template <typename T>
    concept view = span<T> or mdspan<T>;

    template <typename T>
    concept const_view = const_span<T> or const_mdspan<T>;

    template <typename T>
    concept mutable_view = mutable_span<T> or mutable_mdspan<T>;

    // ####################
    // # capsule concetps #
    // ####################

    template <typename T>
    concept capsule_owner = requires (const T& t) {
        { t.capsule() } -> std::convertible_to<capsule>;
    };

    // ###################
    // # layout concepts #
    // ###################

    template <typename T>
    concept contiguous_sized_range = std::ranges::contiguous_range<T>
                                  and std::ranges::sized_range<T>;

    template <typename T>
    concept extents = is_extents<T>;

    template <typename T>
    concept mapping = requires (const T& m) {
        // TODO: complete the list...
        { m.required_span_size() } -> std::convertible_to<std::size_t>;

        { m.is_always_unique()     } -> std::convertible_to<bool>;
        { m.is_always_exhaustive() } -> std::convertible_to<bool>;
        { m.is_always_strided()    } -> std::convertible_to<bool>;
        { m.is_unique()            } -> std::convertible_to<bool>;
        { m.is_exhaustive()        } -> std::convertible_to<bool>;
        { m.is_strided()           } -> std::convertible_to<bool>;
    };

    template <typename T>
    // the only thing that determine a layout is the inner mapping type.
    concept layout = mapping< typename T::template mapping<std::experimental::extents<std::size_t>> >;

namespace detail
{

    template <typename T>
    struct is_std_array : std::false_type {};

    template <typename T, std::size_t N>
    struct is_std_array<std::array<T, N>> : std::true_type {};

} // namespace detail

    template <typename T>
    concept std_array = detail::is_std_array<decay<T>>::value;

    /**
     * @brief Concept that model a relocatable range.
     *
     * A relocatable owning range is a range that can be moved in a general way (can also be a copied) without
     * invalidating the iterators or the values' address. It test if the range is relocatable AND not borrowed.
     *
     * Example:
     * Types that are relocatable and owns data: std::vector
     * Types that are not relocatable or do not own the data: std::array, C arrays, std::span
     *
     *
     * @tparam T
     */
    template <typename T>
    concept relocatable_owning_range = conditional<
        // If enable_relocatable_owning_range and is not indenteterminate
        not boost::logic::indeterminate(spe::enable_relocatable_owning_range<rm_cvref<T>>),
        // Then, the type is relocatable if enable_relocatable_owning_range is true
        bool(spe::enable_relocatable_owning_range<rm_cvref<T>>),
        // otherwise, the type is relocatable if it's not a view nor a lvalue reference
        bool((not std::ranges::view<rm_ref<T>>)
        and (not is_lref<T>) )>;

    template <typename T>
    concept formattable = requires (const T& v, fmt::format_context ctx) {
        fmt::formatter<T>().format(v, ctx);
    };

    template <typename P>
    concept const_pointer = std::is_pointer_v<P> and std::is_const_v<std::remove_pointer_t<P>>;

    template <typename R>
    concept c_string = requires(const R r) {
        { r.c_str() } -> const_pointer; // Can't know for sure if it's a char type.
        { r.size() } -> std::convertible_to<std::size_t>;
    };

} // namespace cpts


namespace host::cpts
{

    template <typename T>
    concept span = std::derived_from<T, host::span<
                    typename T::element_type, T::extent>>;

    template <typename T>
    concept container = std::derived_from<T, host::container<
                    typename T::element_type, T::extent>>;

    template <typename T>
    concept mdspan = emu::cpts::specialization_of<T, host::mdspan>;

    template <typename T>
    concept mdcontainer = emu::cpts::specialization_of<T, host::mdcontainer>;

} // namespace host::cpts

namespace cuda::device::cpts
{

    template <typename T>
    concept span = std::derived_from<T, cuda::device::span<
                    typename T::element_type, T::extent>>;

    template <typename T>
    concept container = std::derived_from<T, cuda::device::container<
                    typename T::element_type, T::extent>>;

    template <typename T>
    concept mdspan = emu::cpts::specialization_of<T, cuda::device::mdspan>;

    template <typename T>
    concept mdcontainer = emu::cpts::specialization_of<T, cuda::device::mdcontainer>;

} // namespace cuda::device::cpts

namespace cpts
{

    template<typename T>
    concept any_span = span<T> or host::cpts::span<T> or cuda::device::cpts::span<T>;

    template<typename T>
    concept any_container = container<T> or host::cpts::container<T> or cuda::device::cpts::container<T>;

    template<typename T>
    concept any_mdspan = mdspan<T> or host::cpts::mdspan<T> or cuda::device::cpts::mdspan<T>;

    template<typename T>
    concept any_mdcontainer = mdcontainer<T> or host::cpts::mdcontainer<T> or cuda::device::cpts::mdcontainer<T>;

    template<typename T>
    concept any_view = any_span<T> or any_mdspan<T>;

    template<typename T>
    concept any_owning = any_container<T> or any_mdcontainer<T>;

} // namespace cpts

} // namespace emu
