#ifndef EMU_WRAP_CAST_SPAN_H
#define EMU_WRAP_CAST_SPAN_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <emu/span.h>
#if EMU_CUDA
#include <emu/device_span.h>
#endif

namespace emu
{

namespace cast
{

    /**
     * @brief [WIP] Provide a common interface to convert from python type to cpp type and the other way round.
     *
     * @tparam T The C++ type to cast.
     */
    template<typename T>
    struct python_convertion_adaptor {
        using cpp_type = T;
        using python_type = pybind11::handle;

    };

    /**
     * @brief Specialization for emu::span_t type.
     *
     * @tparam ElementType
     * @tparam Extent
     */
    template<typename ElementType, std::size_t Extent>
    struct python_convertion_adaptor<span_t<ElementType, Extent>>
    {

        using cpp_type = span_t<ElementType, Extent>;
        using python_type = pybind11::array_t<ElementType>;
        static constexpr auto descr = pybind11::detail::_("span");

        static bool python_convertible(pybind11::handle handle) {
            EMU_TRUE_OR_RETURN_FALSE(pybind11::hasattr(handle, "__array_interface__"));
            auto dtype_src = handle.attr("dtype").cast<pybind11::dtype>();
            auto dtype_dst = pybind11::dtype::of<ElementType>();
            EMU_TRUE_OR_RETURN_FALSE(dtype_src == dtype_dst);
            return true;
        }

        static python_type convert_to_python(pybind11::handle handle) {
            return python_type::ensure(handle);
        }

        static bool cpp_convertible(const python_type & value) {
            EMU_TRUE_OR_RETURN_FALSE(value.ndim() == 1);
            EMU_TRUE_OR_RETURN_FALSE(value.strides(0) == sizeof(ElementType));

            return true;
        }

        static auto convert(python_type & value) {
            return cpp_type(value.mutable_data(), value.size());
        }

        static auto convert(const cpp_type & value) {
            pybind11::str dummyDataOwner;
            return python_type{{value.size()}, value.data(), dummyDataOwner}.inc_ref();
        }

    };

#if EMU_CUDA

namespace detail
{
    struct cupy_module : pybind11::module_
    {
        using base_t = pybind11::module_;

        pybind11::module_ cuda;

    private:

        cupy_module():
            base_t(pybind11::module_::import("cupy")),
            cuda(attr("cuda"))
        {}

    public:

        static cupy_module& instance() {
            static cupy_module instance;
            return instance;
        }
    };

    bool is_cupy_array(pybind11::handle obj) {
        auto& cp = cupy_module::instance();

        auto type = pybind11::type::of(obj);

        return type.is(cp.attr("ndarray"));
    }

} // namespace detail


    template<typename T>
    struct device_python_array : pybind11::object
    {
        using base_t = pybind11::object;

        using base_t::base_t;

        device_python_array(pybind11::object obj): base_t(obj) {}

        static device_python_array ensure(pybind11::handle h) {
            auto result = pybind11::reinterpret_steal<device_python_array>(h.ptr());
            if (!result)
                PyErr_Clear();
            return result;
        }

        std::size_t ndim() const {
            return attr("ndim").cast<std::size_t>();
        }

        std::size_t shape(std::size_t dim) const {
            return attr("shape").attr("__getitem__")(dim).cast<std::size_t>();
        }

        int strides(std::size_t dim) const {
            return attr("strides").attr("__getitem__")(dim).cast<int>();
        }

        std::size_t size() const {
            return attr("size").cast<std::size_t>();
        }

        T* mutable_data() {
            return reinterpret_cast<T*>(attr("data").attr("ptr").cast<std::uintptr_t>());
        }

        const T* data() const {
            return reinterpret_cast<const T*>(attr("data").attr("ptr").cast<std::uintptr_t>());
        }

        cuda::device_t device_id() const {
            return cuda::device::get(attr("device").attr("id").cast<std::size_t>());
        }

    };

    template<typename ElementType, std::size_t Extent>
    struct python_convertion_adaptor<device_span_t<ElementType, Extent>> {

        using cpp_type = device_span_t<ElementType, Extent>;
        using python_type = device_python_array<ElementType>;
        static constexpr auto descr = pybind11::detail::_("device span");


        static bool python_convertible(pybind11::handle handle) {
            EMU_TRUE_OR_RETURN_FALSE(detail::is_cupy_array(handle));
            auto dtype_src = handle.attr("dtype").cast<pybind11::dtype>();
            auto dtype_dst = pybind11::dtype::of<ElementType>();
            EMU_TRUE_OR_RETURN_FALSE(dtype_src == dtype_dst);
            // EMU_TRUE_OR_RETURN_FALSE(pybind11::hasattr(handle, "__cuda_array_interface__"));

            return true;
        }

        static python_type convert_to_python(pybind11::handle handle) {
            return python_type::ensure(handle);
        }

        static bool cpp_convertible(const python_type & value) {
            EMU_TRUE_OR_RETURN_FALSE(value.ndim() == 1);
            EMU_TRUE_OR_RETURN_FALSE(value.strides(0) == sizeof(ElementType));

            return true;
        }

        static auto convert(python_type & value) {
            return cpp_type(value.mutable_data(), value.size(), location::cuda_t(value.device_id()));
        }

        static auto convert(const cpp_type & value) {
            using namespace pybind11::literals; // to bring in the `_a` literal

            auto & cp = detail::cupy_module::instance();

            auto ptr = reinterpret_cast<std::uintptr_t>(value.data());

            auto memory     = cp.cuda.attr("UnownedMemory")(ptr, value.size() * sizeof(ElementType), 0, value.location.device.id());
            auto memory_ptr = cp.cuda.attr("MemoryPointer")(memory, 0);

            return python_type(cp.attr("ndarray")(value.size(), "dtype"_a=pybind11::format_descriptor<ElementType>::format(), "memptr"_a=memory_ptr)).inc_ref();
        }


    };

#endif

} // namespace span

} // namespace emu

namespace pybind11
{

namespace detail
{

    template <typename ElementType, typename Location, std::size_t Extent>
    struct type_caster< emu::detail::span_t<ElementType, Location, Extent> >
    {
        using adaptor     = emu::cast::python_convertion_adaptor<emu::detail::span_t<ElementType, Location, Extent>>;
        using cpp_type    = typename adaptor::cpp_type;
        using python_type = typename adaptor::python_type;

        /**
         * This macro establishes the name 'Toto' in
         * function signatures and declares a local variable
         * 'value' of type Toto
         */
        PYBIND11_TYPE_CASTER(cpp_type, adaptor::descr);

        /**
         * Conversion part 1 (Python->C++): convert a PyObject into a Toto
         * instance or return false upon failure. The second argument
         * indicates whether implicit conversions should be applied.
         */
        bool load(handle src, bool) {
            EMU_TRUE_OR_RETURN_FALSE(adaptor::python_convertible(src));

            auto tmp = adaptor::convert_to_python(src);

            EMU_TRUE_OR_RETURN_FALSE(adaptor::cpp_convertible(tmp));

            value = adaptor::convert(tmp);

            /* Ensure return code was OK (to avoid out-of-range errors etc) */
            return !PyErr_Occurred();
        }

        /**
         * Conversion part 2 (C++ -> Python): convert an inty instance into
         * a Python object. The second and third arguments are used to
         * indicate the return value policy and parent object (for
         * ``return_value_policy::reference_internal``) and are generally
         * ignored by implicit casters.
         */
        static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            return adaptor::convert(value);
        }
    };

} // namespace detail

} // namespace pybind11

#endif //EMU_WRAP_CAST_SPAN_H