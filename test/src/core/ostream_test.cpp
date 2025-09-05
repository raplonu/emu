#include <gtest/gtest.h>

#include <emu/ostream.hpp>

using namespace emu;

struct TestType {
    int a = 42;
    float b = 3.14f;
};

template<>
struct fmt::formatter<TestType> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }
    template<typename FormatContext>
    auto format(const TestType& t, FormatContext& ctx) const {
        return format_to(ctx.out(), "TestType{{ a: {}, b: {} }}", t.a, t.b);
    }
};

namespace
{
    TEST(OstreamTest, CustomType) {
        TestType t;
        std::ostringstream oss;
        oss << emu::as_fmt(t);
        EXPECT_EQ(oss.str(), "TestType{ a: 42, b: 3.14 }");
    }

    TEST(OstreamTest, StandardTypeNoCollision) {
        std::ostringstream oss;
        oss << emu::as_fmt(123) << " " << 4.56 << " " << emu::as_fmt('c') << " " << std::string("hello");
        EXPECT_EQ(oss.str(), "123 4.56 c hello");
    }

} // namespace
