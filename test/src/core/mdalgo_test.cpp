#include <gtest/gtest.h>

#include <emu/mdalgo.hpp>

#include <vector>
#include <array>

using namespace emu;

namespace
{
    TEST(MdAlgo, RankTest)
    {
        {
            std::array<int, 5> arr = {1, 2, 3, 4, 5};

            static_assert(rank(arr) == 1);
        }

        {
            std::vector<int> vec = {1, 2, 3, 4, 5};

            static_assert(rank(vec) == 1);
        }

        {
            int value = 5;

            static_assert(rank(value) == 0);
        }

        {
            md
        }

    }

} // namespace