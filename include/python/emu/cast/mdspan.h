#ifndef EMU_CAST_MDSPAN_H
#define EMU_CAST_MDSPAN_H

#include <emu/cast/common.h>
#include <emu/type_traits.h>
#include <emu/optional.h>
#include <emu/mdspan.h>
#if EMU_CUDA
#include <emu/cuda/mdspan.h>
#endif

#include <pybind11/numpy.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace emu
{

namespace cast
{

    template<typename ElementType, typename Extents, typename LayoutPolicy>
    struct layout_adaptor;

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, mdspan::layout_right>
    {
      using mapping_type = typename mdspan::layout_right::template mapping<Extents>;

        static emu::optional_t<mapping_type> mapping(const py::buffer_info& buffer_info) noexcept {
            std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.rbegin();

            std::size_t last_extent = 1;
            std::size_t last_stride = sizeof(ElementType);
            for(int i = Extents::rank() - 1; i >= 0; --i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == buffer_info.strides[i]);
                last_extent = buffer_info.shape[i];
                last_stride = buffer_info.strides[i];
                if(Extents::static_extent(i) == mdspan::dynamic_extent)
                    *(it_shape++) = buffer_info.shape[i];

            }

            return mapping_type{Extents{dynamic_shape}};
        }
    };

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, mdspan::layout_left>
    {
        using mapping_type = typename mdspan::layout_left::template mapping<Extents>;

        static emu::optional_t<mapping_type> mapping(const py::buffer_info& buffer_info) noexcept {
            std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            auto it_shape = dynamic_shape.begin();

            std::size_t last_extent = 1;
            std::size_t last_stride = sizeof(ElementType);
            for(auto i = 0; i < Extents::rank(); ++i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                EMU_TRUE_OR_RETURN_NULLOPT(last_stride * last_extent == buffer_info.strides[i]);
                last_extent = buffer_info.shape[i];
                last_stride = buffer_info.strides[i];
                if(Extents::static_extent(i) == mdspan::dynamic_extent)
                    *(it_shape++) = buffer_info.shape[i];
            }

            return mapping_type{Extents{dynamic_shape}};
        }
    };

    template<typename ElementType, typename Extents>
    struct layout_adaptor<ElementType, Extents, mdspan::layout_stride>
    {
        using mapping_type = typename mdspan::layout_stride::template mapping<Extents>;

        static emu::optional_t<mapping_type> mapping(const py::buffer_info& buffer_info) noexcept {
            std::array<std::size_t, Extents::rank_dynamic()> dynamic_shape;
            std::array<std::size_t, Extents::rank()> dynamic_strides;
            auto it_shape = dynamic_shape.rbegin();

            for(auto i = 0; i < Extents::rank(); ++i) {
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[i] > 0);
                if(Extents::static_extent(i) == mdspan::dynamic_extent)
                    *(it_shape++) = buffer_info.shape[i];
                dynamic_strides[i] = buffer_info.strides[i] / sizeof(ElementType);
            }

            return mapping_type{Extents{dynamic_shape}, dynamic_strides};
        }
    };

    template <typename ElementType, typename Location, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct python_adaptor<emu::mdspan::detail::mdspan_t<ElementType, Location, Extents, LayoutPolicy, AccessorPolicy>> : std::true_type
    {

        using cpp_type = emu::mdspan::detail::mdspan_t<ElementType, Location, Extents, LayoutPolicy, AccessorPolicy>;

        using bia = buffer_info_adaptor<ElementType, Location>;

        template< std::size_t I, EnableIf<cpp_type::static_extent(I) == mdspan::dynamic_extent> = true>
        static constexpr auto dim_descr() {
            using namespace pybind11::detail;
            return _("*");
        }

        template< std::size_t I, EnableIf<cpp_type::static_extent(I) != mdspan::dynamic_extent> = true>
        static constexpr auto dim_descr() {
            using namespace pybind11::detail;
            return _<cpp_type::static_extent(I)>();
        }

        template< std::size_t... Is >
        static constexpr auto shape_descr(std::index_sequence<Is...>) {
            using namespace pybind11::detail;
            return concat(dim_descr<Is>()...);
        }

        static constexpr auto descr() {
            using namespace pybind11::detail;
            return _("mdspan<") + concat(make_caster<ElementType>::name + _("[") + shape_descr(std::make_index_sequence<cpp_type::rank()>{}) + _("]"), bia::loc_descr()) + _(">");
        }

        static emu::optional_t<cpp_type> convert(pybind11::handle handle) {

            // Try cast handle into buffer.
            return bia::get_buffer_info(handle).and_then([](auto buffer_info) -> emu::optional_t<cpp_type> {
                // Must be only one dimension.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.ndim == cpp_type::rank());

                for(auto i = 0; i < cpp_type::rank(); ++i)
                    // Must be Extent if span is not dynamic.
                    EMU_TRUE_OR_RETURN_NULLOPT(cpp_type::static_extent(i) == span::dynamic_extent or buffer_info.shape[i] == cpp_type::static_extent(i));

                return layout_adaptor<ElementType, Extents, LayoutPolicy>::mapping(buffer_info).and_then([&] (auto&& extents) {
                    return emu::optional_t<cpp_type>(in_place,
                        reinterpret_cast<ElementType*>(buffer_info.ptr),
                        mv(extents),
                        bia::location(buffer_info)
                    );
                });
            });
        }

        static auto convert(const cpp_type& value) {
            return bia::create_handle(value);
        }

    };

} // namespace cast

} // namespace emu

namespace pybind11
{

namespace detail
{

    template <typename ElementType, typename Location, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct type_caster< emu::mdspan::detail::mdspan_t<ElementType, Location, Extents, LayoutPolicy, AccessorPolicy> >
    {
        using adaptor  = emu::cast::python_adaptor<emu::mdspan::detail::mdspan_t<ElementType, Location, Extents, LayoutPolicy, AccessorPolicy>>;
        using cpp_type = typename adaptor::cpp_type;

        PYBIND11_TYPE_CASTER(cpp_type, adaptor::descr());

        bool load(handle src, bool) {
            return adaptor::convert(src)
                .map([&](auto && new_value){
                    value = emu::mv(new_value);
                    return !PyErr_Occurred();
                })
                .value_or(false);
        }

        static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            return adaptor::convert(value).inc_ref();
        }
    };

} // namespace detail

} // namespace pybind11


























































// namespace emu
// {

// namespace cast
// {

// namespace detail
// {

//     template<typename Layout>
//     struct npl_impl;

//     template<> struct npl_impl<span::layout_right>  { static constexpr auto layout = pybind11::array::c_style;   };
//     template<> struct npl_impl<span::layout_left>   { static constexpr auto layout = pybind11::array::f_style;   };
//     template<> struct npl_impl<span::layout_stride> { static constexpr auto layout = pybind11::array::forcecast; };

//     template<typename Layout>
//     constexpr auto numpy_layout = npl_impl<Layout>::layout;

// } // namespace detail


//     /**
//      * @brief Specialization for emu::span_t type.
//      *
//      * @tparam ElementType
//      * @tparam Extent
//      */
//     template<typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
//     struct python_adaptor<mdspan_t<ElementType, Extents, LayoutPolicy, AccessorPolicy>>
//     {

//         using cpp_type = mdspan_t<ElementType, Extents, LayoutPolicy, AccessorPolicy>;
//         using python_type = pybind11::array_t<ElementType, detail::numpy_layout<LayoutPolicy>>;
//         static constexpr auto descr = pybind11::detail::_("mdspan");

//         static bool python_convertible(pybind11::handle handle) {
//             EMU_TRUE_OR_RETURN_FALSE(pybind11::hasattr(handle, "__array_interface__"));
//             auto dtype_src = handle.attr("dtype").cast<pybind11::dtype>();
//             auto dtype_dst = pybind11::dtype::of<ElementType>();
//             EMU_TRUE_OR_RETURN_FALSE(dtype_src == dtype_dst);
//             return true;
//         }

//         static python_type convert_to_python(pybind11::handle handle) {
//             return python_type::ensure(handle);
//         }

//         static bool cpp_convertible(const python_type & value) {
//             EMU_TRUE_OR_RETURN_FALSE(value.ndim() == cpp_type::rank());

//             for(std::size_t i = 0; i < cpp_type::rank(); ++i)
//                 if(cpp_type::static_extent(i) != emu::span::dynamic_extent)
//                     EMU_TRUE_OR_RETURN_FALSE(value.shape(i) == cpp_type::static_extent(i));
//             EMU_TRUE_OR_RETURN_FALSE(not IsConst<ElementType> and value.is_writable());

//             return true;
//         }

//         static auto convert(python_type & value) {
//             std::array<std::size_t, cpp_type::rank_dynamic()> shape;
//             auto it = shape.begin();
//             for(std::size_t i = 0; i < cpp_type::rank(); ++i)
//                 if(cpp_type::static_extent(i) != emu::span::dynamic_extent)
//                     *(it++) = value.shape(i);

//             return cpp_type(value.mutable_data(), shape);
//         }

//         static auto convert(const cpp_type & value) {
//             std::vector<std::size_t> extents; extents.reserve(cpp_type::rank());
//             std::vector<std::size_t> strides; strides.reserve(cpp_type::rank());
//             for(std::size_t i = 0; i < cpp_type::rank(); ++i) {
//                 extents.push_back(value.extent(i));
//                 strides.push_back(value.stride(i) * sizeof(ElementType));
//             }

//             pybind11::str dummyDataOwner;
//             auto res = python_type{mv(extents), mv(strides), value.data(), dummyDataOwner};

//             if (IsConst<ElementType>)
//                 reinterpret_cast<pybind11::detail::PyArray_Proxy*>(res.ptr())->flags &= ~pybind11::detail::npy_api::NPY_ARRAY_WRITEABLE_;

//             return res.inc_ref();
//         }

//     };

// // #if EMU_CUDA

// // namespace detail
// // {

// //     pybind11::module_& cupy_module();

// //     pybind11::module_& cuda_module();

// //     bool is_cupy_array(pybind11::handle obj);

// // } // namespace detail

// //     template<typename T>
// //     struct device_python_array : pybind11::object
// //     {
// //         using base_t = pybind11::object;

// //         using base_t::base_t;

// //         device_python_array(pybind11::object obj): base_t(obj) {}

// //         static device_python_array ensure(pybind11::handle h) {
// //             auto result = pybind11::reinterpret_steal<device_python_array>(h.ptr());
// //             if (!result)
// //                 PyErr_Clear();
// //             return result;
// //         }

// //         std::size_t ndim() const {
// //             return attr("ndim").cast<std::size_t>();
// //         }

// //         std::size_t shape(std::size_t dim) const {
// //             return attr("shape").attr("__getitem__")(dim).cast<std::size_t>();
// //         }

// //         int strides(std::size_t dim) const {
// //             return attr("strides").attr("__getitem__")(dim).cast<int>();
// //         }

// //         std::size_t size() const {
// //             return attr("size").cast<std::size_t>();
// //         }

// //         T* mutable_data() {
// //             return reinterpret_cast<T*>(attr("data").attr("ptr").cast<std::uintptr_t>());
// //         }

// //         const T* data() const {
// //             return reinterpret_cast<const T*>(attr("data").attr("ptr").cast<std::uintptr_t>());
// //         }

// //         cuda::device_t device_id() const {
// //             return cuda::device::get(attr("device").attr("id").cast<std::size_t>());
// //         }

// //     };

// //     template<typename ElementType, std::size_t Extent>
// //     struct python_adaptor<emu::cuda::span_t<ElementType, Extent>> {

// //         using cpp_type = emu::cuda::span_t<ElementType, Extent>;
// //         using python_type = device_python_array<ElementType>;
// //         static constexpr auto descr = pybind11::detail::_("device span");


// //         static bool python_convertible(pybind11::handle handle) {
// //             EMU_TRUE_OR_RETURN_FALSE(detail::is_cupy_array(handle));
// //             auto dtype_src = handle.attr("dtype").cast<pybind11::dtype>();
// //             auto dtype_dst = pybind11::dtype::of<ElementType>();
// //             EMU_TRUE_OR_RETURN_FALSE(dtype_src == dtype_dst);
// //             // EMU_TRUE_OR_RETURN_FALSE(pybind11::hasattr(handle, "__cuda_array_interface__"));

// //             return true;
// //         }

// //         static python_type convert_to_python(pybind11::handle handle) {
// //             return python_type::ensure(handle);
// //         }

// //         static bool cpp_convertible(const python_type & value) {
// //             EMU_TRUE_OR_RETURN_FALSE(value.ndim() == 1);
// //             EMU_TRUE_OR_RETURN_FALSE(value.strides(0) == sizeof(ElementType));

// //             return true;
// //         }

// //         static auto convert(python_type & value) {
// //             return cpp_type(value.mutable_data(), value.size(), location::cuda_t(value.device_id()));
// //         }

// //         static auto convert(const cpp_type & value) {
// //             using namespace pybind11::literals; // to bring in the `_a` literal

// //             auto ptr = reinterpret_cast<std::uintptr_t>(value.data());

// //             auto & cuda = detail::cuda_module();

// //             auto memory     = cuda.attr("UnownedMemory")(ptr, value.size() * sizeof(ElementType), 0, value.location().device.id());
// //             auto memory_ptr = cuda.attr("MemoryPointer")(memory, 0);

// //             return python_type(detail::cupy_module().attr("ndarray")(value.size(), "dtype"_a=pybind11::format_descriptor<ElementType>::format(), "memptr"_a=memory_ptr)).inc_ref();
// //         }


// //     };

// // #endif // EMU_CUDA

// } // namespace span

// } // namespace emu

// namespace pybind11
// {

// namespace detail
// {

//     template <typename ElementType, typename Location, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
//     struct type_caster< emu::span::detail::mdspan_t<ElementType, Location, Extents, LayoutPolicy, AccessorPolicy> >
//     {
//         using adaptor     = emu::cast::python_adaptor<emu::span::detail::mdspan_t<ElementType, Location, Extents, LayoutPolicy, AccessorPolicy>>;
//         using cpp_type    = typename adaptor::cpp_type;
//         using python_type = typename adaptor::python_type;

//         /**
//          * This macro establishes the name 'Toto' in
//          * function signatures and declares a local variable
//          * 'value' of type Toto
//          */
//         PYBIND11_TYPE_CASTER(cpp_type, adaptor::descr);

//         /**
//          * Conversion part 1 (Python->C++): convert a PyObject into a Toto
//          * instance or return false upon failure. The second argument
//          * indicates whether implicit conversions should be applied.
//          */
//         bool load(handle src, bool) {
//             EMU_TRUE_OR_RETURN_FALSE(adaptor::python_convertible(src));

//             auto tmp = adaptor::convert_to_python(src);

//             EMU_TRUE_OR_RETURN_FALSE(adaptor::cpp_convertible(tmp));

//             value = adaptor::convert(tmp);

//             /* Ensure return code was OK (to avoid out-of-range errors etc) */
//             return !PyErr_Occurred();
//         }

//         /**
//          * Conversion part 2 (C++ -> Python): convert an inty instance into
//          * a Python object. The second and third arguments are used to
//          * indicate the return value policy and parent object (for
//          * ``return_value_policy::reference_internal``) and are generally
//          * ignored by implicit casters.
//          */
//         static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
//             return adaptor::convert(value);
//         }
//     };

// } // namespace detail

// } // namespace pybind11

#endif //EMU_CAST_MDSPAN_H