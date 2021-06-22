#ifndef EMU_SPAN_H
#define EMU_SPAN_H

#include <emu/type_traits.h>
#include <emu/byte.h>
#include <emu/misc/location.h>

#include <gsl/span>


namespace emu
{
    /**
     * @brief std::dynamic_extent is a constant of type std::size_t that is used to differentiate std::span of static and dynamic extent.
     *
     * Please refer to https://en.cppreference.com/w/cpp/container/span/dynamic_extent for more detail.
     */
    //TODO: replace gsl::dynamic_extent by std::dynamic_extent when c++20.
    using gsl::dynamic_extent;

    //TODO: replace gsl::as_bytes by std::as_bytes when c++20.
    using gsl::as_bytes;

    //TODO: replace gsl::as_writable_bytes by std::as_writable_bytes when c++20.
    using gsl::as_writable_bytes;

namespace detail
{

    template<typename ElementType, typename Location, std::size_t Extent = dynamic_extent>
    struct span_t : gsl::span<ElementType, Extent>
    {
        using base_t = gsl::span<ElementType, Extent>;

        using element_type               = typename base_t::element_type;
        using value_type                 = typename base_t::value_type;
        using size_type                  = typename base_t::size_type;
        using difference_type            = typename base_t::difference_type;
        using pointer                    = typename base_t::pointer;
        using const_pointer              = typename base_t::const_pointer;
        using reference                  = typename base_t::reference;
        using const_reference            = typename base_t::const_reference;
        using iterator                   = typename base_t::iterator;
        using reverse_iterator           = typename base_t::reverse_iterator;

        static constexpr size_type extent = base_t::Extent;

        using location_type              = Location;


        // [span.cons], span constructors, copy, assignment, and destructor
        template <bool Dependent = false,
                // "Dependent" is needed to make "std::enable_if_t<Dependent || Extent == 0 || Extent
                // == dynamic_extent>" SFINAE, since "std::enable_if_t<Extent == 0 || Extent ==
                // dynamic_extent>" is ill-formed when Extent is greater than 0.
                class = std::enable_if_t<(Dependent ||
                                            gsl::details::is_allowed_extent_conversion<0, Extent>::value)>>
        constexpr span_t() noexcept:
            base_t(), location()
        {}

        // [span.cons], span constructors, copy, assignment, and destructor
        template <bool Dependent = false,
                // "Dependent" is needed to make "std::enable_if_t<Dependent || Extent == 0 || Extent
                // == dynamic_extent>" SFINAE, since "std::enable_if_t<Extent == 0 || Extent ==
                // dynamic_extent>" is ill-formed when Extent is greater than 0.
                class = std::enable_if_t<(Dependent ||
                                            gsl::details::is_allowed_extent_conversion<0, Extent>::value)>>
        constexpr span_t(location_type location ) noexcept:
            base_t(), location(location)
        {}

        constexpr span_t(pointer ptr, size_type count) noexcept:
            base_t(ptr, count), location()
        {}

        constexpr span_t(pointer ptr, size_type count, location_type location) noexcept:
            base_t(ptr, count), location(location)
        {}

        constexpr span_t(pointer firstElem, pointer lastElem) noexcept:
            base_t(firstElem, lastElem), location()
        {}

        constexpr span_t(pointer firstElem, pointer lastElem, location_type location) noexcept:
            base_t(firstElem, lastElem), location(location)
        {}

        template <std::size_t N,
                std::enable_if_t<gsl::details::is_allowed_extent_conversion<N, Extent>::value, int> = 0>
        constexpr span_t(element_type (&arr)[N]) noexcept:
        base_t(arr), location()
        {}

        template <std::size_t N,
                std::enable_if_t<gsl::details::is_allowed_extent_conversion<N, Extent>::value, int> = 0>
        constexpr span_t(element_type (&arr)[N], location_type location) noexcept:
        base_t(arr), location(location)
        {}

        template <
            class T, std::size_t N,
            std::enable_if_t<(gsl::details::is_allowed_extent_conversion<N, Extent>::value &&
                            gsl::details::is_allowed_element_type_conversion<T, element_type>::value),
                            int> = 0>
        constexpr span_t(std::array<T, N>& arr, location_type location = {}) noexcept:
            base_t(arr), location(location)
        {}

        template <class T, std::size_t N,
                std::enable_if_t<
                    (gsl::details::is_allowed_extent_conversion<N, Extent>::value &&
                    gsl::details::is_allowed_element_type_conversion<const T, element_type>::value),
                    int> = 0>
        constexpr span_t(const std::array<T, N>& arr) noexcept:
            base_t(arr), location()
        {}

        template <class T, std::size_t N,
                std::enable_if_t<
                    (gsl::details::is_allowed_extent_conversion<N, Extent>::value &&
                    gsl::details::is_allowed_element_type_conversion<const T, element_type>::value),
                    int> = 0>
        constexpr span_t(const std::array<T, N>& arr, location_type location) noexcept:
            base_t(arr), location(location)
        {}

        // NB: the SFINAE here uses .data() as an incomplete/imperfect proxy for the requirement
        // on Container to be a contiguous sequence container.
        template <class Container,
                class = std::enable_if_t<
                    !gsl::details::is_span<Container>::value && !gsl::details::is_std_array<Container>::value &&
                    std::is_pointer<decltype(std::declval<Container&>().data())>::value &&
                    std::is_convertible<
                        std::remove_pointer_t<decltype(std::declval<Container&>().data())> (*)[],
                        element_type (*)[]>::value>>
        constexpr span_t(Container& cont) noexcept:
            base_t(cont), location()
        {}

        // NB: the SFINAE here uses .data() as an incomplete/imperfect proxy for the requirement
        // on Container to be a contiguous sequence container.
        template <class Container,
                class = std::enable_if_t<
                    !gsl::details::is_span<Container>::value && !gsl::details::is_std_array<Container>::value &&
                    std::is_pointer<decltype(std::declval<Container&>().data())>::value &&
                    std::is_convertible<
                        std::remove_pointer_t<decltype(std::declval<Container&>().data())> (*)[],
                        element_type (*)[]>::value>>
        constexpr span_t(Container& cont, location_type location) noexcept:
            base_t(cont), location(location)
        {}

        template <class Container,
                class = std::enable_if_t<
                    std::is_const<element_type>::value && !gsl::details::is_span<Container>::value &&
                    !gsl::details::is_std_array<Container>::value &&
                    std::is_pointer<decltype(std::declval<const Container&>().data())>::value &&
                    std::is_convertible<std::remove_pointer_t<
                                            decltype(std::declval<const Container&>().data())> (*)[],
                                        element_type (*)[]>::value>>
        constexpr span_t(const Container& cont) noexcept:
            base_t(cont), location()
        {}

        template <class Container,
                class = std::enable_if_t<
                    std::is_const<element_type>::value && !gsl::details::is_span<Container>::value &&
                    !gsl::details::is_std_array<Container>::value &&
                    std::is_pointer<decltype(std::declval<const Container&>().data())>::value &&
                    std::is_convertible<std::remove_pointer_t<
                                            decltype(std::declval<const Container&>().data())> (*)[],
                                        element_type (*)[]>::value>>
        constexpr span_t(const Container& cont, location_type location) noexcept:
            base_t(cont), location(location)
        {}

        constexpr span_t(const span_t& other) noexcept = default;

        template <
            class OtherElementType, std::size_t OtherExtent,
            class = std::enable_if_t<
                gsl::details::is_allowed_extent_conversion<OtherExtent, Extent>::value &&
                gsl::details::is_allowed_element_type_conversion<OtherElementType, element_type>::value>>
        constexpr span_t(const span_t<OtherElementType, location_type, OtherExtent>& other) noexcept:
            base_t(other), location(other.location)
        {}

        template <
            class OtherElementType, std::size_t OtherExtent,
            class = std::enable_if_t<
                gsl::details::is_allowed_extent_conversion<OtherExtent, Extent>::value &&
                gsl::details::is_allowed_element_type_conversion<OtherElementType, element_type>::value &&
                std::is_same<location_type, location::host_t>::value>>
        constexpr span_t(const gsl::span<OtherElementType, OtherExtent>& other) noexcept:
            base_t(other), location()
        {}

        location_type location;
    };
} // namespace detail


    /**
     * @brief The class template span describes an object that can refer to a contiguous sequence of objects
     * with the first element of the sequence at position zero.
     *
     * A span can either have a static extent, in which case the number of elements in the sequence is known
     * and encoded in the type, or a dynamic extent.
     * Please refer to https://en.cppreference.com/w/cpp/container/span for more detail.
     *
     * @note Use span from gsl library since it is the closest implementation of c++20 std::span.
     */
    //TODO: replace gsl::span by std::span when c++20.
    template<typename ElementType, std::size_t Extent = dynamic_extent>
    using span_t = detail::span_t<ElementType, location::host_t, Extent>;



namespace span
{
    template<std::size_t Extent = dynamic_extent, typename Rg>
    constexpr auto create(Rg && range) noexcept
    {
        using value_type = RangeValue<Rg>;
        using std::begin; using std::end;
        return span_t<value_type, Extent>(begin(range), end(range));
    }

    template<std::size_t Extent = dynamic_extent, typename Rg, typename Location>
    constexpr auto create(Rg && range, Location && location) noexcept
    {
        using value_type = RangeValue<Rg>;
        using std::begin; using std::end;
        return detail::span_t<value_type, Location, Extent>(begin(range), end(range), EMU_FWD(location));
    }

    template<std::size_t Extent = dynamic_extent, typename It>
    constexpr auto create(It begin, It end) noexcept
    {
        using value_type = IteratorValue<It>;
        return span_t<value_type, Extent>(begin, end);
    }

    template<std::size_t Extent = dynamic_extent, typename It, typename Location>
    constexpr auto create(It begin, It end, Location && location) noexcept
    {
        using value_type = IteratorValue<It>;
        return detail::span_t<value_type, Location, Extent>(begin, end, EMU_FWD(location));
    }

    template<std::size_t Extent = dynamic_extent, typename It>
    constexpr auto create(It begin, std::size_t count) noexcept
    {
        using value_type = IteratorValue<It>;
        return span_t<value_type, Extent>(begin, begin + count);
    }

    template<std::size_t Extent = dynamic_extent, typename It, typename Location>
    constexpr auto create(It begin, std::size_t count, Location && location) noexcept
    {
        using value_type = IteratorValue<It>;
        return detail::span_t<value_type, Location, Extent>(begin, begin + count, EMU_FWD(location));
    }


} // namespace span

}

#endif //EMU_SPAN_H