#include <gtest/gtest.h>

#include <emu/cast/span.h>

#include <pybind11/embed.h>

namespace py = pybind11;
using namespace py::literals;

template<typename... Args>
auto invoke_with(std::string code, Args&&... args) {
    auto locals = py::dict(EMU_FWD(args)...);
    py::exec(code, py::globals(), locals);
    return locals;
}

namespace
{
    TEST(SpanTest, SimpleSpan)
    {
        py::scoped_interpreter guard{}; // start the interpreter and keep it alive

        int data[] = {1,2,3,4,5};
        emu::span_t<int> s(data);

        EXPECT_EQ(s.size(), 5);

        auto local = invoke_with(R"(size = span.size)", "span"_a=s);
        auto res = local["size"].cast<std::size_t>();

        EXPECT_EQ(s.size(), res);
    }
}
