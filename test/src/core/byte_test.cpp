#include <gtest/gtest.h>

#include <emu/byte.h>

using namespace std;
using namespace gsl;

namespace
{
int modify_both(emu::byte& b, int& i)
{
    i = 10;
    b = to_byte<5>();
    return i;
}

TEST(byte_tests, construction)
{
    {
        const gsl::byte b = static_cast<emu::byte>(4);
        EXPECT_TRUE(static_cast<unsigned char>(b) == 4);
    }

    GSL_SUPPRESS(es.49)
    {
        const emu::byte b = emu::byte(12);
        EXPECT_TRUE(static_cast<unsigned char>(b) == 12);
    }

    {
        const emu::byte b = to_byte<12>();
        EXPECT_TRUE(static_cast<unsigned char>(b) == 12);
    }
    {
        const unsigned char uc = 12;
        const emu::byte b = to_byte(uc);
        EXPECT_TRUE(static_cast<unsigned char>(b) == 12);
    }

#if defined(__cplusplus) && (__cplusplus >= 201703L)
    {
        const emu::byte b { 14 };
        EXPECT_TRUE(static_cast<unsigned char>(b) == 14);
    }
#endif
}

TEST(byte_tests, bitwise_operations)
{
    const emu::byte b = to_byte<0xFF>();

    emu::byte a = to_byte<0x00>();
    EXPECT_TRUE((b | a) == to_byte<0xFF>());
    EXPECT_TRUE(a == to_byte<0x00>());

    a |= b;
    EXPECT_TRUE(a == to_byte<0xFF>());

    a = to_byte<0x01>();
    EXPECT_TRUE((b & a) == to_byte<0x01>());

    a &= b;
    EXPECT_TRUE(a == to_byte<0x01>());

    EXPECT_TRUE((b ^ a) == to_byte<0xFE>());

    EXPECT_TRUE(a == to_byte<0x01>());
    a ^= b;
    EXPECT_TRUE(a == to_byte<0xFE>());

    a = to_byte<0x01>();
    EXPECT_TRUE(~a == to_byte<0xFE>());

    a = to_byte<0xFF>();
    EXPECT_TRUE((a << 4) == to_byte<0xF0>());
    EXPECT_TRUE((a >> 4) == to_byte<0x0F>());

    a <<= 4;
    EXPECT_TRUE(a == to_byte<0xF0>());
    a >>= 4;
    EXPECT_TRUE(a == to_byte<0x0F>());
}

TEST(byte_tests, to_integer)
{
    const emu::byte b = to_byte<0x12>();

    EXPECT_TRUE(0x12 == gsl::to_integer<char>(b));
    EXPECT_TRUE(0x12 == gsl::to_integer<short>(b));
    EXPECT_TRUE(0x12 == gsl::to_integer<long>(b));
    EXPECT_TRUE(0x12 == gsl::to_integer<long long>(b));

    EXPECT_TRUE(0x12 == gsl::to_integer<unsigned char>(b));
    EXPECT_TRUE(0x12 == gsl::to_integer<unsigned short>(b));
    EXPECT_TRUE(0x12 == gsl::to_integer<unsigned long>(b));
    EXPECT_TRUE(0x12 == gsl::to_integer<unsigned long long>(b));

    //      EXPECT_TRUE(0x12 == gsl::to_integer<float>(b));   // expect compile-time error
    //      EXPECT_TRUE(0x12 == gsl::to_integer<double>(b));  // expect compile-time error
}

TEST(byte_tests, aliasing)
{
    int i{0};
    const int res = modify_both(reinterpret_cast<emu::byte&>(i), i);
    EXPECT_TRUE(res == i);
}

}

#ifdef CONFIRM_COMPILATION_ERRORS
copy(src_span_static, dst_span_static);
#endif
