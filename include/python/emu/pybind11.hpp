#pragma once

#include <emu/cstring_view.hpp>
#include <emu/type_traits.hpp>
#include <emu/optional.hpp>

#include <pybind11/pybind11.h>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <pybind11/pytypes.h>

namespace emu::pybind11
{

    namespace py = ::pybind11;

    template<typename T>
    bool can_cast(py::handle handle) {
        return py::detail::make_caster<T>().load(handle, true);
    }

    inline optional<py::detail::str_attr_accessor> try_attr(py::handle handle, cstring_view name) {
        if (py::hasattr(handle, name.c_str()))
            return handle.attr(name.c_str());

        return nullopt;
    }

    template<typename T>
    optional<T> try_cast(py::handle handle) {
        // Implementation directly comes from
        // https://github.com/pybind/pybind11/blob/master/include/pybind11/cast.h
        using namespace py::detail;

        static_assert(!cast_is_temporary_value_reference<T>::value,
            "Unable to cast type to reference: value is local to type caster");

        make_caster<T> conv;

        if(conv.load(handle, true))
            return cast_op<T>(conv);

        return nullopt;
    }

    inline optional<py::handle> not_none(py::handle handle) {
        if (handle.is_none())
            return nullopt;

        return handle;
    }

namespace cpts
{

    template<typename T>
    concept derived_from_handle = std::derived_from<T, py::handle>;

} // namespace cpts

} // namespace emu::pybind11

namespace fmt
{

// /**
//  * @brief Forward every object that inherit pybind11::handle to pybind11::str::format.
//  *
//  * Note: Since handle provide a range interface. We need to explicitly declare it and
//  * its inherited classes not has a range.
//  *
//  * @tparam T
//  * @tparam Char
//  */

    template <emu::pybind11::cpts::derived_from_handle T, typename Char>
    struct formatter<T, Char> : formatter<std::string, Char> {
        using base = fmt::formatter<std::string, Char>;

        format_parse_context::iterator it, end;

        constexpr auto parse(format_parse_context& ctx)
            -> format_parse_context::iterator
        {
            auto it = ctx.begin();

            // Special case when '{}' begin and end are equal.
            if (it == ctx.end()) {
                this->it = it;
                this->end = it;
                return it;
            }

            // Now, We know that it point to the first char after '{:'

            // Point back at '{'
            this->it = it - 2;

            // Iterate until we reach the '}'
            while (*it != '}') it++;

            // Go past it to include '}' in the range.
            this->end = it + 1;

            // Won't compile if it is not pointing to '}'
            return it;
        }

        template <typename FormatContext>
        auto format(const pybind11::handle& obj, FormatContext& ctx) const {
            std::string format_str;
            if (it != end)
                format_str = fmt::to_string(fmt::join(it, end, ""));
            else
                format_str = "{}";

            return base::format(std::string(pybind11::str(format_str).format(obj)), ctx);
        }
    };


    // Specialization for pybind11 python based type to never be treated as a range by fmt.
    template <emu::pybind11::cpts::derived_from_handle T, typename Char>
    struct is_range<T, Char>: std::false_type{};

} // namespace fmt

// FMT_END_NAMESPACE

// FMT_BEGIN_NAMESPACE

//     // Specialization for each pybind11 type to never be treated as a range by fmt.

//     template <typename Char> struct is_range<pybind11::handle  , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::object  , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::bool_   , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::int_    , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::float_  , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::str     , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::bytes   , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::tuple   , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::list    , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::dict    , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::slice   , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::none    , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::capsule , Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::iterable, Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::iterator, Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::function, Char>: std::false_type {};
//     template <typename Char> struct is_range<pybind11::buffer  , Char>: std::false_type {};

// FMT_END_NAMESPACE

// // Note: This code is supposed to disable range formating for any pybind11 types.
// // But it fail since it does not adds any level of template specialization.
// // see: https://en.cppreference.com/w/cpp/language/partial_specialization#Partial_ordering

// // FMT_BEGIN_NAMESPACE

// // namespace detail
// // {

// //     template<typename T>
// //     struct is_range_<T, emu::EnableIf< std::is_base_of_v<pybind11::handle, T> >> : std::false_type { };

// // } // namespace detail

// // FMT_END_NAMESPACE
