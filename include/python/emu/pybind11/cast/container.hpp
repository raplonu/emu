#pragma once

#include <emu/mdalgo.hpp>
#include <emu/pybind11/cast/detail/capsule.hpp>
#include <emu/pybind11/cast/detail/mdspan_caster.hpp>

#include <emu/container.hpp>
#include <span>

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<emu::cpts::container Container>
    struct type_caster< Container > : emu::cast::detail::mdspan_caster< emu::md_equivalent<Container> >
    {
        using base_caster = emu::cast::detail::mdspan_caster< emu::md_equivalent<Container> >;

        using cpp_type = Container;

        using element_type  = typename cpp_type::element_type;

        static_assert(Container::extent == std::dynamic_extent, "pybind11 required only default constructible types. Fixed size container are not.");

        static constexpr auto descr() {
            return _("container<") + make_caster<element_type>::name + _(">");
        }

        PYBIND11_TYPE_CASTER(cpp_type, descr());

        bool load(handle src, bool convert) {
            return base_caster::from_python(src, convert).map([&](auto md_span){
                auto capsule = emu::pybind11::detail::handle_to_capsule(src);
                value = cpp_type(md_span.data_handle(), md_span.size(), std::move(capsule));
                return true;
            }).value_or(false);
        }

        static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            return base_caster::to_python(emu::as_md(value), emu::pybind11::detail::capsule_to_handle(value.capsule())).inc_ref();
        }
    };
} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)

//     template<typename T, std::size_t Extent>
//     struct type_caster< emu::container<T, Extent> >
//     {
//         using cpp_type = emu::container<T, Extent>;

//         static_assert(cpp_type::extent == std::dynamic_extent, "pybind11 required only default constructible types. Fixed size container are not.");

//         using element_type  = typename cpp_type::element_type;
//         using bia = emu::cast::buffer_info_adaptor<element_type>;

//         static constexpr auto descr() {
//             using namespace ::pybind11::detail;
//             return _("container<") + make_caster<element_type>::name + _("[*]>");
//         }

//         PYBIND11_TYPE_CASTER(cpp_type, descr());

//         static emu::optional<cpp_type> from_array(array_t<element_type>& array) {
//             // Must be writable if T is not const.
//             EMU_TRUE_OR_RETURN_NULLOPT(std::is_const_v<T> or array.writeable());

//             // test if base if valid and is a capsule
//             if (auto base = array.base(); base and isinstance<capsule>(base))
//                 // test if capsule points to a emu::container.
//                 if (auto cap = base.template cast<capsule>(); cap.name() == "emu_secret_container") {
//                     fmt::print("found container\n");
//                     return *cap.template get_pointer<cpp_type>();
//                 }

//             T* ptr = nullptr;
//             if constexpr (std::is_const_v<T>)
//                 ptr = array.data();
//             else
//                 ptr = array.mutable_data();

//             auto size = static_cast<std::size_t>(array.size());


//             emu::capsule capsule;
//             if (array.owndata()) {
//                 fmt::print("put array into capsule\n");
//                 capsule = std::move(array);
//             } else
//                 fmt::print("array is not owned\n");

//             return cpp_type(
//                 std::span{ptr, size},
//                 std::move(capsule)
//             );
//         }

//         static cpp_type from_buffer_info(buffer_info& buffer_info) {
//             return {reinterpret_cast<element_type*>(buffer_info.ptr), buffer_info.size};
//         }


//         static bool check_array(const array_t<element_type>& array) {
//             // Must be only one dimension.
//             EMU_TRUE_OR_RETURN_FALSE(array.ndim() == 1);

//             // Must be contigious.
//             EMU_TRUE_OR_RETURN_FALSE(array.strides()[0] == sizeof(element_type));

//             return true;
//         }

//         static bool check_buffer_info(const buffer_info& buffer_info) {
//             // Must be only one dimension.
//             EMU_TRUE_OR_RETURN_FALSE(buffer_info.ndim == 1);

//             // Must be contigious.
//             EMU_TRUE_OR_RETURN_FALSE(buffer_info.strides[0] == sizeof(element_type));

//             return true;
//         }

//         bool load(handle src, bool convert) {
//             emu::optional<cpp_type> result;

//             {
//                 auto array = bia::cast_array(src);

//                 // Only return if array is not null but check failed.
//                 EMU_TRUE_OR_RETURN_FALSE(array.map_or(check_array, true));

//                 result = array.and_then(from_array);
//                 if (result)
//                     fmt::print("from array\n");
//             }

//             if (not result) {
//                 auto buffer = bia::get_buffer_info(src);

//                 // Only return if buffer is not null but check failed.
//                 EMU_TRUE_OR_RETURN_FALSE(buffer.map_or(check_buffer_info, true));

//                 if (buffer) // weird bug here. Cannot use buffer.map...
//                     result = from_buffer_info(*buffer);

//                 if (result)
//                     fmt::print("from buffer\n");
//             }


//             if (not result) {
//                 auto new_array = bia::create_array(src);

//                 // Only return if array is not null but check failed.
//                 EMU_TRUE_OR_RETURN_FALSE(new_array.map_or(check_array, true));

//                 // calling `from_array` is overkill since it tries to get the capsule which does not exist.
//                 result = new_array.and_then(from_array);
//                 if (result)
//                     fmt::print("from NEW array\n");
//             }

//             return std::move(result)
//                 // If result owns the data, we can move it to the value.
//                 .map([&](auto&& new_value) -> bool {
//                     value = emu::mv(new_value);
//                     return !PyErr_Occurred();
//                 })
//                 .value_or(false);
//         }

//         static handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
//             capsule free_when_done(new auto(value), [](void *val) {
//                 auto value = reinterpret_cast<cpp_type*>(val);
//                 delete value;
//             });
//             free_when_done.set_name("emu_secret_container");
//             return bia::create_handle(emu::as_mdspan(value), free_when_done).inc_ref();
//         }
//     };

// } // namespace detail

// } // namespace pybind11
