#pragma once

#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>
#include <emu/detail/basic_span.hpp>
#include <emu/capsule.hpp>

#include <ranges>

namespace emu
{

namespace detail
{

    template <typename ElementType, size_t Extent, typename LocationPolicy, typename ActualType>
    struct basic_container : basic_span<ElementType, Extent, LocationPolicy, ActualType>, capsule
    {
        using base = basic_span<ElementType, Extent, LocationPolicy, ActualType>;

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

        using location_type   = typename base::location_type;
        using actual_type     = typename base::actual_type;

        static constexpr size_t extent = Extent;

        template <typename Type>
        inline static constexpr bool validate_source = location_type::template validate_source<Type>;

    public:
        using base::base;

        template <std::contiguous_iterator It, typename DataHolder>
            explicit(extent != dynamic_extent)
        constexpr basic_container(It first, size_t count, DataHolder&& dh)
            : base(first, count)
            , capsule(EMU_FWD(dh))
        {}

        template <std::contiguous_iterator It, std::sized_sentinel_for<It> End, typename DataHolder>
            requires(not std::is_convertible_v<End, size_t>)
            explicit(extent != dynamic_extent)
        constexpr basic_container(It first, End last, DataHolder&& dh)
            : base(first, last)
            , capsule(EMU_FWD(dh))
        {}

        template<cpts::contiguous_sized_range Range>
            requires (validate_source<Range>)
                //  and (std::ranges::borrowed_range<Range> or is_const<element_type>)
                 and (std::ranges::borrowed_range<Range> or is_const<element_type> or cpts::relocatable_owning_range<Range>)
            explicit(extent != dynamic_extent)
        constexpr basic_container(Range&& range)
            noexcept( noexcept(base(range))
                  and noexcept(capsule_from_range(EMU_FWD(range))))
            : base(range)
            , capsule(capsule_from_range(EMU_FWD(range)))
        {}

        template<cpts::contiguous_sized_range Range, typename DataHolder>
            requires (validate_source<Range>)
            explicit(extent != dynamic_extent)
        constexpr basic_container(Range&& range, DataHolder&& dh)
            : base(EMU_FWD(range))
            , capsule(EMU_FWD(dh))
        {}

        // explicit(extent != std::dynamic_extent)
        // constexpr basic_container( std::initializer_list<value_type> il ) noexcept
        //     requires validate_source<std::initializer_list<value_type>>
        //          and std::is_const_v<element_type>
        //     : base(il.begin(), il.end())
        // {}

        template<typename OT, size_t OExtent, typename OActualType, typename DataHolder>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_container(const basic_container<OT, OExtent, LocationPolicy, OActualType>& other) noexcept
            : base(static_cast<const base&>(other))
            , capsule(static_cast<const capsule&>(other))
        {}

        template<typename OT, size_t OExtent, typename OActualType, typename DataHolder>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_container(const basic_span<OT, OExtent, LocationPolicy, OActualType>& other, DataHolder&& dh) noexcept
            : base(static_cast<const base&>(other))
            , capsule(EMU_FWD(dh))
        {}

        template<typename DataHolder>
        constexpr explicit
        basic_container(base s, DataHolder&& dh)
            : base(s)
            , capsule(EMU_FWD(dh))
        {}

        constexpr basic_container(const basic_container&) noexcept = default;
        constexpr basic_container(basic_container&&) noexcept = default;

        constexpr basic_container& operator=(const basic_container&) noexcept = default;
        constexpr basic_container& operator=(basic_container&&) noexcept = default;

        ~basic_container() noexcept = default;

        // emu::capsule&        capsule() &      noexcept { return capsule_; }
        // emu::capsule const & capsule() const& noexcept { return capsule_; }
        // emu::capsule         capsule() &&     noexcept { return move(capsule_); }

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

} // namespace detail

} // namespace emu

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
