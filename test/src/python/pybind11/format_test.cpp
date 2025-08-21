#include <gtest/gtest.h>
#include <emu/pybind11/format.hpp>

#include <pybind11/embed.h> // everything needed for embedding

namespace py = pybind11;
using namespace pybind11::literals;

namespace {

    TEST(Pybind11Format, BuiltinsFormating)
    {

        {
            auto formatted = fmt::format("{}", py::int_(42));

            EXPECT_EQ(formatted, "42");
        }
        {
            auto formatted = fmt::format("{}", py::float_(3.14));

            EXPECT_EQ(formatted, "3.14");
        }
        {
            auto formatted = fmt::format("{}", py::str("Hello, World!"));

            EXPECT_EQ(formatted, "Hello, World!");
        }
        {
            auto formatted = fmt::format("{}", py::dict("key"_a="value"));

            EXPECT_EQ(formatted, "{'key': 'value'}");
        }

    }

    TEST(Pybind11Format, BuiltinsFormatingWithSpec)
    {

        {
            auto formatted = fmt::format("{:05d}", py::int_(42));

            EXPECT_EQ(formatted, "00042");
        }
        {
            auto formatted = fmt::format("{:.2f}", py::float_(3.14159));

            EXPECT_EQ(formatted, "3.14");
        }
        {
            auto formatted = fmt::format("{:>7}", py::str("Hello"));

            EXPECT_EQ(formatted, "  Hello");
        }

    }

    TEST(Pybind11Format, TypeOjectFormatting)
    {

        {
            auto formatted = fmt::format("{}", py::type::of(py::int_(42)));

            EXPECT_EQ(formatted, "<class 'int'>");
        }
        {
            auto formatted = fmt::format("{}", py::type::of(py::str()));

            EXPECT_EQ(formatted, "<class 'str'>");
        }
        {
            auto formatted = fmt::format("{}", py::type::of(py::dict()));

            EXPECT_EQ(formatted, "<class 'dict'>");
        }

    }

    TEST(Pybind11Format, ModulePathFormatting)
    {

        {
            auto formatted = fmt::format("{}", py::type::of(py::str()).attr("__module__"));

            EXPECT_EQ(formatted, "builtins");
        }

    }


} // namespace