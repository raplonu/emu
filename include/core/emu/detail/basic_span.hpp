#pragma once

#include <cstddef>
#include <emu/span.hpp>
#include <emu/concepts.hpp>
#include <emu/type_traits.hpp>
#include <emu/info.hpp>
#include <emu/utility.hpp>

#include <iterator>
#include <ranges>
#include <span>

namespace emu
{

    using std::dynamic_extent;

namespace detail
{

    template <typename ElementType, size_t Extent, typename LocationPolicy>
    struct basic_span : protected std::span<ElementType, Extent> {

        using base = std::span<ElementType, Extent>;

        using element_type =     typename base::element_type;
        using value_type =       typename base::value_type;
        using size_type =        typename base::size_type;
        using difference_type =  typename base::difference_type;
        using pointer =          typename base::pointer;
        using const_pointer =    typename base::const_pointer;
        using reference =        typename base::reference;
        using const_reference =  typename base::const_reference;
        using iterator =         typename base::iterator;
        using reverse_iterator = typename base::reverse_iterator;

        using location_type = LocationPolicy;

        using base::extent;

        template <typename Type>
        inline static constexpr bool validate_source = LocationPolicy::template validate_source<Type>;

        constexpr basic_span() noexcept = default;

        template <std::contiguous_iterator It>
        constexpr explicit(extent != dynamic_extent)
            basic_span(It first, size_t count) noexcept
            : base(first, count) {}

        template <std::contiguous_iterator It, std::sized_sentinel_for<It> End>
            requires(not std::is_convertible_v<End, size_t>)
        constexpr explicit(extent != dynamic_extent)
            basic_span(It first, End last) noexcept(noexcept(last - first))
            : base(first, last) {}

        template <size_t ArrayExtent>
            requires(extent == dynamic_extent or extent == ArrayExtent)
        constexpr basic_span(tid<ElementType> (&arr)[ArrayExtent]) noexcept : base(arr) {}

        template <typename U, size_t ArrayExtent>
            requires (extent == dynamic_extent or extent == ArrayExtent)
                 and (validate_source<std::array<U, ArrayExtent>>)
        constexpr basic_span(std::array<U, ArrayExtent> &arr) noexcept
            : base(arr) {}

        template <typename U, size_t ArrayExtent>
            requires (extent == dynamic_extent or extent == ArrayExtent)
                 and (validate_source<const std::array<U, ArrayExtent>>)
        constexpr basic_span(const std::array<U, ArrayExtent> &arr) noexcept
            : base(arr) {}

        template<std::ranges::contiguous_range Range>
        requires validate_source<Range>
        constexpr explicit(extent != dynamic_extent)
        basic_span(Range&& range)
            noexcept(noexcept(std::ranges::data(range))
		         and noexcept(std::ranges::size(range)))
            : base(EMU_FWD(range)) {}

        template<typename OT, size_t OExtent>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_span(const basic_span<OT, OExtent, LocationPolicy> &other) noexcept
            : base(static_cast<const base&>(other)) {}

        template<typename OT, size_t OExtent>
        requires (extent == dynamic_extent
               or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_span(const std::span<OT, OExtent> &other) noexcept
            : base(other) {}

        basic_span(const basic_span &) noexcept = default;
        basic_span(basic_span &&) noexcept = default;

        constexpr basic_span &operator=(const basic_span &) noexcept = default;
        constexpr basic_span &operator=(basic_span &&) noexcept = default;

        ~basic_span() noexcept = default;

        using base::begin;
        using base::end;
        using base::rbegin;
        using base::rend;

        template< size_t Count >
        constexpr auto first() const -> basic_span<ElementType, Count, LocationPolicy> {
            return base::template first<Count>();
        }

        constexpr auto first( size_type count ) const -> basic_span<ElementType, dynamic_extent, LocationPolicy> {
            return base::first(count);
        }

        template< size_t Count >
        constexpr auto last() const -> basic_span<ElementType, Count, LocationPolicy> {
            return base::template last<Count>();
        }

        constexpr auto last( size_type count ) const -> basic_span<ElementType, dynamic_extent, LocationPolicy> {
            return base::last(count);
        }

        template< size_t Offset, size_t Count = dynamic_extent >
        constexpr auto subspan() const -> basic_span<ElementType, Count, LocationPolicy> {
            return base::template subspan<Offset, Count>();
        }

        constexpr auto subspan( size_type offset, size_type count = dynamic_extent ) const
            -> basic_span<ElementType, dynamic_extent, LocationPolicy> {
            return base::subspan(offset, count);
        }

        using base::front;
        using base::back;
        using base::operator[];
        using base::data;

        using base::size;
        using base::size_bytes;
        using base::empty;

    };

    template<typename ElementType, size_t ArrayExtent, typename LocationPolicy>
    basic_span(ElementType(&)[ArrayExtent])
        -> basic_span<ElementType, ArrayExtent, LocationPolicy>;

    template<typename ElementType, size_t ArrayExtent, typename LocationPolicy>
    basic_span(std::array<ElementType, ArrayExtent>&)
        -> basic_span<ElementType, ArrayExtent, LocationPolicy>;

    template<typename ElementType, size_t ArrayExtent, typename LocationPolicy>
    basic_span(const std::array<ElementType, ArrayExtent>&)
      -> basic_span<const ElementType, ArrayExtent, LocationPolicy>;

    template<std::contiguous_iterator Iter, typename End, typename LocationPolicy>
    basic_span(Iter, End)
      -> basic_span<std::remove_reference_t<std::iter_reference_t<Iter>>, dynamic_extent, LocationPolicy>;

    template<std::ranges::contiguous_range Range, typename LocationPolicy>
    basic_span(Range &&)
      -> basic_span<std::remove_reference_t<std::ranges::range_reference_t<Range&>>, dynamic_extent, LocationPolicy>;

    template <typename ElementType, auto Extent>
    constexpr auto type_extent = Extent == dynamic_extent
                                    ? dynamic_extent
                                    : Extent / sizeof(ElementType);

    template <typename ElementType, auto Extent>
    constexpr auto byte_extent = Extent == dynamic_extent
                                    ? dynamic_extent
                                    : Extent * sizeof(ElementType);

    template<typename SpanType, typename NewElementType, size_t NewExtent>
    struct rebind_span_type;

    template<typename ElementType, size_t Extent, typename LocationPolicy, typename NewElementType, size_t NewExtent>
    struct rebind_span_type<basic_span<ElementType, Extent, LocationPolicy>, NewElementType, NewExtent> {
        using type = basic_span<
            propagate_const<NewElementType, ElementType>,
            NewExtent,
            LocationPolicy
        >;
    };

    template<typename ElementType, size_t Extent, typename NewElementType, size_t NewExtent>
    struct rebind_span_type<std::span<ElementType, Extent>, NewElementType, NewExtent> {
        using type = std::span<
            propagate_const<NewElementType, ElementType>,
            NewExtent
        >;
    };

    template<typename SpanType, typename NewElementType, size_t NewExtent>
    using rebind_span_type_t = typename rebind_span_type<SpanType, NewElementType, NewExtent>::type;

} // namespace detail

    // Use of reinterpret_cast is safe here as it is used to cast between byte and ElementType
    // Note that you can convert T1 to byte and then to T2 using as_t. Be careful with this.

    template <typename NewType, cpts::span SpanType>
        requires std::same_as<typename SpanType::element_type, byte>
    constexpr auto as_t(SpanType sp) noexcept
        -> detail::rebind_span_type_t<SpanType, NewType, detail::type_extent<NewType, SpanType::extent>>
    {
        auto data = reinterpret_cast<NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);

        return {data, size};
    }

    template<cpts::span SpanType>
    constexpr auto as_bytes(SpanType sp) noexcept
        -> detail::rebind_span_type_t<SpanType, const byte, detail::byte_extent<byte, SpanType::extent> >
    {
      auto data = reinterpret_cast<const byte*>(sp.data());
      auto size = sp.size_bytes();

      return {data, size};
    }

    template<cpts::mutable_span SpanType>
    constexpr auto as_writable_bytes(SpanType sp) noexcept
        -> detail::rebind_span_type_t<SpanType, byte, detail::byte_extent<byte, SpanType::extent> >
    {
      auto data = reinterpret_cast<byte*>(sp.data());
      auto size = sp.size_bytes();

      return {data, size};
    }

    template<cpts::span SpanType>
    auto as_auto_bytes(SpanType sp) noexcept
    {
        if constexpr (std::is_const_v<typename SpanType::element_type>)
            return as_bytes(sp);
        else
            return as_writable_bytes(sp);
    }

namespace spe
{

    template<cpts::span SpanType>
    struct info_t< SpanType >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "span< {}, ", type_name<ElementType>);
            it = detail::format_extent(it, Extent);
            return fmt::format_to(it, " >");
        }

        constexpr auto format_value(const SpanType &sp, fmt::format_context::iterator it) const {
            return fmt::format_to(it, "{} [ {} ]", fmt::ptr(sp.data()), sp.size());
        }
    };

} // namespace spe

} // namespace emu

// Opt-in to borrowed_range concept
template<typename ElementType, std::size_t Extent, typename LocationPolicy>
inline constexpr bool
std::ranges::enable_borrowed_range<emu::detail::basic_span<ElementType, Extent, LocationPolicy>> = true;

// Opt-in to view concept
template<typename ElementType, std::size_t Extent, typename LocationPolicy>
inline constexpr bool
std::ranges::enable_view<emu::detail::basic_span<ElementType, Extent, LocationPolicy>>
    = Extent == 0 || Extent == std::dynamic_extent;
