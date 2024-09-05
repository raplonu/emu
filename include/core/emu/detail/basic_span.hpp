#pragma once

#include <cstdint>
#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>
#include <emu/detail/basic_mdspan.hpp>

#include <emu/info.hpp>
#include <emu/utility.hpp>

#include <initializer_list>
#include <iterator>
#include <cstddef>
#include <span>
#include <ranges>

namespace emu
{

namespace detail
{

    template <typename ElementType, size_t Extent, typename LocationPolicy, typename ActualType>
    struct basic_span : std::span<ElementType, Extent> {

        using base = std::span<ElementType, Extent>;

        using element_type     = typename base::element_type;
        using value_type       = typename base::value_type;
        using size_type        = typename base::size_type;
        using difference_type  = typename base::difference_type;
        using pointer          = typename base::pointer;
        using const_pointer    = typename base::const_pointer;
        using reference        = typename base::reference;
        using const_reference  = typename base::const_reference;
        using iterator         = typename base::iterator;
        using reverse_iterator = typename base::reverse_iterator;

        using location_type = LocationPolicy;
        using actual_type = ActualType;

        static constexpr size_t extent = Extent;

        template <typename Type>
        inline static constexpr bool validate_source = location_type::template validate_source<Type>;

        constexpr basic_span() noexcept = default;

        template <std::contiguous_iterator It>
        constexpr explicit(extent != dynamic_extent)
            basic_span(It first, size_t count) noexcept
            : base(first, count)
        {}

        template <std::contiguous_iterator It, std::sized_sentinel_for<It> End>
            requires(not std::is_convertible_v<End, size_t>)
        constexpr explicit(extent != dynamic_extent)
            basic_span(It first, End last) EMU_NOEXCEPT_EXPR(base(first, last))
            : base(first, last)
        {}

        template <size_t ArrayExtent>
            requires(extent == dynamic_extent or extent == ArrayExtent)
        constexpr basic_span(tid<ElementType> (&arr)[ArrayExtent]) noexcept : base(arr) {}

        template <typename U, size_t ArrayExtent>
            requires (extent == dynamic_extent or extent == ArrayExtent)
                 and (validate_source<std::array<U, ArrayExtent>&>)
        constexpr basic_span(std::array<U, ArrayExtent>& arr) noexcept
            : base(arr.data(), arr.size())
        {}

        template <typename U, size_t ArrayExtent>
            requires (extent == dynamic_extent or extent == ArrayExtent)
                 and (validate_source<const std::array<U, ArrayExtent>&>)
                 and (is_const<element_type>)
        constexpr basic_span(const std::array<U, ArrayExtent>& arr) noexcept
            : base(arr.data(), arr.size())
        {}

        template<cpts::contiguous_sized_range Range>
            requires validate_source<Range>
                 and (std::ranges::borrowed_range<Range> or is_const<element_type>)
        constexpr explicit(extent != dynamic_extent)
        basic_span(Range&& range)
            EMU_NOEXCEPT_EXPR(base(EMU_FWD(range)))
            : base(EMU_FWD(range))
        {}

        explicit(extent != std::dynamic_extent)
        constexpr basic_span( std::initializer_list<value_type> il ) noexcept
            requires validate_source<std::initializer_list<value_type>>
                 and std::is_const_v<element_type>
            : base(il.begin(), il.size())
        {}

        template<typename OT, size_t OExtent, typename OActualType>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_span(const basic_span<OT, OExtent, LocationPolicy, OActualType> &other) noexcept
            : base(static_cast<const base&>(other)) {}

        template<typename OT, size_t OExtent>
        requires (extent == dynamic_extent
               or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_span(const std::span<OT, OExtent> &other) noexcept
            : base(other) {}

        /**
         * @brief Special constructor that allows implicit conversion from std::span to emu::span.
         *
         */
        constexpr basic_span(std::span<ElementType, Extent> sp) noexcept requires (extent != dynamic_extent)
            : base(sp)
        {}

        basic_span(const basic_span &) noexcept = default;
        basic_span(basic_span &&) noexcept = default;

        constexpr basic_span &operator=(const basic_span &) noexcept = default;
        constexpr basic_span &operator=(basic_span &&) noexcept = default;

        ~basic_span() noexcept = default;

        // using base::begin;
        // using base::end;
        // using base::rbegin;
        // using base::rend;

        template< size_t Count >
        constexpr auto first() const {
            return from_span(base::template first<Count>());
        }

        constexpr auto first( size_type count ) const {
            return from_span(base::first(count));
        }

        template< size_t Count >
        constexpr auto last() const {
            return from_span(base::template last<Count>());
        }

        constexpr auto last( size_type count ) const {
            return from_span(base::last(count));
        }

        template< size_t Offset, size_t Count = dynamic_extent >
        constexpr auto subspan() const {
            return from_span(base::template subspan<Offset, Count>());
        }

        constexpr auto subspan( size_type offset, size_type count = dynamic_extent ) const {
            return from_span(base::subspan(offset, count));
        }

        // using base::front;
        // using base::back;
        // using base::operator[];
        // using base::data;

        // using base::size;
        // using base::size_bytes;
        // using base::empty;

        template<typename OT, size_t OExtent>
        constexpr auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return self().from_span(sp);
        }

        const actual_type& self() const noexcept {
            return static_cast<const actual_type&>(*this);
        }

    };

} // namespace detail

    template<cpts::span Span>
    constexpr auto c_contigous(const Span&) {
        return true;
    }

    template<cpts::span Span>
    constexpr auto f_contigous(const Span&) {
        return true;
    }

    // Use of reinterpret_cast is safe here as it is used to cast between byte and ElementType
    // Note that you can convert T1 to byte and then to T2 using as_t. Be careful with this.
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

    template <typename NewType, cpts::span SpanType>
        requires std::same_as<typename SpanType::element_type, byte>
    constexpr auto as_t(SpanType sp) noexcept
    {
        auto* data = reinterpret_cast<NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);

        auto res = std::span(data, size);

        if constexpr ( cpts::std_span<SpanType> )
            return res;
        else
            return sp.from_span( res );
    }

    template<cpts::span SpanType>
    constexpr auto as_bytes(SpanType sp) noexcept
    {
        auto res = std::as_bytes(sp);

        if constexpr ( cpts::std_span<SpanType> )
            return res;
        else
            return sp.from_span( res );
    }

    template<cpts::mutable_span SpanType>
    constexpr auto as_writable_bytes(SpanType sp) noexcept
    {
        auto res = std::as_writable_bytes(sp);

        if constexpr ( cpts::std_span<SpanType> )
            return res;
        else
            return sp.from_span( res );
    }
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

    template<cpts::span SpanType>
    auto as_auto_bytes(SpanType sp) noexcept
    {
        if constexpr ( cpts::const_span<SpanType> )
            return as_bytes(sp);
        else
            return as_writable_bytes(sp);
    }

namespace spe
{

    // template<cpts::span Span>
    // inline constexpr bool enable_relocatable_range<Span> = true;

    template<cpts::span SpanType>
    struct info_t< SpanType >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "span< {}, ", type_name<typename SpanType::element_type>);
            it = detail::format_extent(it, SpanType::extent);
            return fmt::format_to(it, " >");
        }

        constexpr auto format_value(const SpanType &sp, fmt::format_context::iterator it) const {
            return fmt::format_to(it, "{} [ {} ]", fmt::ptr(sp.data()), sp.size());
        }
    };

} // namespace spe

} // namespace emu

// Opt-in to borrowed_range concept
template<emu::cpts::emu_span EmuSpan>
inline constexpr bool
std::ranges::enable_borrowed_range<EmuSpan> = true;

// Opt-in to view concept
template<emu::cpts::emu_span EmuSpan>
inline constexpr bool
std::ranges::enable_view<EmuSpan>
    = EmuSpan::extent == 0 || EmuSpan::extent == std::dynamic_extent;

template<emu::cpts::span Span>
struct std::hash<Span> {
    std::size_t operator()(const Span& sp) const noexcept {
        return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(sp.data())) ^ std::hash<size_t>{}(sp.size());
    }
};

#define EMU_DEFINE_SPAN_DEDUCTION_GUIDES                                  \
    template< class It, class EndOrSize >                                 \
    span( It, EndOrSize ) -> span<iterator_cv_value<It>, dynamic_extent>; \
                                                                          \
    template< class T, size_t N >                                         \
    span( T (&)[N] ) -> span<T, N>;                                       \
                                                                          \
    template< typename Range >                                            \
    span( Range&& ) -> span< range_cv_value<Range>, dynamic_extent>;      \
                                                                          \
    template< class T, size_t N >                                         \
    span( std::array<T, N>& ) -> span<T, N>;                              \
                                                                          \
    template< class T, size_t N >                                         \
    span( const std::array<T, N>& ) -> span< const T, N>;                 \
                                                                          \
    template< class T, size_t N >                                         \
    span( std::span<T, N>& ) -> span<T, N>;                               \
                                                                          \
    template< class T, size_t N >                                         \
    span( std::span<const T, N>& ) -> span< const T, N>;                  \
                                                                          \
    template< typename T >                                                \
    span( std::initializer_list<T> ) -> span< const T, dynamic_extent>;
