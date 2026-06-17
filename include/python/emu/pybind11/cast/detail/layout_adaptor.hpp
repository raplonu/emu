#pragma once

#include <emu/macro.hpp>
#include <emu/mdspan.hpp>
#include <emu/optional.hpp>
#include <emu/pybind11.hpp>
#include <emu/pybind11/format.hpp>

#include <pybind11/numpy.h>


namespace emu::cast::detail
{
    namespace py = ::pybind11;

#define EMU_ASSIGN_EXTENT_IF_DYNAMIC_OR_CHECK_STATIC(static_extent, input_value, output_value) \
    do {                                                                                       \
        auto extent_value = static_cast<size_t>(input_value);                                  \
        if(static_extent == dynamic_extent) {                                                  \
            output_value = extent_value;                                                       \
        } else {                                                                               \
            EMU_TRUE_OR_RETURN_NULLOPT(static_extent == extent_value);                         \
        }                                                                                      \
    } while(0)

    template<typename ElementType, typename Extents, typename LayoutPolicy>
    struct layout_adaptor;

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, layout_right>
    {
      using mapping_type = typename layout_right::template mapping<Extents>;

        static optional<mapping_type> mapping_from(const py::buffer_info& buffer_info) noexcept
        {
            std::array<size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.rbegin();

            size_t last_extent = 1;
            size_t last_stride = sizeof(ElementType);
            for(int i = Extents::rank() - 1; i >= 0; --i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == static_cast<size_t>(buffer_info.strides[i]));

                last_extent = buffer_info.shape[i];
                last_stride = buffer_info.strides[i];

                EMU_ASSIGN_EXTENT_IF_DYNAMIC_OR_CHECK_STATIC(Extents::static_extent(i), buffer_info.shape[i], *(it_shape++));
            }

            return mapping_type{Extents{dynamic_shape}};
        }

        static optional<mapping_type> mapping_from(py::dict array_interface) noexcept
        {
            std::array<size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.rbegin();

            auto shape = array_interface["shape"].cast<py::sequence>();

            py::sequence strides;
            size_t stride_size = 0;

            if (array_interface.contains("strides"))
                if (py::object s = array_interface["strides"]; !s.is_none()) {
                    strides = s.cast<py::sequence>();
                    stride_size = strides.size();
                }

            EMU_TRUE_OR_RETURN_NULLOPT(stride_size == Extents::rank() or stride_size == 0);

            size_t last_extent = 1;
            size_t last_stride = sizeof(ElementType);

            for(int i = Extents::rank() - 1; i >= 0; --i) {
                auto current_shape = shape[i].cast<size_t>();

                if (stride_size != 0) {
                    auto current_stride = strides[i].cast<size_t>();

                    EMU_TRUE_OR_RETURN_NULLOPT(current_stride > 0);
                    EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == static_cast<size_t>(current_stride));

                    last_extent = current_shape;
                    last_stride = current_stride;
                }

                EMU_ASSIGN_EXTENT_IF_DYNAMIC_OR_CHECK_STATIC(Extents::static_extent(i), current_shape, *(it_shape++));
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
            std::array<size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.begin();

            size_t last_extent = 1;
            size_t last_stride = sizeof(ElementType);
            for(size_t i = 0; i < Extents::rank(); ++i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == static_cast<size_t>(buffer_info.strides[i]));

                last_extent = buffer_info.shape[i];
                last_stride = buffer_info.strides[i];

                EMU_ASSIGN_EXTENT_IF_DYNAMIC_OR_CHECK_STATIC(Extents::static_extent(i), buffer_info.shape[i], *(it_shape++));
            }

            return mapping_type{Extents{dynamic_shape}};
        }

        static optional<mapping_type> mapping_from(py::dict array_interface) noexcept
        {
            std::array<size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.begin();

            // shape: typically a tuple in __array_interface__
            auto shape = array_interface["shape"].cast<py::sequence>();

            // strides: may be missing or None (missing => C-contiguous in NumPy's array interface)
            py::sequence strides;
            size_t stride_size = 0;

            if (array_interface.contains("strides")) {
                if (py::object s = array_interface["strides"]; !s.is_none()) {
                    strides = s.cast<py::sequence>();
                    stride_size = strides.size();
                }
            }

            EMU_TRUE_OR_RETURN_NULLOPT(stride_size == Extents::rank() or stride_size == 0);

            //TODO: needs better review about this block:
            // If no strides are provided, NumPy specifies C-contiguous.
            // Accept only if C- and F-contiguity are equivalent (rank<=1 or degenerate extents).
            if (stride_size == 0) {
                if constexpr (Extents::rank() > 1) {
                    size_t non_ones = 0;
                    for (size_t i = 0; i < Extents::rank(); ++i) {
                        const auto e = shape[i].cast<size_t>();
                        if (e != 1) ++non_ones;
                    }
                    EMU_TRUE_OR_RETURN_NULLOPT(non_ones <= 1);
                }
            }

            size_t last_extent = 1;
            size_t last_stride = sizeof(ElementType);

            for(size_t i = 0; i < Extents::rank(); ++i) {
                const auto current_shape  = shape[i].cast<size_t>();

                if(stride_size != 0) {
                    const auto current_stride = strides[i].cast<size_t>();

                    EMU_TRUE_OR_RETURN_NULLOPT(current_stride > 0);
                    EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == current_stride);

                    last_extent = current_shape;
                    last_stride = current_stride;
                }

                EMU_ASSIGN_EXTENT_IF_DYNAMIC_OR_CHECK_STATIC(Extents::static_extent(i), current_shape, *(it_shape++));
            }

            return mapping_type{Extents{dynamic_shape}};
        }
    };

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, layout_stride>
    {
        using mapping_type = typename layout_stride::template mapping<Extents>;

        static optional<mapping_type> mapping_from(const py::buffer_info& buffer_info) noexcept
        {
            std::array<size_t, Extents::rank_dynamic()> dynamic_shape;
            std::array<size_t, Extents::rank()> dynamic_strides;
            auto it_shape = dynamic_shape.rbegin();

            for(size_t i = 0; i < Extents::rank(); ++i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);

                EMU_ASSIGN_EXTENT_IF_DYNAMIC_OR_CHECK_STATIC(Extents::static_extent(i), buffer_info.shape[i], *(it_shape++));

                dynamic_strides[i] = buffer_info.strides[i] / sizeof(ElementType);
            }

            return mapping_type{Extents{dynamic_shape}, dynamic_strides};
        }

        static optional<mapping_type> mapping_from(py::dict /* array_interface */) noexcept
        {
          constexpr size_t rank = Extents::rank();

          const auto shape = array_interface["shape"].cast<py::list>();
          const auto strides = array_interface["strides"].cast<py::list>();

          EMU_TRUE_OR_RETURN_NULLOPT(shape.size() == rank && strides.size() == rank);

          std::array<std::size_t, rank> dynamic_shape;
          std::array<std::size_t, rank> dynamic_strides;

          for(int i = rank - 1; i >= 0; --i) {
            const auto current_shape = shape[i].cast<size_t>();
            const auto current_stride = strides[i].cast<size_t>();
            EMU_TRUE_OR_RETURN_NULLOPT(current_shape > 0 && current_stride > 0);
            dynamic_shape[i] = current_shape;
            dynamic_strides[i] = current_stride / sizeof(ElementType);
          }

          return mapping_type{Extents{dynamic_shape}, dynamic_strides};
        }
    };

#undef EMU_ASSIGN_EXTENT_IF_DYNAMIC_OR_CHECK_STATIC

} // namespace emu::cast::detail
