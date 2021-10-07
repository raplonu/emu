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


/**
 * @brief Forward every object that inherit pybind11::object to pybind11::str::format.
 *
 * Note: It would have been better to use `handle` instead of `object`. But since a handle is
 * a range of handle, it causes formating error. For now,
 *
 * @tparam T
 * @tparam Char
 */
template <typename Char>
struct fmt::formatter<pybind11::handle, Char /*, emu::EnableIf< emu::IsBaseOf<pybind11::object, T>, void > */> {

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

template <> struct fmt::formatter<pybind11::object  >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::bool_   >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::int_    >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::float_  >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::str     >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::bytes   >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::tuple   >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::list    >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::dict    >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::slice   >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::none    >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::capsule >: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::iterable>: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::iterator>: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::function>: formatter<pybind11::handle> {};
template <> struct fmt::formatter<pybind11::buffer  >: formatter<pybind11::handle> {};

// Note: This code is supposed to disable range formating for any pybind11 types.
// But it fail since it does not adds any level of template specialization.
// see: https://en.cppreference.com/w/cpp/language/partial_specialization#Partial_ordering

// FMT_BEGIN_NAMESPACE

// namespace detail
// {

//     template<typename T>
//     struct is_range_<T, emu::EnableIf< emu::IsBaseOf<pybind11::handle, T> >> : std::false_type { };

// } // namespace detail

// FMT_END_NAMESPACE

#endif //EMU_PYBIND_H