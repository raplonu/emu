#pragma once


#include <emu/pybind11/cast/detail/capsule.hpp>
#include <emu/mdspan.hpp>
#include <emu/pybind11/cast/detail/mdspan_caster.hpp>

#include <emu/container.hpp>
#include <span>

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<emu::cpts::mdcontainer MdContainer>
    struct type_caster< MdContainer > : emu::cast::detail::mdspan_caster<MdContainer>
    {
        using base_caster = emu::cast::detail::mdspan_caster<MdContainer>;

        using cpp_type = MdContainer;

        using element_type  = typename cpp_type::element_type;
        using extents_type  = typename cpp_type::extents_type;
        using layout_type   = typename cpp_type::layout_type;
        using accessor_type = typename cpp_type::accessor_type;

        static_assert(extents_type::rank_dynamic() > 0, "pybind11 required only default constructible types. Fixed size mdcontainer are not.");

        static constexpr auto descr() {
            return _("mdcontainer<") + concat(make_caster<element_type>::name , const_name<cpp_type::rank()>() + _("d")) + _(">");
        }

        PYBIND11_TYPE_CASTER(cpp_type, descr());

        bool load(handle src, bool convert) {
            return base_caster::from_python(src, convert).map([&](auto new_value){
                auto capsule = emu::pybind11::detail::handle_to_capsule(src);
                value = cpp_type(new_value, std::move(capsule));
                return true;
            }).value_or(false);
        }

        static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            return base_caster::to_python(value, emu::pybind11::detail::capsule_to_handle(value.capsule())).inc_ref();
        }
    };

} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)


// #pragma once

// #include <emu/cast/common.hpp>

// namespace emu::cast
// {

//     template<typename ElementType, typename Extents, typename LayoutPolicy>
//     struct layout_adaptor;

//     template<typename ElementType, typename Extents>
//     struct layout_adaptor<ElementType, Extents, layout_right>
//     {
//       using mapping_type = typename layout_right::template mapping<Extents>;

//         static emu::optional<mapping_type> mapping(const py::buffer_info& buffer_info) noexcept {
//             std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
//             auto it_shape = dynamic_shape.rbegin();

//             std::size_t last_extent = 1;
//             std::size_t last_stride = sizeof(ElementType);
//             for(int i = Extents::rank() - 1; i >= 0; --i) {
//                 EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
//                 EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == buffer_info.strides[i]);
//                 last_extent = buffer_info.shape[i];
//                 last_stride = buffer_info.strides[i];
//                 if(Extents::static_extent(i) == std::dynamic_extent)
//                     *(it_shape++) = buffer_info.shape[i];

//             }

//             return mapping_type{Extents{dynamic_shape}};
//         }
//     };

//     template<typename ElementType, typename Extents>
//     struct layout_adaptor<ElementType, Extents, layout_left>
//     {
//         using mapping_type = typename layout_left::template mapping<Extents>;

//         static emu::optional<mapping_type> mapping(const py::buffer_info& buffer_info) noexcept {
//             std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
//             auto it_shape = dynamic_shape.begin();

//             std::size_t last_extent = 1;
//             std::size_t last_stride = sizeof(ElementType);
//             for(auto i = 0; i < Extents::rank(); ++i) {
//                 EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
//                 EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == buffer_info.strides[i]);
//                 last_extent = buffer_info.shape[i];
//                 last_stride = buffer_info.strides[i];
//                 if(Extents::static_extent(i) == std::dynamic_extent)
//                     *(it_shape++) = buffer_info.shape[i];
//             }

//             return mapping_type{Extents{dynamic_shape}};
//         }
//     };

//     template<typename ElementType, typename Extents>
//     struct layout_adaptor<ElementType, Extents, layout_stride>
//     {
//         using mapping_type = typename layout_stride::template mapping<Extents>;

//         static emu::optional<mapping_type> mapping(const py::buffer_info& buffer_info) noexcept {
//             std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
//             std::array<std::size_t, Extents::rank()> dynamic_strides;
//             auto it_shape = dynamic_shape.rbegin();

//             for(auto i = 0; i < Extents::rank(); ++i) {
//                 EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
//                 if(Extents::static_extent(i) == std::dynamic_extent)
//                     *(it_shape++) = buffer_info.shape[i];
//                 dynamic_strides[i] = buffer_info.strides[i] / sizeof(ElementType);
//             }

//             return mapping_type{Extents{dynamic_shape}, dynamic_strides};
//         }
//     };

// } // namespace emu::cast

// namespace pybind11
// {

// namespace detail
// {
//     // template<typename ElementType, typename Location, std::size_t Extent>
//     template<emu::cpts::mdspan Mdspan>
//     struct type_caster< Mdspan >
//     {
//         // using adaptor  = emu::cast::python_adaptor<emu::detail::span<ElementType, Location, Extent>>;
//         using cpp_type = Mdspan;

//         using element_type  = typename cpp_type::element_type;
//         using extents_type  = typename cpp_type::extents_type;
//         using layout_type   = typename cpp_type::layout_type;
//         using accessor_type = typename cpp_type::accessor_type;

//         using bia = emu::cast::buffer_info_adaptor<element_type>;
//         using la = emu::cast::layout_adaptor<element_type, extents_type, layout_type>;

//         template< std::size_t I >
//             requires (cpp_type::static_extent(I) == std::dynamic_extent)
//         static constexpr auto dim_descr() {
//             using namespace detail;
//             return _("*");
//         }

//         template< std::size_t I>
//             requires (cpp_type::static_extent(I) != std::dynamic_extent)
//         static constexpr auto dim_descr() {
//             return _<cpp_type::static_extent(I)>();
//         }

//         template< std::size_t... Is >
//         static constexpr auto shape_descr(std::index_sequence<Is...>) {
//             return concat(dim_descr<Is>()...);
//         }

//         static constexpr auto descr() {
//             return _("mdspan<") + concat(make_caster<element_type>::name + _("[") + shape_descr(std::make_index_sequence<cpp_type::rank()>{}) + _("]"), bia::loc_descr()) + _(">");
//         }

//         PYBIND11_TYPE_CASTER(cpp_type, descr());

//         bool load(handle src, bool convert) {
//             return bia::get_buffer_info(src).and_then([](auto buffer_info) -> emu::optional<cpp_type> {
//                 // Must have the same rank.
//                 EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.ndim == cpp_type::rank());

//                 for(auto i = 0; i < cpp_type::rank(); ++i)
//                     // Must be Extent if span is not dynamic.
//                     EMU_TRUE_OR_RETURN_NULLOPT(cpp_type::static_extent(i) == std::dynamic_extent or buffer_info.shape[i] == cpp_type::static_extent(i));

//                 return la::mapping(buffer_info).and_then([&] (auto&& extents) {
//                     return emu::optional<cpp_type>(emu::in_place,
//                         reinterpret_cast<element_type*>(buffer_info.ptr),
//                         emu::mv(extents)
//                     );
//                 });
//             }).map([&](auto && new_value){
//                 value = emu::mv(new_value);
//                 return !PyErr_Occurred();
//             })
//             .value_or(false);
//         }

//         static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
//             return bia::create_handle(value).inc_ref();
//         }
//     };

// } // namespace detail

// } // namespace pybind11
