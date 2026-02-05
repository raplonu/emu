#pragma once

#include <emu/detail/mdspan_types.hpp>
#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>
#include <emu/mdspan.hpp>

#include <ranges>

namespace emu
{

template<typename T>
struct tensor_traits {
    // By default, all types are considered unsupported tensors. Specializations will provide actual traits for supported tensor types.
    //TODO: Evaluate the possibility to provide some default implementations for non-tensor types, such as scalar types or other ranges.
    constexpr static bool unsupported = true;
};

namespace detail
{

    template <typename T, typename Default, typename = void>
    struct accessor_type_or_impl {
        using type = Default;
    };

    // Specialization: enabled only if T::accessor_type exists
    template <typename T, typename Default>
    struct accessor_type_or_impl<T, Default, std::void_t<typename T::accessor_type>> {
        using type = typename T::accessor_type;
    };

    template <typename T, typename Default>
    using accessor_type_or = typename accessor_type_or_impl<T, Default>::type;

} // namespace detail

namespace cpts
{

    // check if T is not unsupported by tensor_traits
    template<typename T>
    concept tensor = not requires { tensor_traits<T>::unsupported; };

} // namespace cpts

    template<cpts::mdspan MdSpan>
    struct tensor_traits<MdSpan>
    {
        using type = rm_cvref<MdSpan>;

        static constexpr std::size_t rank = type::rank();

        using extents_type = typename type::extents_type;
        using layout_type = typename type::layout_type;

        using mapping_type = typename type::mapping_type;

        using element_type = typename type::element_type;
        using value_type = typename type::value_type;

        using index_type = typename type::index_type;
        using size_type = typename type::size_type;
        using rank_type = typename type::rank_type;

        using accessor_type = typename type::accessor_type;

        using data_handle_type = typename type::data_handle_type;
        using reference = typename type::reference;

        static constexpr data_handle_type data_handle(const MdSpan& mdspan) noexcept {
            return mdspan.data_handle();
        }

        static constexpr const mapping_type& mapping(const MdSpan& mdspan) noexcept {
            return mdspan.mapping();
        }

        static constexpr const accessor_type& accessor(const MdSpan& mdspan) noexcept {
            return mdspan.accessor();
        }

        static constexpr size_type size(const MdSpan& mdspan) noexcept {
            return mdspan.size();
        }

        static constexpr bool is_empty(const MdSpan& mdspan) noexcept {
            return mdspan.is_empty();
        }

        static constexpr index_type extent(const MdSpan& mdspan, rank_type r) noexcept {
            return mdspan.extent(r);
        }

        static constexpr index_type stride(const MdSpan& mdspan, rank_type r) noexcept {
            return mdspan.stride(r);
        }

        static constexpr bool is_unique(const MdSpan& mdspan) { return mdspan.is_unique(); }
        static constexpr bool is_exhaustive(const MdSpan& mdspan) { return mdspan.is_exhaustive(); }
        static constexpr bool is_strided(const MdSpan& mdspan) { return mdspan.is_strided(); }
        static constexpr bool is_always_unique(const MdSpan& mdspan) { return mdspan.is_always_unique(); }
        static constexpr bool is_always_exhaustive(const MdSpan& mdspan) { return mdspan.is_always_exhaustive(); }
        static constexpr bool is_always_strided(const MdSpan& mdspan) { return mdspan.is_always_strided(); }

        static constexpr std::size_t required_span_size(const MdSpan& mdspan) {
            return mdspan.required_span_size();
        }
    };

    /**
     * @brief Specialization of tensor_traits for any type that is a contiguous & sized range.
     *
     * This concept covers types like `std::span`, `std::vector`, and other similar view/containers.
     *
     * @tparam Span
     */
    template<typename Range>
        requires std::ranges::contiguous_range<Range>
             and std::ranges::sized_range<Range>
    struct tensor_traits<Range>
    {
        static constexpr std::size_t rank = 1;

        using extents_type = dims<rank>;
        using layout_type = layout_right;

        using mapping_type = typename layout_type::mapping<extents_type>;

        using element_type = rm_ref<typename std::ranges::range_reference_t<Range>>;
        using value_type = typename std::ranges::range_value_t<Range>;

        using index_type = std::size_t;
        using size_type = std::size_t;
        using rank_type = std::size_t;

        using accessor_type = detail::accessor_type_or<rm_cvref<Range>, default_accessor<element_type>>;

        using data_handle_type = typename accessor_type::data_handle_type;
        using reference = typename accessor_type::reference;

        template<typename R>
        static constexpr data_handle_type data_handle(R&& range) noexcept {
            return std::ranges::data(EMU_FWD(range));
        }

        static constexpr mapping_type mapping(const Range& range) noexcept {
            return mapping_type(extents_type{size(range)});
        }

        static constexpr accessor_type accessor(const Range& range) noexcept {
            if constexpr (requires { range.accessor(); })
                return range.accessor();
            else
                return accessor_type();
        }

        static constexpr size_type size(const Range& range) noexcept {
            return std::ranges::size(range);
        }

        static constexpr bool is_empty(const Range& range) noexcept {
            return std::ranges::empty(range);
        }

        static constexpr index_type extent(const Range& range, rank_type) noexcept {
            return size(range);
        }

        static constexpr index_type stride(const Range&, rank_type) noexcept {
            return 1;
        }

        static constexpr bool is_unique(const Range&) { return true; }
        static constexpr bool is_exhaustive(const Range&) { return true; }
        static constexpr bool is_strided(const Range&) { return true; }
        static constexpr bool is_always_unique(const Range&) { return true; }
        static constexpr bool is_always_exhaustive(const Range&) { return true; }
        static constexpr bool is_always_strided(const Range&) { return true; }

        static constexpr std::size_t required_span_size(const Range& range) {
            return size(range);
        }
    };

namespace spe
{

    template <typename T>
    struct tensor_converter {

        template <typename TT>
        static constexpr auto convert(TT&& t) noexcept {
            using traits = tensor_traits<rm_ref<TT>>;

            using mdspan_type = mdspan<
                typename traits::element_type,
                typename traits::extents_type,
                typename traits::layout_type,
                typename traits::accessor_type
            >;

            return mdspan_type{
                traits::data_handle(t),
                traits::mapping(t),
                traits::accessor(t)
            };
        }
    };

} // namespace spe

    template<typename T>
    constexpr auto as_tensor(T&& t) noexcept {

        if constexpr (requires { t.as_tensor(); })
            return t.as_tensor();
        else if constexpr (cpts::mdspan<rm_cvref<T>>)
            return EMU_FWD(t);
        else
            return spe::tensor_converter<rm_cvref<T>>::convert(EMU_FWD(t));
    }

    template<typename T>
    using mdspan_type_t = decltype(as_tensor(std::declval<T>()));

} // namespace emu
