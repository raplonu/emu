#ifndef EMU_SPAN_H
#define EMU_SPAN_H

#include <emu/type_traits.h>
#include <emu/utility.h>
#include <emu/byte.h>
#include <emu/misc/location.h>

#include <gsl/span>

namespace emu
{

namespace span
{
    template<
        typename ElementType, std::size_t Extent, typename Location,
        EnableIf<IsLocation<Location>> = true
    >
    constexpr auto create(gsl::span<ElementType, Extent> s, Location location);

namespace detail
{
    template<typename Rg>
    constexpr auto HasPointerIterator = IsPointer<RangeIteratorType<Rg>>;

    template<typename It>
    constexpr auto IsPointerIterator = IsSame<IteratorValue<It>*, It>;

} // namespace detail

    /**
     * @brief std::dynamic_extent is a constant of type std::size_t that is used to differentiate std::span of static and dynamic extent.
     *
     * Please refer to https://en.cppreference.com/w/cpp/container/span/dynamic_extent for more detail.
     */
    //TODO: replace gsl::dynamic_extent by std::dynamic_extent when c++20.
    using gsl::dynamic_extent;



namespace detail
{
    template<std::size_t From, std::size_t To>
    constexpr auto IsAllowedExtentConversion = gsl::details::is_allowed_extent_conversion<From, To>::value;

    template <typename From, typename To>
    constexpr auto IsAllowedElementTypeConversion = gsl::details::is_allowed_element_type_conversion<From, To>::value;

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

        using base_t::extent;

        using location_type              = Location;

        // constexpr span_t() noexcept = default;
        constexpr span_t(const span_t&) noexcept = default;
        constexpr span_t(span_t&&) noexcept = default;

        template< bool Dependent = false,
                // "Dependent" is needed to make "std::enable_if_t<Dependent || Extent == 0 || Extent
                // == dynamic_extent>" SFINAE, since "std::enable_if_t<Extent == 0 || Extent ==
                // dynamic_extent>" is ill-formed when Extent is greater than 0.
                EnableIf<(Dependent || IsAllowedExtentConversion<0, Extent>)> = true
        >
        constexpr span_t() noexcept:
            base_t(), location_()
        {}

        constexpr span_t(pointer ptr, size_type count, location_type location = {}) noexcept:
            base_t(ptr, count), location_(location)
        {}

        constexpr span_t(pointer firstElem, pointer lastElem, location_type location = {}) noexcept:
            base_t(firstElem, lastElem), location_(location)
        {}

        constexpr span_t(std::initializer_list<element_type> init_list, location_type location = {}) noexcept:
            base_t(init_list.begin(), init_list.end()), location_(location)
        {}

        template< std::size_t N, EnableIf<IsAllowedExtentConversion<N, Extent>> = true>
        constexpr span_t(element_type (&arr)[N], location_type location = {}) noexcept:
            base_t(arr), location_(location)
        {}

        template<
            class T, std::size_t N,
            EnableIf<IsAllowedExtentConversion<N, Extent> and IsAllowedElementTypeConversion<T, element_type>> = true
        >
        constexpr span_t(std::array<T, N>& arr, location_type location = {}) noexcept:
            base_t(arr), location_(location)
        {}

        template<
            class T, std::size_t N,
            EnableIf<IsAllowedExtentConversion<N, Extent> and IsAllowedElementTypeConversion<const T, element_type>> = true
        >
        constexpr span_t(const std::array<T, N>& arr, location_type location = {}) noexcept:
            base_t(arr), location_(location)
        {}

        // NB: the SFINAE here uses .data() as an incomplete/imperfect proxy for the requirement
        // on Container to be a contiguous sequence container.
        template <class Container,
                EnableIf<
                    !gsl::details::is_span<Container>::value && !gsl::details::is_std_array<Container>::value &&
                    std::is_pointer<decltype(std::declval<Container&>().data())>::value &&
                    std::is_convertible<
                        std::remove_pointer_t<decltype(std::declval<Container&>().data())> (*)[],
                        element_type (*)[]>::value
                > = true
        >
        constexpr span_t(Container& cont) noexcept:
            base_t(cont), location_()
        {}

        // NB: the SFINAE here uses .data() as an incomplete/imperfect proxy for the requirement
        // on Container to be a contiguous sequence container.
        template <class Container,
                EnableIf<
                    !gsl::details::is_span<Container>::value && !gsl::details::is_std_array<Container>::value &&
                    std::is_pointer<decltype(std::declval<Container&>().data())>::value &&
                    std::is_convertible<
                        std::remove_pointer_t<decltype(std::declval<Container&>().data())> (*)[],
                        element_type (*)[]>::value
                > = true
        >
        constexpr span_t(Container& cont, location_type location) noexcept:
            base_t(cont), location_(location)
        {}

        template <class Container,
                EnableIf<
                    std::is_const<element_type>::value && !gsl::details::is_span<Container>::value &&
                    !gsl::details::is_std_array<Container>::value &&
                    std::is_pointer<decltype(std::declval<const Container&>().data())>::value &&
                    std::is_convertible<std::remove_pointer_t<
                                            decltype(std::declval<const Container&>().data())> (*)[],
                                        element_type (*)[]>::value
                > = true
        >
        constexpr span_t(const Container& cont, location_type location = {}) noexcept:
            base_t(cont), location_(location)
        {}

        template <
            class OtherElementType, std::size_t OtherExtent,
            EnableIf<
                IsAllowedExtentConversion<OtherExtent, extent> &&
                IsAllowedElementTypeConversion<OtherElementType, element_type> &&
                std::is_same<location_type, location::host_t>::value
            > = true
        >
        constexpr span_t(const span_t<OtherElementType, location_type, OtherExtent>& other) noexcept:
            base_t(other), location_(other.location())
        {}

        template <
            class OtherElementType, std::size_t OtherExtent,
            EnableIf<
                IsAllowedExtentConversion<OtherExtent, extent> &&
                IsAllowedElementTypeConversion<OtherElementType, element_type>
            > = true
        >
        constexpr span_t(const gsl::span<OtherElementType, OtherExtent>& other, location_type location = {}) noexcept:
            base_t(other), location_(location)
        {}

        constexpr span_t& operator=(const span_t&) noexcept = default;
        constexpr span_t& operator=(span_t&&) noexcept = default;

        template <std::size_t Count>
        constexpr auto first() const noexcept {
            return as_span_t(base_t::template first<Count>());
        }

        template <std::size_t Count>
        constexpr auto last() const noexcept {
            return as_span_t(base_t::template last<Count>());
        }

        constexpr auto first(size_type count) const noexcept {
            return as_span_t(base_t::first(count));
        }

        constexpr auto last(size_type count) const noexcept {
            return as_span_t(base_t::last(count));
        }

        template< std::size_t Offset, std::size_t Count = dynamic_extent >
        constexpr auto subspan() const noexcept {
            return as_span_t(base_t::template subspan<Offset, Count>());
        }

        constexpr auto subspan( size_type offset, size_type count = dynamic_extent ) const noexcept {
            return as_span_t(base_t::subspan(offset, count));
        }

        location_type location() const noexcept {
            return location_;
        }

    private:
        location_type location_;

        template<typename OtherElementType, std::size_t OtherExtent>
        constexpr auto as_span_t(gsl::span<OtherElementType, OtherExtent> span) const noexcept {
            return span_t<OtherElementType, location_type, OtherExtent>(span, location());
        }
    };

} // namespace detail

} // namespace span

} // namespace emu

namespace gsl
{

namespace details
{

    template <class ElementType, typename Location, std::size_t Extent>
    struct is_span_oracle<emu::span::detail::span_t<ElementType, Location, Extent>> : std::true_type {};

} // namespace details

} // namespace gsl

namespace emu
{

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
    template<typename ElementType, std::size_t Extent = span::dynamic_extent>
    using span_t = span::detail::span_t<ElementType, location::host_t, Extent>;

namespace span
{

    template<
        std::size_t Extent = dynamic_extent, typename Rg,
        EnableIf<not IsPointer<Rg>> = true,
        EnableIf<not HasData<Rg>> = true,
        EnableIf<detail::HasPointerIterator<Rg>> = true
    >
    constexpr auto create(Rg && range) noexcept
    {
        using value_type = ForwardConst<Rg, RangeValue<Rg>>;
        using std::begin; using std::end;
        return span_t<value_type, Extent>(begin(range), end(range));
    }

    template<
        std::size_t Extent = dynamic_extent, typename Rg, typename Location,
        EnableIf<not IsPointer<Rg>> = true,
        EnableIf<not HasData<Rg>> = true,
        EnableIf<detail::HasPointerIterator<Rg> and IsLocation<Location>> = true
    >
    constexpr auto create(Rg && range, Location && location) noexcept
    {
        using value_type = ForwardConst<Rg, RangeValue<Rg>>;
        using std::begin; using std::end;
        return detail::span_t<value_type, Location, Extent>(begin(range), end(range), EMU_FWD(location));
    }

    template<typename T> struct TOo;

    template<
        std::size_t Extent = dynamic_extent, typename Container,
        EnableIf<not IsPointer<Container>> = true,
        EnableIf<HasData<Container>> = true,
        EnableIf<IsContainer<Container>> = true
    >
    constexpr auto create(Container && container) noexcept
    {
        using value_type = ForwardConst<Container, ContainerValue<Container>>;
        return span_t<value_type, Extent>(container.data(), container.size());
    }

    template<
        std::size_t Extent = dynamic_extent, typename Container, typename Location,
        EnableIf<not IsPointer<Container>> = true,
        EnableIf<HasData<Container>> = true,
        EnableIf<IsContainer<Container> and IsLocation<Location>> = true
    >
    constexpr auto create(Container && container, Location && location) noexcept
    {
        using value_type = ForwardConst<Container, ContainerValue<Container>>;
        return detail::span_t<value_type, Location, Extent>(container.data(), container.size(), EMU_FWD(location));
    }

    template<std::size_t Extent = dynamic_extent, typename T>
    constexpr auto create(T* begin, T* end) noexcept
    {
        return span_t<T, Extent>(begin, end);
    }

    template<
        std::size_t Extent = dynamic_extent, typename T, typename Location,
        EnableIf<IsLocation<Location>> = true
    >
    constexpr auto create(T* begin, T* end, Location && location) noexcept
    {
        return detail::span_t<T, Location, Extent>(begin, end, EMU_FWD(location));
    }

    template<std::size_t Extent = dynamic_extent, typename T>
    constexpr auto create(T* begin, std::size_t count) noexcept
    {
        return span_t<T, Extent>(begin, count);
    }

    template<
        std::size_t Extent = dynamic_extent, typename T, typename Location,
        EnableIf<IsLocation<Location>> = true
    >
    constexpr auto create(T* begin, std::size_t count, Location && location) noexcept
    {
        return detail::span_t<T, Location, Extent>(begin, count, EMU_FWD(location));
    }

namespace detail
{
    template <typename To, typename From, std::size_t Extent>
    struct calculate_byte_size : std::integral_constant<std::size_t, Extent * sizeof(From) / sizeof(To)>
    {
        static_assert(((Extent * sizeof(From)) % sizeof(To)) == 0        , "Size is incompatible.");
        static_assert(Extent < dynamic_extent / sizeof(From) * sizeof(To), "Size is too big."     );
    };

    template <typename To, typename From>
    struct calculate_byte_size<To, From, dynamic_extent>
        : std::integral_constant<std::size_t, dynamic_extent>
    {};
} // namespace detail

    template<typename To, typename From, typename Location, std::size_t Extent,
        // Enable when destination is const or source is not const.
        EnableIf<IsConst<To> || not IsConst<From> > = true
    >
    constexpr auto as(span::detail::span_t<From, Location, Extent> s) noexcept
    {
        using type = detail::span_t<To, Location, detail::calculate_byte_size<To, From, Extent>::value>;

        return type{reinterpret_cast<To*>(s.data()), s.size_bytes(), s.location()};
    }

    template<typename Span, typename... Args>
    EMU_HODE constexpr auto subspan(Span span, Args&&... args) noexcept {
        return span.subspan(EMU_FWD(args)...);
    }

} // namespace span

}

#endif //EMU_SPAN_H