#include <gtest/gtest.h>
#include <utility_test.hpp>
#include <vector>

#include <emu/container.hpp>
#include <emu/mdcontainer.hpp>
#include <emu/scoped.hpp>


#include <emu/info.hpp>

#include <cstdlib>
#include <type_traits>

template<typename C>
void check(C &con, size_t s, int c){
    EXPECT_EQ(con.size(), s);
    EXPECT_EQ(con.use_count(), c);
}
template<typename C, typename T>
void check(C &con, size_t s, int c, const T *ptr){
    EXPECT_EQ(con.data_handle(), ptr);
    check(con, s, c);
}
template<typename C, typename A, std::size_t N>
void check(C &con, size_t s, int c, const std::array<A,N> &dims){
    check(con, s, c);
    for(int i=0;i<N;i++){
        EXPECT_EQ(con.extents().extent(i),dims[i]);
    }
}
template<typename C, typename T, typename A, std::size_t N>
void check(C &con, size_t s, int c, const T *ptr, const std::array<A,N> &dims){
    check(con, s, c, ptr);
    for(std::size_t i=0;i<N;i++){
        EXPECT_EQ(con.extents().extent(i),dims[i]);
    }
}

namespace
{

    template<typename T, int d>
    void test_construct_1(){
        emu::mdcontainer<T, emu::_nd<d>> con;
        check(con, 0, 0, (T*)nullptr);
    };

    template<typename T, int ... exts>
    void test_construct_2(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);

        std::vector<T> v(size,1);
        T * ptr = v.data();
        emu::mdcontainer con(std::move(v),exts...);

        check(con, size, cpt, ptr);
    };

    template<typename T, int ... exts>
    void test_construct_3(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        emu::span sdims = dims;
        std::vector<T> v(size, 1);
        T * ptr = v.data();
        emu::mdcontainer con(std::move(v),sdims);

        check(con, size, cpt, ptr, dims);
    };

    template<typename T, int ... exts>
    void test_construct_4(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        std::vector<T> v(size,cpt);
        T * ptr = v.data();
        emu::mdcontainer con(std::move(v),dims);

        check(con, size, cpt, ptr, dims);
    };

    template<typename T, int ... exts>
    void test_construct_5(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        std::vector<T> v(size,1);
        T * ptr = v.data();
        emu::extents ext_t(exts...);
        emu::mdcontainer con(std::move(v),ext_t);

        check(con, size, cpt, ptr, dims);
    };

    template<typename T, int ... exts>
    void test_construct_6(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        std::vector<T> v(size,1);
        T * ptr = v.data();
        using mdc = emu::mdcontainer<T, emu::_nd<d>>;
        using mapping_t = typename mdc::mapping_type;
        emu::extents ext_t(exts...);
        mapping_t mapping(ext_t);
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
            test_construct_3<int,2,3>();
            test_construct_3<int,2,3,2>();
        }
        {
            // (4)
            test_construct_4<int,2>();
            test_construct_4<int,2,3>();
            test_construct_4<int,2,3,2>();
        }
        {
            // (5)
            test_construct_5<int,2>();
            test_construct_5<int,2,3>();
            test_construct_5<int,2,3,2>();
        }
        {
            // (6)
            test_construct_6<int,2>();
            test_construct_6<int,2,3>();
            test_construct_6<int,2,3,2>();
        }
    }

    template<typename T, int ... exts>
    void test_make_2(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        auto con = emu::make_mdcontainer<T>(exts...);

        check(con, size, cpt, dims);
    };

    template<typename T, int ... exts>
    void test_make_3(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        emu::span sdims = dims;
        auto con = emu::make_mdcontainer<T>(sdims);

        check(con, size, cpt, dims);
    };

    template<typename T, int ... exts>
    void test_make_4(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        auto con = emu::make_mdcontainer<T>(dims);

        check(con, size, cpt, dims);
    };

    template<typename T, int ... exts>
    void test_make_5(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        const emu::_nd<d> ext_t(exts...);
        auto con = emu::make_mdcontainer<T,d>(ext_t);

        check(con, size, cpt, dims);
    };

    template<typename T, int ... exts>
    void test_make_6(){
        int cpt = 1;
        constexpr std::size_t d = sizeof...(exts);
        size_t size = (1 * ... * exts);
        auto dims = std::array{exts...};

        using mdc = emu::mdcontainer<T, emu::_nd<d>>;
        using mapping_t = typename mdc::mapping_type;

        emu::extents ext_t(exts...);
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

}