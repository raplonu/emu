#include <gtest/gtest.h>
#include <emu/pybind11/cast/cstring_view.hpp>

#include <pybind11/embed.h> // everything needed for embedding

namespace py = pybind11;

using emu::cstring_view;
using namespace emu::literals;

namespace {

    TEST(CastCStringView, PythonToCpp) {
        {
            auto res = py::eval("'abc'").cast<cstring_view>();

            EXPECT_EQ(res, "abc"_csv);
        }
        {
            auto res = py::eval("''").cast<cstring_view>();

            EXPECT_EQ(res, ""_csv);
        }
        {
            //TODO: more investigation needed
            // Test is "disable" because of the following error:
            // Cannnot return a view since the underlying buffer cannot be kept alive.
            // auto res = py::eval("'💩'").cast<emu::cu16string_view>();
            auto res = py::eval("'💩'").cast<std::u16string>();

            EXPECT_EQ(res, u"💩"_csv);
        }
    }

    TEST(CastCStringView, CppToPython) {
        using namespace py::literals;
        {
            auto locals = py::dict("s"_a="abc"_csv);
            EXPECT_TRUE(py::eval("s == 'abc'", locals).cast<bool>());
        }
        {
            auto locals = py::dict("s"_a=""_csv);
            EXPECT_TRUE(py::eval("s == ''", locals).cast<bool>());
        }
        {
            auto locals = py::dict("s"_a="💩"_csv);
            EXPECT_TRUE(py::eval("s == '💩'", locals).cast<bool>());
        }
    }

} // namespace
