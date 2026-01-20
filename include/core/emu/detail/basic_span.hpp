#pragma once

#include <cstdint>
#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>
#include <emu/detail/mdspan_types.hpp>
#include <emu/tensor_traits.hpp>

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

    template <typename ElementType, size_t Extent, typename AccessorPolicy>
    struct basic_span;

} // namespace detail

namespace cpts
{

    template <typename T>
    concept emu_span = same_as<T, emu::detail::basic_span<
                        typename T::element_type, T::extent,
                        typename T::accessor_type>
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

    template <typename ElementType, size_t Extent, typename AccessorPolicy>
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


        template<typename OT, size_t OExtent, typename OAccessor>
        requires (extent == std::dynamic_extent
               or OExtent == std::dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != std::dynamic_extent && OExtent != std::dynamic_extent)
        basic_span(const basic_span<OT, OExtent, OAccessor> &other) noexcept
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

        // using base::begin;
        // using base::end;
        // using base::rbegin;
        // using base::rend;

        template< size_t Count >
        constexpr auto first() const {
            return actual_from_base(base::template first<Count>());
        }

        constexpr auto first( size_type count ) const {
            return actual_from_base(base::first(count));
        }

        template< size_t Count >
        constexpr auto last() const {
            return actual_from_base(base::template last<Count>());
        }

        constexpr auto last( size_type count ) const {
            return actual_from_base(base::last(count));
        }

        template< size_t Offset, size_t Count = std::dynamic_extent >
        constexpr auto subspan() const {
            return actual_from_base(base::template subspan<Offset, Count>());
        }

        constexpr auto subspan( size_type offset, size_type count = std::dynamic_extent ) const {
            return actual_from_base(base::subspan(offset, count));
        }

        // using base::front;
        // using base::back;
        // using base::operator[];
        // using base::data;

        constexpr reference operator[](size_type idx) const noexcept
        {
            return base::operator[](idx);
        }

        // using base::size;
        // using base::size_bytes;
        // using base::empty;

        /**
         * @brief Returns an ActualType constructed from a std::span.
         *
         * Default implementation that constructs ActualType from data pointer and size.
         * Can be specialized in derived classes for custom behavior.
         *
         * @tparam OT
         * @tparam OExtent
         * @param sp
         * @return constexpr auto
         */
        template<typename OT, size_t OExtent>
        constexpr auto actual_from_base(std::span<OT, OExtent> sp) const noexcept {
            return basic_span(sp.data(), sp.size());
        }

    };

    //Note: the following deduction guides do not take into account the AccessorPolicy.
    //It must be provided by alias partial specialization. Then `void` will be replaced by the actual accessor policy.

    template< class It, class EndOrSize >
    basic_span( It, EndOrSize ) -> basic_span<iterator_cv_value<It>, dynamic_extent, void>;

    template< class T, size_t N >
    basic_span( T (&)[N] ) -> basic_span<T, N, void>;

    template< typename Range >
    basic_span( Range&& ) -> basic_span< range_cv_value<Range>, dynamic_extent, void>;

    template< class T, size_t N >
    basic_span( std::array<T, N>& ) -> basic_span<T, N, void>;

    template< class T, size_t N >
    basic_span( const std::array<T, N>& ) -> basic_span< const T, N, void>;

    template< class T, size_t N >
    basic_span( std::span<T, N>& ) -> basic_span<T, N, void>;

    template< class T, size_t N >
    basic_span( std::span<const T, N>& ) -> basic_span< const T, N, void>;

    template< typename T >
    basic_span( std::initializer_list<T> ) -> basic_span< const T, dynamic_extent, void>;

} // namespace detail

    // template<cpts::span Span>
    // constexpr auto c_contigous(const Span&) {
    //     return true;
    // }

    // template<cpts::span Span>
    // constexpr auto f_contigous(const Span&) {
    //     return true;
    // }

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

    template <typename ElementType, size_t Extent, typename AccessorPolicy>
    struct tensor_traits<emu::detail::basic_span<ElementType, Extent, AccessorPolicy>>
    {
        static constexpr std::size_t rank = 1;

        using extents_type = dims<rank>;
        using layout_type = layout_right;

        using mapping_type = typename layout_type::mapping<extents_type>;

        using element_type = ElementType;
        using value_type = rm_cvref<element_type>;

        using index_type = std::size_t;
        using size_type = std::size_t;
        using rank_type = std::size_t;

        using accessor_type = AccessorPolicy;

        using data_handle_type = typename accessor_type::data_handle_type;
        using reference = typename accessor_type::reference;

        static constexpr data_handle_type data_handle(emu::detail::basic_span<ElementType, Extent, AccessorPolicy>& span) noexcept {
            return span.data();
        }

        static constexpr mapping_type mapping(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>& span) noexcept {
            return mapping_type(extents_type{span.size()});
        }

        static constexpr accessor_type accessor(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&) noexcept {
            return accessor_type();
        }

        static constexpr size_type size(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>& span) noexcept {
            return span.size();
        }

        static constexpr bool is_empty(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>& span) noexcept {
            return span.empty();
        }

        static constexpr index_type extent(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>& span, rank_type) noexcept {
            return span.size();
        }

        static constexpr index_type stride(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&, rank_type) noexcept {
            return 1;
        }

        static constexpr bool is_unique(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&) { return true; }
        static constexpr bool is_exhaustive(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&) { return true; }
        static constexpr bool is_strided(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&) { return true; }
        static constexpr bool is_always_unique(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&) { return true; }
        static constexpr bool is_always_exhaustive(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&) { return true; }
        static constexpr bool is_always_strided(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>&) { return true; }
        static constexpr std::size_t required_span_size(const emu::detail::basic_span<ElementType, Extent, AccessorPolicy>& span) {
            return span.size();
        }
    };

namespace spe
{

    // template<cpts::span Span>
    // inline constexpr bool enable_relocatable_range<Span> = true;

    template<cpts::std_span SpanType>
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

    template<cpts::emu_span SpanType>
    struct info_t< SpanType >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            //TODO: Adds info about accessor.
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

//TODO: why I needed that ?
// I think it was to compare span without looking at the content.
// template<emu::cpts::span Span>
// struct std::hash<Span> {
//     std::size_t operator()(const Span& sp) const noexcept {
//         return std::hash<uintptr_t>{}(reinterpret_cast<uintptr_t>(sp.data())) ^ std::hash<size_t>{}(sp.size());
//     }
// };
