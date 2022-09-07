#ifndef EMU_MDSPAN_H
#define EMU_MDSPAN_H

#include <emu/type_traits.h>
#include <emu/misc/location.h>
#include <emu/span.h>

#include <experimental/mdspan>

namespace emu
{

namespace mdspan
{

    using namespace emu::span;

namespace detail
{

    namespace stdex = std::experimental;

} // namespace detail

    using detail::stdex::layout_right;
    using detail::stdex::layout_left;
    using detail::stdex::layout_stride;

    using layout_c = layout_right;
    using layout_f = layout_left;

    using layout_row    = layout_right;
    using layout_column = layout_left;

    using detail::stdex::default_accessor;

    template <size_t Rank>
    using dextents_t = detail::stdex::dextents<std::size_t, Rank>;

    template <size_t... Extents>
    using extents_t = detail::stdex::extents<std::size_t, Extents...>;

    using detail::stdex::full_extent_t;
    using detail::stdex::full_extent;

    /// Get stdex::mdspan extents as an array.
    template <typename T = std::size_t, typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    auto extents(const detail::stdex::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> & mdspan)
    {
        std::array<T, decltype(mdspan)::rank()> res{};
        for (std::size_t i = 0; i < decltype(mdspan)::rank(); ++i)
            res[i] = mdspan.extent(0);
        return res;
    }

    /// Get stdex::mdspan strides as an array.
    template <typename T = std::size_t, typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    auto strides(const detail::stdex::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> & mdspan)
    {
        std::array<T, decltype(mdspan)::rank()> res{};
        for (std::size_t i = 0; i < decltype(mdspan)::rank(); ++i)
            res[i] = mdspan.stride(0);
        return res;
    }

    // TODO: Replaces all strides version by the a template when C++17.
    template <typename T = std::size_t>
    auto strides(span_t<const std::size_t> extents, std::size_t word_size)
    {
        std::vector<T> res(extents.size());

        auto last = word_size;
        for (int i = extents.size() - 1; i >= 0; --i) {
            res[i] = last;
            last = extents[i] * res[i];
        }
        return res;
    }

    template <typename T = std::size_t>
    auto strides(span_t<const int> extents, std::size_t word_size)
    {
        std::vector<T> res(extents.size());

        auto last = word_size;
        for (int i = extents.size() - 1; i >= 0; --i) {
            res[i] = last;
            last = extents[i] * res[i];
        }
        return res;
    }

    template <typename T = std::size_t>
    auto strides(span_t<const long int> extents, std::size_t word_size)
    {
        std::vector<T> res(extents.size());

        auto last = word_size;
        for (int i = extents.size() - 1; i >= 0; --i) {
            res[i] = last;
            last = extents[i] * res[i];
        }
        return res;
    }

namespace detail
{

    using namespace emu::span::detail;

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
        // using difference_type = typename base_t::difference_type;
        using pointer         = typename base_t::pointer;
        using reference       = typename base_t::reference;

        using location_type   = Location;

    private:
        static constexpr auto DefaultAccessor = std::is_default_constructible_v<accessor_type>;
        static constexpr auto MappingConstructible = std::is_constructible_v<mapping_type, extents_type>;

        template<typename SizeType, std::size_t N>
        static constexpr auto array_match =
            std::is_convertible_v<SizeType, size_type> and
            N == extents_type::rank_dynamic();

    public:


        MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdspan_t() = default;
        MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdspan_t(const mdspan_t&) = default;
        MDSPAN_INLINE_FUNCTION_DEFAULTED constexpr mdspan_t(mdspan_t&&) = default;

        MDSPAN_TEMPLATE_REQUIRES(
            class... SizeTypes,
            /* requires */ (
            _MDSPAN_FOLD_AND(_MDSPAN_TRAIT(std::is_convertible, SizeTypes, size_type) /* && ... */) &&
            _MDSPAN_TRAIT(std::is_constructible, extents_type, SizeTypes...) &&
            _MDSPAN_TRAIT(std::is_constructible, mapping_type, extents_type) &&
            _MDSPAN_TRAIT(std::is_default_constructible, accessor_type)
            )
        )
        MDSPAN_INLINE_FUNCTION
        explicit constexpr mdspan_t(pointer p, SizeTypes... dynamic_extents)
            // TODO @proposal-bug shouldn't I be allowed to do `move(p)` here?
            : base_t(p, dynamic_extents...), location_()
        { }

        MDSPAN_TEMPLATE_REQUIRES(
            class SizeType, size_t N,
            /* requires */ (
            _MDSPAN_TRAIT(std::is_convertible, SizeType, size_type) &&
            _MDSPAN_TRAIT(std::is_constructible, extents_type, std::array<SizeType, N>) &&
            _MDSPAN_TRAIT(std::is_constructible, mapping_type, extents_type) &&
            _MDSPAN_TRAIT(std::is_default_constructible, accessor_type)
            )
        )
        MDSPAN_CONDITIONAL_EXPLICIT(N != extents_type::rank_dynamic())
        MDSPAN_INLINE_FUNCTION
        constexpr mdspan_t(pointer p, const std::array<SizeType, N>& dynamic_extents, location_type location = {})
            : base_t(p, dynamic_extents), location_(location)
        { }

        MDSPAN_FUNCTION_REQUIRES(
            (MDSPAN_INLINE_FUNCTION constexpr),
            mdspan_t, (pointer p, const extents_type& exts, location_type location = {}), ,
            /* requires */ (_MDSPAN_TRAIT(std::is_default_constructible, accessor_type) &&
                            _MDSPAN_TRAIT(std::is_constructible, mapping_type, extents_type))
        ) : base_t(p, exts), location_(location)
        { }

        MDSPAN_FUNCTION_REQUIRES(
            (MDSPAN_INLINE_FUNCTION constexpr),
            mdspan_t, (pointer p, const mapping_type& m, location_type location = {}), ,
            /* requires */ (_MDSPAN_TRAIT(std::is_default_constructible, accessor_type))
        ) : base_t(p, m), location_(location)
        { }

        MDSPAN_INLINE_FUNCTION
        constexpr mdspan_t(pointer p, const mapping_type& m, const accessor_type& a, location_type location = {})
            : base_t(p, m, a), location_(location)
        { }

        MDSPAN_TEMPLATE_REQUIRES(
            class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor,
            /* requires */ (
            _MDSPAN_TRAIT(std::is_constructible, mapping_type, typename OtherLayoutPolicy::template mapping<OtherExtents>) &&
            _MDSPAN_TRAIT(std::is_constructible, accessor_type, OtherAccessor) &&
            _MDSPAN_TRAIT(std::is_constructible, pointer, typename OtherAccessor::pointer) &&
            _MDSPAN_TRAIT(std::is_constructible, extents_type, OtherExtents)
            )
        )
        MDSPAN_INLINE_FUNCTION
        constexpr mdspan_t(const mdspan_t<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other)
            : base_t(other), location_(other.location)
        { }

        MDSPAN_INLINE_FUNCTION_DEFAULTED
        ~mdspan_t() = default;

        // EMU_HODE constexpr mdspan_t()                noexcept(IsNothrowDefaultConstructible<location_type>) = default;
        // EMU_HODE constexpr mdspan_t(const mdspan_t&) noexcept(IsNothrowCopyConstructible   <location_type>) = default;
        // EMU_HODE constexpr mdspan_t(mdspan_t&&)      noexcept(IsNothrowMoveConstructible   <location_type>) = default;

        // // Constructor with dynamic_extents.

        // template< bool Dependent = false,
        //     EnableIf<
        //         Dependent ||
        //         MappingConstructible     and
        //         DefaultAccessor
        //     > = true
        // >
        // EMU_HODE constexpr mdspan_t(pointer p, location_type location = {}) noexcept:
        //     base_t(p, mapping_type(extents_type()), accessor_type()), location_(location)
        // {}

        // template<
        //     typename SizeType, std::size_t N,
        //     EnableIf<
        //         array_match<SizeType, N> and
        //         MappingConstructible     and
        //         DefaultAccessor
        //     > = true
        // >
        // EMU_HODE constexpr mdspan_t(pointer p, const std::array<SizeType, N>& dynamic_extents, location_type location = {}) noexcept:
        //     base_t(p, mapping_type(extents_type(dynamic_extents)), accessor_type()), location_(location)
        // {}

        // template<
        //     typename SizeType, std::size_t N,
        //     EnableIf<
        //         array_match<SizeType, N> and
        //         MappingConstructible
        //     > = true
        // >
        // EMU_HODE constexpr mdspan_t(pointer p, const std::array<SizeType, N>& dynamic_extents, const accessor_type& a, location_type location = {}) noexcept:
        //     base_t(p, mapping_type(extents_type(dynamic_extents)), a), location_(location)
        // {}

        // // Constructor with extents_type.

        // template< bool Dependent = false,
        //     EnableIf<Dependent || MappingConstructible and DefaultAccessor> = true
        // >
        // EMU_HODE constexpr mdspan_t(pointer p, const extents_type& exts, location_type location = {}) noexcept:
        //     base_t(p, mapping_type(exts), accessor_type()), location_(location)
        // {}

        // template< bool Dependent = false,
        //     EnableIf<Dependent || MappingConstructible> = true
        // >
        // EMU_HODE constexpr mdspan_t(pointer p, const extents_type& exts, const accessor_type& a, location_type location = {}) noexcept:
        //     base_t(p, mapping_type(exts), a), location_(location)
        // {}

        // // Constructor with mapping_type.

        // template< bool Dependent = false,
        //     EnableIf<Dependent || DefaultAccessor> = true
        // >
        // EMU_HODE constexpr mdspan_t(pointer p, const mapping_type& m, location_type location = {}) noexcept:
        //     base_t(p, m), location_(location)
        // {}

        // EMU_HODE constexpr mdspan_t(pointer p, const mapping_type& m, const accessor_type& a, location_type location = {}) noexcept:
        //     base_t(p, m, a), location_(location)
        // {}

        // // Other constructor

        // EMU_HODE constexpr mdspan_t(base_t base, location_type location = {}) noexcept:
        //     base_t(base), location_(location)
        // {}

        // template<class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor,
        //     EnableIf<
        //         std::is_convertible_v<typename OtherLayoutPolicy::template mapping<OtherExtents>, mapping_type> &&
        //         std::is_convertible_v<OtherAccessor, accessor_type> &&
        //         std::is_convertible_v<typename OtherAccessor::pointer, pointer> &&
        //         std::is_convertible_v<OtherExtents, extents_type>
        //     > = true
        // >
        // EMU_HODE constexpr mdspan_t(const mdspan_t<OtherElementType, Location, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other):
        //     base_t(other), location_(other.location())
        // {}

        // template<class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor,
        //     EnableIf<
        //         std::is_convertible_v<typename OtherLayoutPolicy::template mapping<OtherExtents>, mapping_type> &&
        //         std::is_convertible_v<OtherAccessor, accessor_type> &&
        //         std::is_convertible_v<typename OtherAccessor::pointer, pointer> &&
        //         std::is_convertible_v<OtherExtents, extents_type>
        //     > = true
        // >
        // EMU_HODE constexpr mdspan_t(const stdex::mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other):
        //     base_t(other), location_()
        // {}

        // template<bool Dependent = false,
        //     class OtherElementType, std::size_t OtherExtents,
        //     EnableIf< Dependent or base_t::static_extent(0) == dynamic_extent > = true,
        //     EnableIf< Dependent or base_t::rank() == 1 > = true,
        //     EnableIf< Dependent or IsAllowedExtentConversion<OtherExtents, base_t::static_extent(0)> > = true,
        //     EnableIf< Dependent or std::is_convertible_v<typename detail::span_t<OtherElementType, Location, OtherExtents>::pointer, pointer> > = true
        // >
        // explicit EMU_HODE constexpr mdspan_t(const detail::span_t<OtherElementType, Location, OtherExtents>& other) noexcept:
        //     base_t(other.data(), mapping_type(extents_type({other.size()})), accessor_type()), location_(other.location)
        // {}

        // template<bool Dependent = false,
        //     class OtherElementType, std::size_t OtherExtents,
        //     EnableIf< Dependent or base_t::static_extent(0) != dynamic_extent > = true,
        //     EnableIf< Dependent or base_t::rank() == 1 > = true,
        //     EnableIf< Dependent or IsAllowedExtentConversion<OtherExtents, base_t::static_extent(0)> > = true,
        //     EnableIf< Dependent or std::is_convertible_v<typename detail::span_t<OtherElementType, Location, OtherExtents>::pointer, pointer> > = true
        // >
        // explicit EMU_HODE constexpr mdspan_t(const detail::span_t<OtherElementType, Location, OtherExtents>& other) noexcept:
        //     base_t(other.data(), mapping_type(extents_type()), accessor_type()), location_(other.location)
        // {}

        // template<bool Dependent = false,
        //     class OtherElementType, std::size_t OtherExtents,
        //     EnableIf< Dependent or base_t::static_extent(0) == dynamic_extent > = true,
        //     EnableIf< Dependent or base_t::rank() == 1 > = true,
        //     EnableIf< Dependent or IsAllowedExtentConversion<OtherExtents, base_t::static_extent(0)> > = true,
        //     EnableIf< Dependent or std::is_convertible_v<typename gsl::span<OtherElementType, OtherExtents>::pointer, pointer> > = true
        // >
        // EMU_HODE constexpr mdspan_t(const gsl::span<OtherElementType, OtherExtents>& other, location_type location = {}) noexcept:
        //     base_t(other.data(), mapping_type(extents_type({other.size()})), accessor_type()), location_(location)
        // {}

        // template<bool Dependent = false,
        //     class OtherElementType, std::size_t OtherExtents,
        //     EnableIf< Dependent or base_t::static_extent(0) != dynamic_extent > = true,
        //     EnableIf< Dependent or base_t::rank() == 1 > = true,
        //     EnableIf< Dependent or IsAllowedExtentConversion<OtherExtents, base_t::static_extent(0)> > = true,
        //     EnableIf< Dependent or std::is_convertible_v<typename gsl::span<OtherElementType, OtherExtents>::pointer, pointer> > = true
        // >
        // EMU_HODE constexpr mdspan_t(const gsl::span<OtherElementType, OtherExtents>& other, location_type location = {}) noexcept:
        //     base_t(other.data(), mapping_type(extents_type()), accessor_type()), location_(location)
        // {}

        EMU_HODE constexpr mdspan_t& operator=(const mdspan_t&) noexcept = default;
        EMU_HODE constexpr mdspan_t& operator=(mdspan_t&&) noexcept = default;

        template<typename... SliceSpecs>
        EMU_HODE constexpr auto subspan(SliceSpecs... slices) const noexcept {
            return as_span_t(detail::stdex::submdspan(*this, EMU_FWD(slices)...));
        }

        EMU_HODE constexpr std::array<size_type, base_t::rank()> extents() const noexcept {
            // return strides();
            std::array<size_type, base_t::rank()> s{};
            for (auto i = 0; i < base_t::rank(); ++i) s[i] = base_t::extent(i);
            return s;
        }

        EMU_HODE constexpr std::array<size_type, base_t::rank()> stride() const noexcept {
            std::array<size_type, base_t::rank()> s{};
            for (auto i = 0; i < base_t::rank(); ++i) s[i] = base_t::stride(i);
            return s;
        }

        // Explicitly expose stride from super since this::stride mask it.
        using base_t::stride;

        EMU_HODE constexpr location_type location() const noexcept {
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

} // namespace mdspan

    template <typename ElementType, typename Extents, typename LayoutPolicy = mdspan::layout_right, typename AccessorPolicy = mdspan::default_accessor<ElementType>>
    using mdspan_t = mdspan::detail::mdspan_t<ElementType, location::host_t, Extents, LayoutPolicy, AccessorPolicy>;

    template<typename ElementType> using mdspan_1d_t   = mdspan_t<ElementType, mdspan::dextents_t<1>>;
    template<typename ElementType> using mdspan_2d_t   = mdspan_t<ElementType, mdspan::dextents_t<2>>;
    template<typename ElementType> using mdspan_3d_t   = mdspan_t<ElementType, mdspan::dextents_t<3>>;

    template<typename ElementType> using mdspan_1d_c_t = mdspan_t<ElementType, mdspan::dextents_t<1>>;
    template<typename ElementType> using mdspan_2d_c_t = mdspan_t<ElementType, mdspan::dextents_t<2>>;
    template<typename ElementType> using mdspan_3d_c_t = mdspan_t<ElementType, mdspan::dextents_t<3>>;

    template<typename ElementType> using mdspan_1d_f_t = mdspan_t<ElementType, mdspan::dextents_t<1>, mdspan::layout_f>;
    template<typename ElementType> using mdspan_2d_f_t = mdspan_t<ElementType, mdspan::dextents_t<2>, mdspan::layout_f>;
    template<typename ElementType> using mdspan_3d_f_t = mdspan_t<ElementType, mdspan::dextents_t<3>, mdspan::layout_f>;

    template<typename ElementType> using mdspan_1d_s_t = mdspan_t<ElementType, mdspan::dextents_t<1>, mdspan::layout_stride>;
    template<typename ElementType> using mdspan_2d_s_t = mdspan_t<ElementType, mdspan::dextents_t<2>, mdspan::layout_stride>;
    template<typename ElementType> using mdspan_3d_s_t = mdspan_t<ElementType, mdspan::dextents_t<3>, mdspan::layout_stride>;

namespace mdspan
{

    template<std::size_t Extent = dynamic_extent, typename T>
    constexpr auto create(T* begin, std::size_t count) noexcept
    {
        return mdspan_t<T, extents_t<Extent>>(begin, {count});
    }

    template<
        std::size_t Extent = dynamic_extent, typename T, typename Location,
        EnableIf<IsLocation<Location>> = true
    >
    constexpr auto create(T* begin, std::size_t count, Location && location) noexcept
    {
        return detail::mdspan_t<T, Location, extents_t<Extent>>(begin, {count}, EMU_FWD(location));
    }

    template<typename T, typename Location, std::size_t Extent>
    constexpr auto create(const detail::span_t<T, Location, Extent> & s) noexcept
    {
        return detail::mdspan_t<T, Location, extents_t<Extent>>(s.data(), extents_t<Extent>{s.size()}, s.location());
    }

    template<typename T, std::size_t Extent>
    constexpr auto create(const gsl::span<T, Extent> & s) noexcept
    {
        return mdspan_t<T, extents_t<Extent>>(s.data(), {s.size()});
    }

    template<typename T, std::size_t Extent, typename Location>
    constexpr auto create(const gsl::span<T, Extent> & s, Location && location) noexcept
    {
        return mdspan_t<T, extents_t<Extent>>(s.data(), {s.size()}, EMU_FWD(location));
    }

} // namespace mdspan

} // namespace emu

#endif //EMU_MDSPAN_H