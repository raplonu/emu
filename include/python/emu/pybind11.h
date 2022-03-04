#ifndef EMU_PYBIND_H
#define EMU_PYBIND_H

#include <emu/type_traits.h>
#include <emu/optional.h>

#include <pybind11/pybind11.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace emu
{

namespace pybind11
{

    namespace py = ::pybind11;

    template<typename T>
    bool can_cast(py::handle handle) {
        return py::detail::make_caster<T>().load(handle, true);
    }

    template<typename T>
    optional_t<T> try_cast(py::handle handle) {
        // Implementation directly comes from
        // https://github.com/pybind/pybind11/blob/master/include/pybind11/cast.h
        using namespace py::detail;

        static_assert(!cast_is_temporary_value_reference<T>::value,
            "Unable to cast type to reference: value is local to type caster");

        make_caster<T> conv;

        if(conv.load(handle, true))
            return cast_op<T>(conv);
        else
            return nullopt;
    }

    inline optional_t<py::handle> not_none(py::handle handle) {
        if (handle.is_none())
            return nullopt;
        else
            return handle;
    }

} // namespace pybind11

} // namespace emu

FMT_BEGIN_NAMESPACE

/**
 * @brief Forward every object that inherit pybind11::handle to pybind11::str::format.
 *
 * Note: Since handle provide a range interface. We need to explicitly declare it and
 * its inherited classes not has a range.
 *
 * @tparam T
 * @tparam Char
 */
template <typename T, typename Char>
struct formatter<T, Char, emu::EnableIf< std::is_base_of_v<pybind11::handle, T>, void > > {

    std::string format_ = "{}";

    auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();

        // No formating, keep the default format.
        if((end - it) <= 1)
            return it;

        format_ = fmt::format("{{:{}", fmt::join(ctx, ""));

        return --end;
    }

    template <typename FormatContext>
    auto format(const pybind11::handle& obj, FormatContext& ctx) {
        return format_to(ctx.out(), std::string(pybind11::str(format_).format(obj)));
    }
};

FMT_END_NAMESPACE

FMT_BEGIN_NAMESPACE

    // Specialization for each pybind11 type to never be treated as a range by fmt.

    template <typename Char> struct is_range<pybind11::handle  , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::object  , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::bool_   , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::int_    , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::float_  , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::str     , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::bytes   , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::tuple   , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::list    , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::dict    , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::slice   , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::none    , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::capsule , Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::iterable, Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::iterator, Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::function, Char>: std::false_type {};
    template <typename Char> struct is_range<pybind11::buffer  , Char>: std::false_type {};

FMT_END_NAMESPACE

// Note: This code is supposed to disable range formating for any pybind11 types.
// But it fail since it does not adds any level of template specialization.
// see: https://en.cppreference.com/w/cpp/language/partial_specialization#Partial_ordering

// FMT_BEGIN_NAMESPACE

// namespace detail
// {

//     template<typename T>
//     struct is_range_<T, emu::EnableIf< std::is_base_of_v<pybind11::handle, T> >> : std::false_type { };

// } // namespace detail

// FMT_END_NAMESPACE

#endif //EMU_PYBIND_H