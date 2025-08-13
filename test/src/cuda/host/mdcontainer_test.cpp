#include <gtest/gtest.h>
#include <emu/cuda/host/mdcontainer.hpp>
#include <emu/cuda.hpp>

namespace
{
    template<typename T, size_t... Exts>
    void test_make_simple(){
        constexpr std::size_t size = (1 * ... * Exts);
        auto con = emu::cuda::host::make_mdcontainer<T>(Exts...);

        EXPECT_NE(con.data_handle(), nullptr);
        EXPECT_EQ(con.size(), size);
    }

    TEST(MDContainer, AllocateSimple)
    {
        test_make_simple<int, 3, 4>();
        test_make_simple<double, 10>();
        test_make_simple<double, 10, 20>();
    }

} // namespace
