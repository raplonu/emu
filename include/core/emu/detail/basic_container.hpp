#pragma once

#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>

#include <emu/detail/basic_span.hpp>
#include <emu/capsule.hpp>

#include <initializer_list>
#include <ranges>
#include <vector>

namespace emu
{

namespace detail
{

    template <typename ElementType, size_t Extent, typename AccessorPolicy>
    struct basic_container;

};

namespace cpts
{

    template <typename T>
    concept container = std::derived_from<T, emu::detail::basic_container<typename T::element_type, T::extent, typename T::accessor_type>>;

    template <typename T>
    concept const_container = container<T> and std::is_const_v<typename T::element_type>;

    template <typename T>
    concept mutable_container = container<T> and (not std::is_const_v<typename T::element_type>);

} // namespace cpts

namespace detail
{

    /**
     * @brief A basic container that combines span-like functionality with lifetime management.
     *
     * `basic_container` inherits from `basic_span` to provide a view over a contiguous
     * sequence of objects, and from `emu::capsule` to manage the lifetime of the
     * underlying data. This makes it a versatile container that can be used for
     * both owning and non-owning views of data.
     *
     * @tparam ElementType The type of elements in the container.
     * @tparam Extent The extent of the container, which can be dynamic.
     * @tparam AccessorPolicy A policy for accessing the elements.
     * @tparam ActualType The actual derived container type.
     */
    template <typename ElementType, size_t Extent, typename AccessorPolicy>
    struct basic_container : basic_span<ElementType, Extent, AccessorPolicy>, emu::capsule
    {
        using span_type = basic_span<ElementType, Extent, AccessorPolicy>;
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

        using accessor_type   = typename span_type::accessor_type;

        static constexpr size_t extent = Extent;

    public:
        /**
         * @brief Inherits constructors from `basic_span`.
         */
        using span_type::span_type;


        /**
         * @brief Constructs a `basic_container` from an iterator, a count, and a data holder.
         * @param first An iterator to the beginning of the sequence.
         * @param count The number of elements in the sequence.
         * @param dh The data holder for lifetime management.
         * @param i An integer used to disambiguate the constructor.
         */
        template <std::contiguous_iterator It, typename DataHolder>
            explicit (extent != dynamic_extent)
        constexpr basic_container(It first, size_t count, DataHolder&& dh)
            : span_type(first, count)
            , capsule_base(EMU_FWD(dh))
        {};

        /**
         * @brief Constructs a `basic_container` from a begin and end iterator, and a data holder.
         * @param first An iterator to the beginning of the sequence.
         * @param last An iterator to the end of the sequence.
         * @param dh The data holder for lifetime management.
         */
        template <std::contiguous_iterator It, std::sized_sentinel_for<It> End, typename DataHolder>
            requires (not std::is_convertible_v<End, size_t>)
            explicit (extent != dynamic_extent)
        constexpr basic_container(It first, End last, DataHolder&& dh)
            : span_type(first, last)
            , capsule_base(EMU_FWD(dh))
        {};

        /**
         * @brief Constructs a `basic_container` from a contiguous range.
         * @param range The contiguous range.
         */
        template<cpts::contiguous_sized_range Range>
            requires (cpts::not_derived_from<rm_cvref<Range>, basic_container>)
                 and (std::ranges::borrowed_range<Range> or is_const<element_type> or cpts::relocatable_owning_range<Range>)
            explicit (extent != dynamic_extent)
        constexpr basic_container(Range&& range)
            noexcept( noexcept(span_type(range))
                  and noexcept(capsule_from_range(EMU_FWD(range))))
            : span_type(range)
            , capsule_base(capsule_from_range(EMU_FWD(range)))
        {}

        /**
         * @brief Constructs a `basic_container` from a contiguous range and a data holder.
         * @param range The contiguous range.
         * @param dh The data holder for lifetime management.
         */
        template<cpts::contiguous_sized_range Range, typename DataHolder>
            explicit (extent != dynamic_extent)
        constexpr basic_container(Range&& range, DataHolder&& dh)
            : span_type(EMU_FWD(range))
            , capsule_base(EMU_FWD(dh))
        {}

        explicit (extent != std::dynamic_extent)
        constexpr basic_container( std::initializer_list<value_type> il ) noexcept
            requires std::is_const_v<element_type>
            : basic_container(std::vector<value_type>(il)) // Move list into vector to manage lifetime and capture in capsule
        {}

        /**
         * @brief Copy constructor from another `basic_container`.
         * @param other The other `basic_container` to copy from.
         */
        template<typename OT, size_t OExtent, typename DataHolder>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        explicit (extent != dynamic_extent && OExtent != dynamic_extent)
        constexpr basic_container(const basic_container<OT, OExtent, AccessorPolicy>& other) noexcept
            : span_type(static_cast<const span_type&>(other))
            , capsule_base(other.capsule())
        {}

        /**
         * @brief Constructs a `basic_container` from a `basic_span` and a data holder.
         * @param other The `basic_span` to construct from.
         * @param dh The data holder for lifetime management.
         */
        template<typename OT, size_t OExtent, typename DataHolder>
        requires (extent == dynamic_extent
               or OExtent == dynamic_extent
               or extent == OExtent)
        explicit (extent != dynamic_extent && OExtent != dynamic_extent)
        constexpr basic_container(const basic_span<OT, OExtent, AccessorPolicy>& other, DataHolder&& dh) noexcept
            : span_type(static_cast<const span_type&>(other))
            , capsule_base(EMU_FWD(dh))
        {}

        /**
         * @brief Constructs a `basic_container` from a `span_type` and a data holder.
         * @param s The `span_type` to construct from.
         * @param dh The data holder for lifetime management.
         */
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

        /**
         * @brief Returns a reference to the underlying capsule.
         * @return A reference to the capsule.
         */
        emu::capsule&        capsule() &      noexcept { return static_cast<emu::capsule&>(*this); }
        /**
         * @brief Returns a const reference to the underlying capsule.
         * @return A const reference to the capsule.
         */
        emu::capsule const & capsule() const& noexcept { return static_cast<const emu::capsule&>(*this); }
        /**
         * @brief Returns an rvalue reference to the underlying capsule.
         * @return An rvalue reference to the capsule.
         */
        emu::capsule&&         capsule() &&     noexcept { return static_cast<emu::capsule&&>(*this); }

        // auto use_count() const noexcept { return capsule_.use_count(); }

        // template< std::size_t Count >
        // constexpr basic_container<element_type, Count, location_type> first() const noexcept {
        //     return from_span(span_type::template first<Count>());
        // }

        // constexpr basic_container<element_type, std::dynamic_extent, location_type> first( size_type count ) const noexcept {
        //     return from_span(span_type::first(count));
        // }

        // template< std::size_t Count >
        // constexpr basic_container<element_type, Count, location_type> last() const noexcept {
        //     return from_span(span_type::template last<Count>());
        // }

        // constexpr basic_container<element_type, std::dynamic_extent, location_type> last( size_type count ) const noexcept {
        //     return from_span(span_type::last(count));
        // }

        /**
         * @brief Creates a subcontainer of this container.
         * @tparam Offset The offset from the beginning of the container.
         * @tparam Count The number of elements in the subcontainer.
         * @return A new container that is a view of a subsection of this container.
         */
        template< std::size_t Offset,
              std::size_t Count = std::dynamic_extent >
        constexpr auto subcontainer() const noexcept {
            return actual_from_base(span_type::template subspan<Offset, Count>());
        }

        /**
         * @brief Creates a subcontainer of this container.
         * @param offset The offset from the beginning of the container.
         * @param count The number of elements in the subcontainer.
         * @return A new container that is a view of a subsection of this container.
         */
        constexpr auto subcontainer( size_type offset,
                 size_type count = std::dynamic_extent ) const noexcept {
            return actual_from_base(span_type::subspan(offset, count));
        }

        /**
         * @brief Returns a pair consisting of the span and the capsule.
         * @return A pair containing the span and the capsule.
         */
        std::pair<span_type, emu::capsule> as_pair() const & noexcept  {
            return {span_type(*this), capsule()};
        }

        /**
         * @brief Returns a pair consisting of the span and the capsule.
         * @return A pair containing the span and the capsule.
         */
        std::pair<span_type, emu::capsule> as_pair() && noexcept {
            return {span_type(*this), std::move(capsule())};
        }

        // template< size_t Offset, size_t Count = dynamic_extent >
        // constexpr auto subspan() const -> basic_container = delete;

        // constexpr auto subspan( size_type offset, size_type count = dynamic_extent ) const -> basic_container = delete;

        // using span_type::begin;
        // using span_type::end;
        // using span_type::rbegin;
        // using span_type::rend;

        template< size_t Count >
        constexpr auto first() const {
            return actual_from_base(span_type::template first<Count>());
        }

        constexpr auto first( size_type count ) const {
            return actual_from_base(span_type::first(count));
        }

        template< size_t Count >
        constexpr auto last() const {
            return actual_from_base(span_type::template last<Count>());
        }

        constexpr auto last( size_type count ) const {
            return actual_from_base(span_type::last(count));
        }

        template< size_t Offset, size_t Count = dynamic_extent >
        constexpr auto subspan() const {
            return actual_from_base(span_type::template subspan<Offset, Count>());
        }

        constexpr auto subspan( size_type offset, size_type count = dynamic_extent ) const {
            return actual_from_base(span_type::subspan(offset, count));
        }

        // using span_type::front;
        // using span_type::back;
        // using span_type::operator[];
        // using span_type::data;

        constexpr reference operator[](size_type idx) const noexcept
        {
            return span_type::operator[](idx);
        }
        // using span_type::size;
        // using span_type::size_bytes;
       // using span_type::empty;

        template<typename OT, size_t OExtent>
        constexpr auto actual_from_base(std::span<OT, OExtent> sp) const noexcept {
            return basic_container(sp.data(), sp.size(), this->capsule());
        }

    };

    //Note: the following deduction guides do not take into account the AccessorPolicy.
    //It must be provided by alias partial specialization. Then `void` will be replaced by the actual accessor policy.

    template< class It, class EndOrSize >
    basic_container( It, EndOrSize )
        -> basic_container< iterator_cv_value<It>, dynamic_extent , void>;
    template< class It, class EndOrSize, class DataHolder >
    basic_container( It, EndOrSize, DataHolder&& )
        -> basic_container< iterator_cv_value<It>, dynamic_extent , void>;

    template< class T, size_t N >
    basic_container( T (&)[N] )
        -> basic_container<T, N, void>;
    template< class T, size_t N, class DataHolder >
    basic_container( T (&)[N], DataHolder&& )
        -> basic_container<T, N, void>;

    template< typename Range >
    basic_container( Range&& )
        -> basic_container< range_cv_value<Range>, dynamic_extent, void>;
    template< typename Range, class DataHolder >
    basic_container( Range&&, DataHolder&& )
        -> basic_container< range_cv_value<Range>, dynamic_extent, void>;

    template< class T, size_t N >
    basic_container( std::array<T, N>& )
        -> basic_container<T, N, void>;
    template< class T, size_t N >
    basic_container( const std::array<T, N>& )
        -> basic_container< const T, N, void>;

    template< class T, size_t N >
    basic_container( std::span<T, N> )
        -> basic_container<T, N, void>;
    template< class T, size_t N, class DataHolder >
    basic_container( std::span<T, N>, DataHolder&& )
        -> basic_container<T, N, void>;

    template< class T, size_t N >
    basic_container( std::span<const T, N> )
        -> basic_container< const T, N, void>;
    template< class T, size_t N, class DataHolder >
    basic_container( std::span<const T, N>, DataHolder&& )
        -> basic_container< const T, N, void>;

    template< typename T >
    basic_container( std::initializer_list<T> )
        -> basic_container< T, dynamic_extent, void>;


} // namespace detail

    template <typename ElementType, size_t Extent, typename AccessorPolicy>
     struct tensor_traits< detail::basic_container<ElementType, Extent, AccessorPolicy>>
        : tensor_traits< typename detail::basic_container<ElementType, Extent, AccessorPolicy>::span_type >
     {};

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
