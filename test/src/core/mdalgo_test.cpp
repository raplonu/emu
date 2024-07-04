#include <gtest/gtest.h>

//TODO: removes NOLINT when rank is implemented.
#include <emu/mdalgo.hpp> // NOLINT

#include <vector>
#include <array>

using namespace emu;

namespace
{
    TEST(MdAlgo, RankTest)
    {
        {
            const std::array<int, 5> arr = {1, 2, 3, 4, 5};

            (void)arr;
            // static_assert(rank(arr) == 1);
        }

        {
            const std::vector<int> vec = {1, 2, 3, 4, 5};

            (void)vec;
            // static_assert(rank(vec) == 1);
        }

        {
            const int value = 5;

            (void)value;
            // static_assert(rank(value) == 0);
        }

        {
            // emu::mdspan m{nullptr, 1,2,3};

            // static_assert(rank(m) == 3);
        }

    }

} // namespace
