#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

#include <emu/cuda/device/container.hpp>
#include <emu/cuda/device/mdcontainer.hpp>
#include <emu/cuda.hpp>
#include <emu/scoped.hpp>

#include <emu/info.hpp>

#include <cstdlib>

namespace
{

    template<typename C>
    void check(C &container, size_t expected_size, int expected_ptr_count){
        EXPECT_EQ(container.size(), expected_size);
        EXPECT_EQ(container.use_count(), expected_ptr_count);
    }
    template<typename C, typename T>
    void check(C &container, size_t expected_size, int expected_ptr_count, const T *ptr){
        EXPECT_EQ(container.data_handle(), ptr);
        check(container, expected_size, expected_ptr_count);
    }
    template<typename C, typename A, std::size_t N>
    void check(C &container, size_t expected_size, int expected_ptr_count, const std::array<A,N> &dims){
        check(container, expected_size, expected_ptr_count);
        for(std::size_t i = 0; i < N; i++) {
            EXPECT_EQ(container.extents().extent(i), dims[i]);
        }
    }
    template<typename C, typename T, typename A, std::size_t N>
    void check(C &container, size_t expected_size, int expected_ptr_count, const T *ptr, const std::array<A,N> &dims){
        check(container, expected_size, expected_ptr_count, ptr);
        for(std::size_t i = 0; i < N; i++) {
            EXPECT_EQ(container.extents().extent(i), dims[i]);
        }
    }


    template<typename T, std::size_t D>
    void test_construct_1(){
        emu::cuda::device::mdcontainer<T, emu::dims<D>> con;
        check(con, 0, 0, (T*)nullptr);
    };

    template<typename T, size_t... Exts>
    void test_make_2(){
        auto device = emu::cuda::device::current();
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        auto con = emu::cuda::device::make_mdcontainer<T>(device, Exts...);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_3(){
        auto device = emu::cuda::device::current();
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        const emu::span sdims = dims;
        auto con = emu::cuda::device::make_mdcontainer<T>(device, sdims);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_4(){
        auto device = emu::cuda::device::current();
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        auto con = emu::cuda::device::make_mdcontainer<T>(device, dims);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_5(){
        auto device = emu::cuda::device::current();
        const int cpt = 1;
        constexpr std::size_t d = sizeof...(Exts);
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        const emu::dims<d> ext_t(Exts...);
        auto con = emu::cuda::device::make_mdcontainer<T,d>(device, ext_t);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_6(){
        auto device = emu::cuda::device::current();
        const int cpt = 1;
        constexpr std::size_t d = sizeof...(Exts);
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        using mdc = emu::cuda::device::mdcontainer<T, emu::dims<d>>;
        using mapping_t = typename mdc::mapping_type;

        const emu::extents ext_t(Exts...);
        const mapping_t mapping(ext_t);
        auto con = emu::cuda::device::make_mdcontainer<T>(device, mapping);

        check(con, size, cpt, dims);
    };

    TEST(CudaDeviceMdContainer, Constructors)
    {
        {
        // constructor (1) - default construction
            test_construct_1<int,1>();
            test_construct_1<int,2>();
            test_construct_1<int,3>();
        }
    }

    TEST(CudaDeviceMdContainer, make_mdcontainer)
    {
        {
            test_make_2<int,2>();
            test_make_2<int,2,3>();
            test_make_2<int,2,3,2>();
        }
        {
            test_make_3<int,2>();
            test_make_3<int,2,3>();
            test_make_3<int,2,3,2>();
        }
        {
            test_make_4<int,2>();
            test_make_4<int,2,3>();
            test_make_4<int,2,3,2>();
        }
        {
            test_make_5<int,2>();
            test_make_5<int,2,3>();
            test_make_5<int,2,3,2>();
        }
        {
            test_make_6<int,2>();
            test_make_6<int,2,3>();
            test_make_6<int,2,3,2>();
        }
    }

    TEST(CudaDeviceMdContainer, container_info)
    {
        auto device = emu::cuda::device::current();
        auto con = emu::cuda::device::make_mdcontainer<int>(device, std::array{2,3,2});

        auto str = fmt::to_string(emu::info(con));

        using testing::StartsWith;
        EXPECT_THAT(str, StartsWith("mdspan<int, [dyn, dyn, dyn], C/right>"));
    }

    TEST(CudaDeviceMdContainer, submdcontainer)
    {
        auto device = emu::cuda::device::current();
        auto con = emu::cuda::device::make_mdcontainer<int>(device, std::array{2,3,2});

        EXPECT_EQ(con.rank(), 3);
        EXPECT_EQ(con.use_count(), 1);

        auto sub = emu::submdcontainer(con, 0);

        EXPECT_EQ(sub.rank(), 2);
        EXPECT_EQ(con.use_count(), 2);
        EXPECT_EQ(sub.use_count(), 2);
    }

} // namespace
