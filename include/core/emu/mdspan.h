#ifndef EMU_MDSPAN_H
#define EMU_MDSPAN_H

#include <emu/type_traits.h>
#include <emu/byte.h>
#include <emu/misc/location.h>
#include <emu/span.h>

#include <experimental/mdspan>

namespace emu
{

namespace span
{

namespace detail
{

    namespace stdex = std::experimental;

} // namespace detail

    using detail::stdex::layout_right;
    using detail::stdex::layout_left;
    using detail::stdex::layout_stride;

    using detail::stdex::default_accessor;

    using detail::stdex::extents;
    using detail::stdex::full_extent_t;
    using detail::stdex::full_extent;

namespace detail
{

    template <typename ElementType, typename Location, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<ElementType>>
    struct mdspan_t : stdex::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> {

        using base_t = stdex::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

        using extents_type    = typename base_t::extents_type;
        using layout_type     = typename base_t::layout_type;
        using accessor_type   = typename base_t::accessor_type;
        using mapping_type    = typename base_t::mapping_type;
        using element_type    = typename base_t::element_type;
        using value_type      = typename base_t::value_type;
        using size_type       = typename base_t::size_type;
        using difference_type = typename base_t::difference_type;
        using pointer         = typename base_t::pointer;
        using reference       = typename base_t::reference;

        using location_type   = Location;

    private:
        static constexpr auto DefaultAccessor = IsDefaultConstructible<accessor_type>;
        static constexpr auto MappingConstructible = IsConstructible<mapping_type, extents_type>;

        template<typename SizeType, std::size_t N>
        static constexpr auto array_match =
            IsConvertible<SizeType, size_type> and
            N == extents_type::rank_dynamic();

    public:

        EMU_HODE constexpr mdspan_t() noexcept = default;
        EMU_HODE constexpr mdspan_t(const mdspan_t&) noexcept = default;
        EMU_HODE constexpr mdspan_t(mdspan_t&&) noexcept = default;

        // Constructor with dynamic_extents.

        template< bool Dependent = false,
            EnableIf<
                Dependent ||
                MappingConstructible     and
                DefaultAccessor
            > = true
        >
        EMU_HODE constexpr mdspan_t(pointer p, location_type location = {}) noexcept:
            base_t(p, mapping_type(extents_type()), accessor_type()), location_(location)
        {}

        template<
            typename SizeType, std::size_t N,
            EnableIf<
                array_match<SizeType, N> and
                MappingConstructible     and
                DefaultAccessor
            > = true
        >
        EMU_HODE constexpr mdspan_t(pointer p, const std::array<SizeType, N>& dynamic_extents, location_type location = {}) noexcept:
            base_t(p, mapping_type(extents_type(dynamic_extents)), accessor_type()), location_(location)
        {}

        template<
            typename SizeType, std::size_t N,
            EnableIf<
                array_match<SizeType, N> and
                MappingConstructible
            > = true
        >
        EMU_HODE constexpr mdspan_t(pointer p, const std::array<SizeType, N>& dynamic_extents, const accessor_type& a, location_type location = {}) noexcept:
            base_t(p, mapping_type(extents_type(dynamic_extents)), a), location_(location)
        {}

        // Constructor with extents_type.

        template< bool Dependent = false,
            EnableIf<Dependent || MappingConstructible and DefaultAccessor> = true
        >
        EMU_HODE constexpr mdspan_t(pointer p, const extents_type& exts, location_type location = {}) noexcept:
            base_t(p, mapping_type(exts), accessor_type()), location_(location)
        {}

        template< bool Dependent = false,
            EnableIf<Dependent || MappingConstructible> = true
        >
        EMU_HODE constexpr mdspan_t(pointer p, const extents_type& exts, const accessor_type& a, location_type location = {}) noexcept:
            base_t(p, mapping_type(exts), a), location_(location)
        {}

        // Constructor with mapping_type.

        template< bool Dependent = false,
            EnableIf<Dependent || DefaultAccessor> = true
        >
        EMU_HODE constexpr mdspan_t(pointer p, const mapping_type& m, location_type location = {}) noexcept:
            base_t(p, m), location_(location)
        {}

        EMU_HODE constexpr mdspan_t(pointer p, const mapping_type& m, const accessor_type& a, location_type location = {}) noexcept:
            base_t(p, m, a), location_(location)
        {}

        template<class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor,
            EnableIf<
                IsConvertible<typename OtherLayoutPolicy::template mapping<OtherExtents>, mapping_type> &&
                IsConvertible<OtherAccessor, accessor_type> &&
                IsConvertible<typename OtherAccessor::pointer, pointer> &&
                IsConvertible<OtherExtents, extents_type>
            > = true
        >
        EMU_HODE constexpr mdspan_t(const mdspan_t<OtherElementType, Location, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other):
            base_t(other), location_(other.location())
        {}

        template<class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor,
            EnableIf<
                IsConvertible<typename OtherLayoutPolicy::template mapping<OtherExtents>, mapping_type> &&
                IsConvertible<OtherAccessor, accessor_type> &&
                IsConvertible<typename OtherAccessor::pointer, pointer> &&
                IsConvertible<OtherExtents, extents_type>
            > = true
        >
        EMU_HODE constexpr mdspan_t(const stdex::mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other):
            base_t(other), location_()
        {}

        EMU_HODE constexpr mdspan_t& operator=(const mdspan_t&) noexcept = default;
        EMU_HODE constexpr mdspan_t& operator=(mdspan_t&&) noexcept = default;

        template<typename... SliceSpecs>
        EMU_HODE constexpr auto subspan(SliceSpecs... slices) const noexcept {
            return as_span_t(detail::stdex::submdspan(*this, EMU_FWD(slices)...));
        }

        location_type location() const noexcept {
            return location_;
        }

    private:
        location_type location_;

        template<class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor>
        constexpr auto as_span_t(stdex::mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor> span) const noexcept {
            return mdspan_t<OtherElementType, location_type, OtherExtents, OtherLayoutPolicy, OtherAccessor>(span, location());
        }
    };

} // namespace detail

} // namespace span

    template <typename ElementType, typename Extents, typename LayoutPolicy = span::layout_right, typename AccessorPolicy = span::default_accessor<ElementType>>
    using mdspan_t = span::detail::mdspan_t<ElementType, location::host_t, Extents, LayoutPolicy, AccessorPolicy>;

} // namespace emu

#endif //EMU_MDSPAN_H