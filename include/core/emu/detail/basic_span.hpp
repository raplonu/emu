#pragma once

#include <cstdint>
#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>
#include <emu/detail/mdspan_types.hpp>
#include <emu/tensor_traits.hpp>

#include <emu/info.hpp>
#include <emu/capsule.hpp>
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

    template <typename ElementType, size_t Extent, typename AccessorPolicy, typename Derived>
    struct basic_span;

} // namespace detail

namespace cpts
{

    template <typename T>
    concept emu_span
        = std::derived_from<T,
            emu::detail::basic_span<
                typename T::element_type,
                T::extent,
                typename T::accessor_type,
                typename T::derived_type
            >
        >;

    template <typename T>
    concept std_span = same_as<T, std::span<typename T::element_type, T::extent>>;

    template <typename T>
    concept span = std_span<T>
                or emu_span<T>;

    template <typename T>
    concept const_span = span<T> and is_const<typename T::element_type>;

    template <typename T>
    concept mutable_span = span<T> and (not is_const<typename T::element_type>);

} // namespace cpts

namespace detail
{

    template <typename ElementType, size_t Extent, typename AccessorPolicy, typename Derived>
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

        using accessor_type = AccessorPolicy;
        using derived_type = Derived;

        static constexpr size_t extent = Extent;

        // TODO: use accessor_type internally: in constructors and operator[].
        // like mdspan, don't bother to add too many constructors. Just for one.
        // [[no_unique_address]] accessor_type accessor{};

        constexpr basic_span() noexcept = default;

        template <std::contiguous_iterator It>
        constexpr explicit(extent != std::dynamic_extent)
            basic_span(It first, size_t count) noexcept
            : base(first, count)
            // , accessor{}
        {}

        template <std::contiguous_iterator It, std::sized_sentinel_for<It> End>
            requires(not std::is_convertible_v<End, size_t>)
        constexpr explicit(extent != std::dynamic_extent)
            basic_span(It first, End last) EMU_NOEXCEPT_EXPR(base(first, last))
            : base(first, last)
        {}

        template <size_t ArrayExtent>
            requires(extent == std::dynamic_extent or extent == ArrayExtent)
        constexpr basic_span(tid<ElementType> (&arr)[ArrayExtent]) noexcept : base(arr) {}

        template <typename U, size_t ArrayExtent>
            requires (extent == std::dynamic_extent or extent == ArrayExtent)
        constexpr basic_span(std::array<U, ArrayExtent>& arr) noexcept
            : base(arr.data(), arr.size())
        {}

        template <typename U, size_t ArrayExtent>
            requires (extent == std::dynamic_extent or extent == ArrayExtent)
                 and (is_const<element_type>)
        constexpr basic_span(const std::array<U, ArrayExtent>& arr) noexcept
            : base(arr.data(), arr.size())
        {}

        template<cpts::contiguous_sized_range Range>
            requires (std::ranges::borrowed_range<Range> or is_const<element_type>)
        constexpr explicit(extent != std::dynamic_extent)
        basic_span(Range&& range)
            EMU_NOEXCEPT_EXPR(base(EMU_FWD(range)))
            : base(EMU_FWD(range))
        {}

        explicit(extent != std::dynamic_extent)
        constexpr basic_span( std::initializer_list<value_type> il ) noexcept
            requires std::is_const_v<element_type>
            : base(il.begin(), il.size())
        {}

        template<typename OT, size_t OExtent, typename OAccessor, typename ODerived>
        requires (extent == std::dynamic_extent
               or OExtent == std::dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != std::dynamic_extent && OExtent != std::dynamic_extent)
        basic_span(const basic_span<OT, OExtent, OAccessor, ODerived> &other) noexcept
            : base(static_cast<const base&>(other)) {}

        template<typename OT, size_t OExtent>
        requires (extent == std::dynamic_extent
               or OExtent == std::dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != std::dynamic_extent && OExtent != std::dynamic_extent)
        basic_span(const std::span<OT, OExtent> &other) noexcept
            : base(other) {}

        /**
         * @brief Special constructor that allows implicit conversion from std::span to emu::span.
         *
         */
        constexpr basic_span(std::span<ElementType, Extent> sp) noexcept requires (extent != std::dynamic_extent)
            : base(sp)
        {}

        basic_span(const basic_span &) noexcept = default;
        basic_span(basic_span &&) noexcept = default;

        constexpr basic_span &operator=(const basic_span &) noexcept = default;
        constexpr basic_span &operator=(basic_span &&) noexcept = default;

        ~basic_span() noexcept = default;

        template< size_t Count >
        constexpr auto first() const {
            return self().actual_from_base(base::template first<Count>());
        }

        constexpr auto first( size_type count ) const {
            return self().actual_from_base(base::first(count));
        }

        template< size_t Count >
        constexpr auto last() const {
            return self().actual_from_base(base::template last<Count>());
        }

        constexpr auto last( size_type count ) const {
            return self().actual_from_base(base::last(count));
        }

        template< size_t Offset, size_t Count = std::dynamic_extent >
        constexpr auto subspan() const {
            return self().actual_from_base(base::template subspan<Offset, Count>());
        }

        constexpr auto subspan( size_type offset, size_type count = std::dynamic_extent ) const {
            return self().actual_from_base(base::subspan(offset, count));
        }

        // constexpr reference operator[](size_type idx) const noexcept
        // {
        //     return base::operator[](idx);
        // }

        const derived_type& self() const noexcept {
            return static_cast<const derived_type&>(*this);
        }

    };

} // namespace detail

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
            return sp.actual_from_base( res );
    }

    template<cpts::span SpanType>
    constexpr auto as_bytes(SpanType sp) noexcept
    {
        auto res = std::as_bytes(sp);

        if constexpr ( cpts::std_span<SpanType> )
            return res;
        else
            return sp.actual_from_base( res );
    }

    template<cpts::mutable_span SpanType>
    constexpr auto as_writable_bytes(SpanType sp) noexcept
    {
        auto res = std::as_writable_bytes(sp);

        if constexpr ( cpts::std_span<SpanType> )
            return res;
        else
            return sp.actual_from_base( res );
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

    template<cpts::std_span SpanType>
    struct info_t< SpanType >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "span< {}, ", type_name<typename SpanType::element_type>);
            it = detail::format_extent(it, SpanType::extent);
            return fmt::format_to(it, " >");
        }

        constexpr auto format_value(const SpanType &sp, fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "{} [ {} ]", fmt::ptr(sp.data()), sp.size());
            if constexpr (emu::cpts::capsule_owner<SpanType>) {
                it = format_value_info(sp.capsule(), it);
            }
            return it;
        }
    };

    template<cpts::emu_span SpanType>
    struct info_t< SpanType >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "span< {}, ", type_name<typename SpanType::element_type>);
            it = detail::format_extent(it, SpanType::extent);
            return fmt::format_to(it, ", {} >", type_name<typename SpanType::accessor_type>);
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

//TODO: why I needed that ?
// I think it was to compare span without looking at the content.
// template<emu::cpts::span Span>
// struct std::hash<Span> {
//     std::size_t operator()(const Span& sp) const noexcept {
//         return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(sp.data())) ^ std::hash<size_t>{}(sp.size());
//     }
// };

#define EMU_DEFINE_SPAN_DEDUCTION_GUIDES(span_type)                              \
template< class It, class EndOrSize >                                            \
span_type( It, EndOrSize ) -> span_type<iterator_cv_value<It>, dynamic_extent>;  \
                                                                                 \
template< class T, size_t N >                                                    \
span_type( T (&)[N] ) -> span_type<T, N>;                                        \
                                                                                 \
template< typename Range >                                                       \
span_type( Range&& ) -> span_type< range_cv_value<Range>, dynamic_extent>;       \
                                                                                 \
template< class T, size_t N >                                                    \
span_type( std::array<T, N>& ) -> span_type<T, N>;                               \
                                                                                 \
template< class T, size_t N >                                                    \
span_type( const std::array<T, N>& ) -> span_type< const T, N>;                  \
                                                                                 \
template< class T, size_t N >                                                    \
span_type( std::span<T, N>& ) -> span_type<T, N>;                                \
                                                                                 \
template< class T, size_t N >                                                    \
span_type( std::span<const T, N>& ) -> span_type< const T, N>;                   \
                                                                                 \
template< typename T >                                                           \
span_type( std::initializer_list<T> ) -> span_type< const T, dynamic_extent>
