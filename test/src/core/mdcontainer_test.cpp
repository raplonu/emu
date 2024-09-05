#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>

#include <emu/container.hpp>
#include <emu/mdcontainer.hpp>
#include <emu/scoped.hpp>

#include <emu/info.hpp>

#include <cstdlib>



#include <test_utility/view/core.hpp>

#include <test_utility/view/mdview_test.hpp>
#include <test_utility/view/capsule_test.hpp>

namespace
{

REGISTER_TYPED_TEST_SUITE_P(
    ViewTest,  // The first argument is the test case name.
    // The rest of the arguments are the test names.
    MDViewConstruct, TakeOwnerShip, Wrap, FreeCapsule/* , TestDevice */);

    struct mdcontainer_1d_of_int /* : DoViewTest, DoContiguousTest, DoCapsuleTest */ {

        using data_type = int;

        using view_type = emu::mdcontainer_1d<int>;
        using const_view_type = emu::mdcontainer_1d<const int>;

        static constexpr size_t rank = 1;

        template<typename DataHolder>
        static const_view_type from_ptr_and_size(const data_type *ptr, size_t size, DataHolder &&data_holder)
        {
            return const_view_type(ptr, std::forward<DataHolder>(data_holder), emu::exts_flag, size);
        }

    };

    using ContainerTestsList = testing::Types<mdcontainer_1d_of_int>;

    INSTANTIATE_TYPED_TEST_SUITE_P(MDContainerTests,    // Instance name
                                ViewTest,             // Test case name
                                ContainerTestsList);  // Type list




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
        emu::mdcontainer<T, emu::dims<D>> con;
        check(con, 0, 0, (T*)nullptr);
    };

    template<typename T, int ... Exts>
    void test_construct_2(){
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);

        std::vector<T> v(size,1);
        T * ptr = v.data();
        emu::mdcontainer con(std::move(v), Exts...);

        check(con, size, cpt, ptr);
    };

    template<typename T, size_t ... Exts>
    void test_construct_3(){
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        const emu::span sdims = dims;
        std::vector<T> v(size, 1);
        T * ptr = v.data();
        emu::mdcontainer con(std::move(v), sdims);

        check(con, size, cpt, ptr, dims);
    };

    template<typename T, int ... Exts>
    void test_construct_4(){
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        std::vector<T> v(size,cpt);
        T * ptr = v.data();
        emu::mdcontainer con(std::move(v),dims);

        check(con, size, cpt, ptr, dims);
    };

    template<typename T, int ... Exts>
    void test_construct_5(){
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        std::vector<T> v(size,1);
        T * ptr = v.data();
        const emu::extents ext_t(Exts...);
        emu::mdcontainer con(std::move(v),ext_t);

        check(con, size, cpt, ptr, dims);
    };

    template<typename T, int ... Exts>
    void test_construct_6(){
        const int cpt = 1;
        constexpr std::size_t d = sizeof...(Exts);
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        std::vector<T> v(size,1);
        T * ptr = v.data();
        using mdc = emu::mdcontainer<T, emu::dims<d>>;
        using mapping_t = typename mdc::mapping_type;
        const emu::extents ext_t(Exts...);
        const mapping_t mapping(ext_t);
        emu::mdcontainer con(std::move(v),mapping);

        check(con, size, cpt, ptr, dims);
    };


    TEST(MdContainer, Constructors)
    {
        {
        // constructor (1)
            test_construct_1<int,1>();
            test_construct_1<int,2>();
            test_construct_1<int,3>();
        }
        {
            // (2)
            test_construct_2<int,2>();
            test_construct_2<int,2,3>();
            test_construct_2<int,2,3,2>();
        }
        {
            // (3)
            test_construct_3<int,2>();
            // test_construct_3<int,2,3>();
            // test_construct_3<int,2,3,2>();
        }
        // {
        //     // (4)
        //     test_construct_4<int,2>();
        //     test_construct_4<int,2,3>();
        //     test_construct_4<int,2,3,2>();
        // }
        // {
        //     // (5)
        //     test_construct_5<int,2>();
        //     test_construct_5<int,2,3>();
        //     test_construct_5<int,2,3,2>();
        // }
        // {
        //     // (6)
        //     test_construct_6<int,2>();
        //     test_construct_6<int,2,3>();
        //     test_construct_6<int,2,3,2>();
        // }
    }

    template<typename T, size_t... Exts>
    void test_make_2(){
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        auto con = emu::make_mdcontainer<T>(Exts...);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_3(){
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        const emu::span sdims = dims;
        auto con = emu::make_mdcontainer<T>(sdims);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_4(){
        const int cpt = 1;
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        auto con = emu::make_mdcontainer<T>(dims);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_5(){
        const int cpt = 1;
        constexpr std::size_t d = sizeof...(Exts);
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        const emu::dims<d> ext_t(Exts...);
        auto con = emu::make_mdcontainer<T,d>(ext_t);

        check(con, size, cpt, dims);
    };

    template<typename T, size_t... Exts>
    void test_make_6(){
        const int cpt = 1;
        constexpr std::size_t d = sizeof...(Exts);
        const size_t size = (1 * ... * Exts);
        auto dims = std::array{Exts...};

        using mdc = emu::mdcontainer<T, emu::dims<d>>;
        using mapping_t = typename mdc::mapping_type;

        const emu::extents ext_t(Exts...);
        const mapping_t mapping(ext_t);
        auto con = emu::make_mdcontainer<T>(mapping);

        check(con, size, cpt, dims);
    };

    TEST(MdContainer, make_mdcontainer)
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

    TEST(MdContainer, container_info)
    {
        auto con = emu::make_mdcontainer<int>(std::array{2,3,2});

        auto str = fmt::to_string(emu::info(con));

        using testing::StartsWith;
        EXPECT_THAT(str, StartsWith("mdspan<int, [dyn, dyn, dyn], C/right>"));
    }

    TEST(MdContainer, submdcontainer)
    {
        auto con = emu::make_mdcontainer<int>(std::array{2,3,2});

        EXPECT_EQ(con.rank(), 3);
        EXPECT_EQ(con.use_count(), 1);

        auto sub = emu::submdcontainer(con, 0);

        EXPECT_EQ(sub.rank(), 2);
        EXPECT_EQ(con.use_count(), 2);
        EXPECT_EQ(sub.use_count(), 2);
    }

} // namespace
