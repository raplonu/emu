#pragma once

#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>

#include <emu/detail/basic_span.hpp>
#include <emu/capsule.hpp>

#include <initializer_list>
#include <ranges>
#include <vector>

namespace emu::detail
{

    template <typename ElementType, size_t Extent, typename LocationPolicy, typename ActualType>
    struct basic_container : basic_span<ElementType, Extent, LocationPolicy, ActualType>, emu::capsule
    {
        using span_type = basic_span<ElementType, Extent, LocationPolicy, ActualType>;
        using capsule_base = emu::capsule;

        using element_type     = typename span_type::element_type;
        using value_type       = typename span_type::value_type;
        using size_type        = typename span_type::size_type;
        using difference_type  = typename span_type::difference_type;
        using pointer          = typename span_type::pointer;
        using const_pointer    = typename span_type::const_pointer;
        using reference        = typename span_type::reference;
        using const_reference  = typename span_type::const_reference;
        using iterator         = typename span_type::iterator;
        using reverse_iterator = typename span_type::reverse_iterator;

        using location_type   = typename span_type::location_type;
        using actual_type     = typename span_type::actual_type;

        static constexpr size_t extent = Extent;

        template <typename Type>
        inline static constexpr bool validate_source = location_type::template validate_source<Type>;

    public:
        using span_type::span_type;

        template <std::contiguous_iterator It, typename DataHolder>
            explicit(extent != dynamic_extent)
        constexpr basic_container(It first, size_t count, DataHolder&& dh)
            : span_type(first, count)
            , capsule_base(EMU_FWD(dh))
        {}

        template <std::contiguous_iterator It, std::sized_sentinel_for<It> End, typename DataHolder>
            requires(not std::is_convertible_v<End, size_t>)
            explicit(extent != dynamic_extent)
        constexpr basic_container(It first, End last, DataHolder&& dh)
            : span_type(first, last)
            , capsule_base(EMU_FWD(dh))
        {}

        template<cpts::contiguous_sized_range Range>
            requires (cpts::not_derived_from<rm_cvref<Range>, basic_container>)
                 and (validate_source<Range>)
                 and (std::ranges::borrowed_range<Range> or is_const<element_type> or cpts::relocatable_owning_range<Range>)
            explicit(extent != dynamic_extent)
        constexpr basic_container(Range&& range)
            noexcept( noexcept(span_type(range))
                  and noexcept(capsule_from_range(EMU_FWD(range))))
            : span_type(range)
            , capsule_base(capsule_from_range(EMU_FWD(range)))
        {}

        template<cpts::contiguous_sized_range Range, typename DataHolder>
            requires (validate_source<Range>)
            explicit(extent != dynamic_extent)
        constexpr basic_container(Range&& range, DataHolder&& dh)
            : span_type(EMU_FWD(range))
            , capsule_base(EMU_FWD(dh))
        {}

        // explicit(extent != std::dynamic_extent)
        // constexpr basic_container( std::initializer_list<value_type> il ) noexcept
        //     requires validate_source<std::initializer_list<value_type>>
        //          and std::is_const_v<element_type>
        //     : span_type(il.begin(), il.end())
        // {}

        template<typename OT, size_t OExtent, typename OActualType, typename DataHolder>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_container(const basic_container<OT, OExtent, LocationPolicy, OActualType>& other) noexcept
            : span_type(static_cast<const span_type&>(other))
            , capsule_base(other.capsule())
        {}

        template<typename OT, size_t OExtent, typename OActualType, typename DataHolder>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_container(const basic_span<OT, OExtent, LocationPolicy, OActualType>& other, DataHolder&& dh) noexcept
            : span_type(static_cast<const span_type&>(other))
            , capsule_base(EMU_FWD(dh))
        {}

        template<typename DataHolder>
        constexpr explicit
        basic_container(span_type s, DataHolder&& dh)
            : span_type(s)
            , capsule_base(EMU_FWD(dh))
        {}

        constexpr basic_container(const basic_container&) noexcept = default;
        constexpr basic_container(basic_container&&) noexcept = default;

        constexpr basic_container& operator=(const basic_container&) noexcept = default;
        constexpr basic_container& operator=(basic_container&&) noexcept = default;

        ~basic_container() noexcept = default;

        emu::capsule&        capsule() &      noexcept { return static_cast<emu::capsule&>(*this); }
        emu::capsule const & capsule() const& noexcept { return static_cast<const emu::capsule&>(*this); }
        emu::capsule&&         capsule() &&     noexcept { return static_cast<emu::capsule&&>(*this); }

        // auto use_count() const noexcept { return capsule_.use_count(); }

        // template< std::size_t Count >
        // constexpr basic_container<element_type, Count, location_type> first() const noexcept {
        //     return from_span(base::template first<Count>());
        // }

        // constexpr basic_container<element_type, std::dynamic_extent, location_type> first( size_type count ) const noexcept {
        //     return from_span(base::first(count));
        // }

        // template< std::size_t Count >
        // constexpr basic_container<element_type, Count, location_type> last() const noexcept {
        //     return from_span(base::template last<Count>());
        // }

        // constexpr basic_container<element_type, std::dynamic_extent, location_type> last( size_type count ) const noexcept {
        //     return from_span(base::last(count));
        // }

        template< std::size_t Offset,
              std::size_t Count = std::dynamic_extent >
        constexpr auto subcontainer() const noexcept {
            return actual_type::template subspan<Offset, Count>();
        }

        constexpr auto subcontainer( size_type offset,
                 size_type count = std::dynamic_extent ) const noexcept {
            return actual_type::subspan(offset, count);
        }

        std::pair<span_type, emu::capsule> as_pair() const & noexcept  {
            return {span_type(*this), capsule()};
        }

        std::pair<span_type, emu::capsule> as_pair() && noexcept {
            return {span_type(*this), std::move(capsule())};
        }

        // template< size_t Offset, size_t Count = dynamic_extent >
        // constexpr auto subspan() const -> basic_container = delete;

        // constexpr auto subspan( size_type offset, size_type count = dynamic_extent ) const -> basic_container = delete;

        // using base::begin;
        // using base::end;
        // using base::rbegin;
        // using base::rend;

        // using base::front;
        // using base::back;
        // using base::operator[];
        // using base::data;

        // using base::size;
        // using base::size_bytes;
        // using base::empty;

    };

} // namespace emu::detail

// Opt-in to borrowed_range concept
//
// emu::container may own the data it points to. If it does, it will be destroyed when the last
// container pointing to it is destroyed.
// we cannot safely borrow the container.
template<emu::cpts::container Container>
inline constexpr bool
std::ranges::enable_borrowed_range<Container> = false;

// Opt-in to view concept
template<emu::cpts::container Container>
inline constexpr bool
std::ranges::enable_view<Container>
    = Container::extent == 0 || Container::extent == std::dynamic_extent;

#define EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES                                                       \
    template< class It, class EndOrSize >                                                           \
    container( It, EndOrSize )               -> container< iterator_cv_value<It>, dynamic_extent >; \
    template< class It, class EndOrSize, class DataHolder >                                         \
    container( It, EndOrSize, DataHolder&& ) -> container< iterator_cv_value<It>, dynamic_extent >; \
                                                                                                    \
    template< class T, size_t N >                                                                   \
    container( T (&)[N] )               -> container<T, N>;                                         \
    template< class T, size_t N, class DataHolder >                                                 \
    container( T (&)[N], DataHolder&& ) -> container<T, N>;                                         \
                                                                                                    \
    template< typename Range >                                                                      \
    container( Range&& )               -> container< range_cv_value<Range>, dynamic_extent>;        \
    template< typename Range, class DataHolder >                                                    \
    container( Range&&, DataHolder&& ) -> container< range_cv_value<Range>, dynamic_extent>;        \
                                                                                                    \
    template< class T, size_t N >                                                                   \
    container( std::array<T, N>& )       -> container<T, N>;                                        \
    template< class T, size_t N >                                                                   \
    container( const std::array<T, N>& ) -> container< const T, N>;                                 \
                                                                                                    \
    template< class T, size_t N >                                                                   \
    container( std::span<T, N> )       -> container<T, N>;                                          \
    template< class T, size_t N, class DataHolder >                                                 \
    container( std::span<T, N>, DataHolder&& )       -> container<T, N>;                            \
                                                                                                    \
    template< class T, size_t N >                                                                   \
    container( std::span<const T, N> ) -> container< const T, N>;                                   \
    template< class T, size_t N, class DataHolder >                                                 \
    container( std::span<const T, N>, DataHolder&& ) -> container< const T, N>;                     \
                                                                                                    \
    template< typename T >                                                                          \
    container( std::initializer_list<T> ) -> container< T, dynamic_extent>;
