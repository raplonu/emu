#include <gtest/gtest.h>

#include <emu/pybind11.h>

#include <pybind11/embed.h>

#include <string>

namespace py = pybind11;
using namespace py::literals;
using namespace std::string_literals;

namespace
{
    TEST(Pybind11Test, FormatIntZero)
    {
        py::scoped_interpreter guard{}; // start the interpreter and keep it alive

        py::int_ i(0);

        static_assert(std::is_base_of_v<pybind11::handle, py::int_>, "TOTO");

        {
            // No formatter.
            auto s = fmt::format("{}", i);
            EXPECT_EQ(s, "0"s);
            EXPECT_EQ(s.size(), 1);
            EXPECT_EQ(std::stoi(s), 0);
        }

        // {
        //     // Emtpy formatter.
        //     auto s = fmt::format("{:}", i);
        //     EXPECT_EQ(s, "0"s);
        //     EXPECT_EQ(s.size(), 1);
        //     EXPECT_EQ(std::stoi(s), 0);
        // }

        // {
        //     // `width = 4` formatter.
        //     auto s = fmt::format("{:4}", i);
        //     EXPECT_EQ(s, "   0"s);
        //     EXPECT_EQ(s.size(), 4);
        //     EXPECT_EQ(std::stoi(s), 0);
        // }


        // {
        //     // `fill with 0` and `width = 4` formatter.
        //     auto s = fmt::format("{:04}", i);
        //     EXPECT_EQ(s, "0000"s);
        //     EXPECT_EQ(s.size(), 4);
        //     EXPECT_EQ(std::stoi(s), 0);
        // }
    }

}

