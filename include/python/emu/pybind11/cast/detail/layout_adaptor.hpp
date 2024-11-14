#pragma once

#include <emu/macro.hpp>
#include <emu/mdspan.hpp>
#include <emu/optional.hpp>
#include <emu/pybind11.hpp>

#include <pybind11/numpy.h>

namespace emu::cast::detail
{
    namespace py = ::pybind11;

    template<typename ElementType, typename Extents, typename LayoutPolicy>
    struct layout_adaptor;

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, layout_right>
    {
      using mapping_type = typename layout_right::template mapping<Extents>;

        static optional<mapping_type> mapping_from(const py::buffer_info& buffer_info) noexcept
        {
            std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.rbegin();

            std::size_t last_extent = 1;
            std::size_t last_stride = sizeof(ElementType);
            for(int i = Extents::rank() - 1; i >= 0; --i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == static_cast<size_t>(buffer_info.strides[i]));
                last_extent = buffer_info.shape[i];
                last_stride = buffer_info.strides[i];
                if(Extents::static_extent(i) == std::dynamic_extent)
                    *(it_shape++) = buffer_info.shape[i];
            }

            return mapping_type{Extents{dynamic_shape}};
        }

        static optional<mapping_type> mapping_from(py::dict array_interface) noexcept
        {
            std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.rbegin();

            std::size_t last_extent = 1;
            std::size_t last_stride = sizeof(ElementType);

            auto shape = array_interface["shape"].cast<py::list>();

            py::list strides;

            if (auto s = pybind11::try_attr(array_interface, "strides"); s)
                strides = s->cast<py::list>();


            for(int i = Extents::rank() - 1; i >= 0; --i) {
                auto current_shape = shape[i].cast<size_t>();

                if (strides.size() != 0) {
                    auto current_stride = shape[i].cast<size_t>();
                    EMU_TRUE_OR_RETURN_NULLOPT(current_stride > 0);
                    EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == static_cast<size_t>(current_stride));
                    last_extent = current_shape;
                    last_stride = current_stride;
                }

                if(Extents::static_extent(i) == std::dynamic_extent)
                    *(it_shape++) = current_shape;
            }

            return mapping_type{Extents{dynamic_shape}};
        }
    };

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, layout_left>
    {
        using mapping_type = typename layout_left::template mapping<Extents>;

        static optional<mapping_type> mapping_from(const py::buffer_info& buffer_info) noexcept
        {
            std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.begin();

            std::size_t last_extent = 1;
            std::size_t last_stride = sizeof(ElementType);
            for(auto i = 0; i < Extents::rank(); ++i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == buffer_info.strides[i]);
                last_extent = buffer_info.shape[i];
                last_stride = buffer_info.strides[i];
                if(Extents::static_extent(i) == std::dynamic_extent)
                    *(it_shape++) = buffer_info.shape[i];
            }

            return mapping_type{Extents{dynamic_shape}};
        }

        static optional<mapping_type> mapping_from(py::dict array_interface) noexcept
        {
            //TODO: implement
            fmt::print(stderr, "WARNING: {} not implemented!\n", __PRETTY_FUNCTION__);
            return nullopt;
            // std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            // auto it_shape = dynamic_shape.begin();

            // std::size_t last_extent = 1;
            // std::size_t last_stride = sizeof(ElementType);
            // for(auto i = 0; i < Extents::rank(); ++i) {
            //     EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
            //     EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == buffer_info.strides[i]);
            //     last_extent = buffer_info.shape[i];
            //     last_stride = buffer_info.strides[i];
            //     if(Extents::static_extent(i) == std::dynamic_extent)
            //         *(it_shape++) = buffer_info.shape[i];
            // }

            // return mapping_type{Extents{dynamic_shape}};
        }
    };

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, layout_stride>
    {
        using mapping_type = typename layout_stride::template mapping<Extents>;

        static optional<mapping_type> mapping_from(const py::buffer_info& buffer_info) noexcept
        {
            std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            std::array<std::size_t, Extents::rank()> dynamic_strides;
            auto it_shape = dynamic_shape.rbegin();

            for(auto i = 0; i < Extents::rank(); ++i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                if(Extents::static_extent(i) == std::dynamic_extent)
                    *(it_shape++) = buffer_info.shape[i];
                dynamic_strides[i] = buffer_info.strides[i] / sizeof(ElementType);
            }

            return mapping_type{Extents{dynamic_shape}, dynamic_strides};
        }

        static optional<mapping_type> mapping_from(py::dict array_interface) noexcept
        {
            //TODO: implement
            fmt::print(stderr, "WARNING: {} not implemented!\n", __PRETTY_FUNCTION__);

            return nullopt;
            // std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            // std::array<std::size_t, Extents::rank()> dynamic_strides;
            // auto it_shape = dynamic_shape.rbegin();

            // for(auto i = 0; i < Extents::rank(); ++i) {
            //     EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
            //     if(Extents::static_extent(i) == std::dynamic_extent)
            //         *(it_shape++) = buffer_info.shape[i];
            //     dynamic_strides[i] = buffer_info.strides[i] / sizeof(ElementType);
            // }

            // return mapping_type{Extents{dynamic_shape}, dynamic_strides};
        }
    };


} // namespace emu::cast::detail
