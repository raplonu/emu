#include <gtest/gtest.h>

#include <emu/span.h>

#include <array>
#include <vector>

struct MeVector {
    std::array<int, 5> arr;

    // MeVector() = default;

    int* data() {
        return arr.data();
    }

    const int* data() const {
        return arr.data();
    }

    std::size_t size() const {
        return arr.size();
    }

};

struct MeRange {
    std::array<int, 5> arr;

    MeRange() = default;

    int* begin() {
        return arr.data();
    }

    const int* begin() const {
        return arr.data();
    }

    int* end() {
        return arr.data() + 5;
    }

    const int* end() const {
        return arr.data() + 5;
    }

};

namespace
{
    TEST(SpanCreateTest, EmptyPtr)
    {
        std::array<int, 0> arr;

        auto span1 = emu::span::create(arr.data(), arr.size());

        EXPECT_EQ(span1.data(), arr.data());
        EXPECT_EQ(span1.size(), arr.size());

        auto span2 = emu::span::create(arr.data(), arr.data() + arr.size());

        EXPECT_EQ(span2.data(), arr.data());
        EXPECT_EQ(span2.size(), arr.size());
    }

    TEST(SpanCreateTest, NonEmptyPtr)
    {
        std::array<int, 5> arr = {1,2,3,4,5};

        auto span1 = emu::span::create(arr.data(), arr.size());

        EXPECT_EQ(span1.data(), arr.data());
        EXPECT_EQ(span1.size(), arr.size());

        auto span2 = emu::span::create(arr.data(), arr.data() + arr.size());

        EXPECT_EQ(span2.data(), arr.data());
        EXPECT_EQ(span2.size(), arr.size());
    }

    TEST(SpanCreateTest, NonEmptyConstPtr)
    {
        const std::array<int, 5> arr = {1,2,3,4,5};

        auto span1 = emu::span::create(arr.data(), arr.size());

        EXPECT_EQ(span1.data(), arr.data());
        EXPECT_EQ(span1.size(), arr.size());

        auto span2 = emu::span::create(arr.data(), arr.data() + arr.size());

        EXPECT_EQ(span2.data(), arr.data());
        EXPECT_EQ(span2.size(), arr.size());
    }

    TEST(SpanCreateTest, EmptyArray)
    {
        std::array<int, 0> arr;

        auto span = emu::span::create(arr);

        EXPECT_EQ(span.data(), arr.data());
        EXPECT_EQ(span.size(), arr.size());
    }

    TEST(SpanCreateTest, NonEmptyArray)
    {
        std::array<int, 5> arr = {1,2,3,4,5};

        auto span = emu::span::create(arr);

        EXPECT_EQ(span.data(), arr.data());
        EXPECT_EQ(span.size(), arr.size());
    }

    TEST(SpanCreateTest, NonEmptyConstArray)
    {
        const std::array<int, 5> arr = {1,2,3,4,5};

        auto span = emu::span::create(arr);

        EXPECT_EQ(span.data(), arr.data());
        EXPECT_EQ(span.size(), arr.size());
    }

    TEST(SpanCreateTest, NonEmptyStaticArray)
    {
        std::array<int, 5> arr = {1,2,3,4,5};

        auto span = emu::span::create<5>(arr);

        EXPECT_EQ(span.data(), arr.data());
        EXPECT_EQ(span.size(), arr.size());
    }

    TEST(SpanCreateTest, NonEmptyConstStaticArray)
    {
        const std::array<int, 5> arr = {1,2,3,4,5};

        auto span = emu::span::create<5>(arr);

        EXPECT_EQ(span.data(), arr.data());
        EXPECT_EQ(span.size(), arr.size());
    }

    TEST(SpanCreateTest, EmptyVector)
    {
        std::vector<int> vec;

        auto span = emu::span::create(vec);

        EXPECT_EQ(span.data(), vec.data());
        EXPECT_EQ(span.size(), vec.size());
    }

    TEST(SpanCreateTest, NonEmptyVector)
    {
        std::vector<int> vec = {1,2,3,4,5};

        auto span = emu::span::create(vec);

        EXPECT_EQ(span.data(), vec.data());
        EXPECT_EQ(span.size(), vec.size());
    }

    TEST(SpanCreateTest, NonEmptyConstVector)
    {
        const std::vector<int> vec = {1,2,3,4,5};

        auto span = emu::span::create(vec);

        EXPECT_EQ(span.data(), vec.data());
        EXPECT_EQ(span.size(), vec.size());
    }

    TEST(SpanCreateTest, NonEmptyMeVector)
    {
        MeVector vec;

        auto span = emu::span::create(vec);

        EXPECT_EQ(span.data(), vec.data());
        EXPECT_EQ(span.size(), vec.size());
    }

    TEST(SpanCreateTest, NonEmptyConstMeVector)
    {
        const MeVector vec = MeVector();

        auto span = emu::span::create(vec);

        EXPECT_EQ(span.data(), vec.data());
        EXPECT_EQ(span.size(), vec.size());
    }

    TEST(SpanCreateTest, NonEmptyMeRange)
    {
        MeRange rgn;

        auto span = emu::span::create(rgn);

        EXPECT_EQ(*span.begin(), *rgn.begin());
        EXPECT_EQ(span.size(), 5);
    }

    TEST(SpanCreateTest, NonEmptyConstMeRange)
    {
        const MeRange rgn = MeRange();

        auto span = emu::span::create(rgn);

        EXPECT_EQ(*span.begin(), *rgn.begin());
        EXPECT_EQ(span.size(), 5);
    }

    TEST(SpanCreateTest, VectorToStaticSpan)
    {
        std::vector<int> vec = {1,2,3,4,5};

        auto span = emu::span::create(vec).subspan<0,5>();

        EXPECT_EQ(*span.begin(), *vec.begin());
        EXPECT_EQ(span.size(), 5);
    }

}
