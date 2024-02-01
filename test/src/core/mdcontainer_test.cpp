#include <gtest/gtest.h>
#include <utility_test.hpp>
#include <vector>

#include <emu/container.hpp>
#include <emu/mdcontainer.hpp>
#include <emu/scoped.hpp>


#include <emu/info.hpp>

#include <cstdlib>
#include <type_traits>

namespace
{



    TEST(MdSpan, Construct)
    {


        {
            /*auto con1 =*/ emu::make_container<int>(12);
            /*auto con2 =*/ emu::make_container<int>(2,6);
            /*auto con3 =*/ emu::make_container<int>(2,3,2);
            //fmt::print("{}:{}  {}\n",__FILE__,__LINE__,emu::info(con));
        }

        //// (3) mdspan( data_handle_type p, std::span<OtherIndexType, N> exts );
        //{
        //    std::vector<int> v = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
        //    std::array<size_t,1> d1 ;
        //    emu::span sd1(d1);
        //    emu::mdspan_1d<int> spa1(v.data(),d1); // c4
        //    emu::mdspan_1d<int> spa2(v.data(),sd1); // c4
        //    //emu::mdspan_1d<int> spa2(v.data(),emu::stdex::extents<int,1>(12));
        //    fmt::print("{}\n{}\n",emu::info(spa1),emu::info(spa2));

        //}
    }

    template<typename T, int d>
    void test_construct_1(){
        emu::mdcontainer<T, emu::_nd<d>> con;
        EXPECT_EQ(con.data_handle(), nullptr);
        EXPECT_EQ(con.size(), 0);
        EXPECT_EQ(con.use_count(), 0);
    };

    template<typename T, typename ... OtherIndexTypes>
    void test_construct_2(int n, OtherIndexTypes... exts){
        constexpr std::size_t d = sizeof...(exts);
        std::vector<T> v(n,1);
        T * ptr = v.data();
        emu::mdcontainer<T,emu::_nd<d>> con(v.data(),std::move(v),exts...);
        EXPECT_EQ(con.data_handle(), ptr);
        EXPECT_EQ(con.size(), n);
        EXPECT_EQ(con.use_count(), 1);
    };

    template<typename T, typename ... OtherIndexTypes>
    void test_construct_3(OtherIndexTypes... exts){
        constexpr std::size_t d = sizeof...(OtherIndexTypes);
        auto dims = std::array{exts...};
        emu::span sdims = dims;
        int n=1;
        for(int i=0;i<d;i++){n = n*dims[i];}
        std::vector<T> v(n,1);
        T * ptr = v.data();
        emu::mdcontainer<T,emu::_nd<d>> con(v.data(),std::move(v),sdims);
        EXPECT_EQ(con.data_handle(), ptr);
        EXPECT_EQ(con.size(), n);
        EXPECT_EQ(con.use_count(), 1);
        for(int i=0;i<d;i++){EXPECT_EQ(con.extents().extent(i),dims[i]);}
    };

    template<typename T, typename ... OtherIndexTypes>
    void test_construct_4(OtherIndexTypes... exts){
        constexpr std::size_t d = sizeof...(OtherIndexTypes);
        auto dims = std::array{exts...};
        int n=1;
        for(int i=0;i<d;i++){n = n*dims[i];}
        std::vector<T> v(n,1);
        T * ptr = v.data();

        emu::mdcontainer<T,emu::_nd<d>> con(ptr,std::move(v),dims);
        EXPECT_EQ(con.data_handle(), ptr);
        EXPECT_EQ(con.size(), n);
        EXPECT_EQ(con.use_count(), 1);
        fmt::print("{}\n",emu::info(con));
        for(int i=0;i<d;i++){EXPECT_EQ(con.extents().extent(i),dims[i]);}
    };



template<emu::cpts::mdspan S>
void check(S span){}

    TEST(MdContainer, Constructors)
    {
        {
            emu::mdcontainer_0d<int> con(0);
            emu::mdspan_0d<int> spa(0);
            EXPECT_EQ(con.data_handle(), nullptr);
            EXPECT_EQ(con.size(), 1);
            EXPECT_EQ(con.use_count(), 0);
            check(spa);
        }
        {
        // constructor (1) mdspan()
            test_construct_1<int,1>();
            test_construct_1<int,2>();
            test_construct_1<int,3>();
        }
        {
            // (2) explicit mdspan( data_handle_type p, OtherIndexTypes... exts );
            test_construct_2<int>(12,12);
            test_construct_2<int>(12,2,6);
            test_construct_2<int>(12,2,3,2);
        }
        {
            // (4) explicit mdspan( data_handle_type p, OtherIndexTypes... exts );
            test_construct_3<int>(12);
            test_construct_3<int>(2,6);
            test_construct_3<int>(2,3,2);
        }
        {
            // (4) explicit mdspan( data_handle_type p, OtherIndexTypes... exts );
            test_construct_4<int>(12);
            test_construct_4<int>(2,6);
            test_construct_4<int>(2,3,2);
        }
    }

    template<typename T, typename ... OtherIndexTypes>
    void test_make(OtherIndexTypes... exts){
        constexpr std::size_t n = sizeof...(exts);
        std::size_t size=1;
        for(auto e :{exts...}){size = size*e;}
        auto con = emu::make_container<int>(exts...);
        EXPECT_EQ(con.size(), size);
        EXPECT_EQ(con.use_count(), 1);
    };
    template<typename T, typename OtherIndexTypes, std::size_t N>
    void test_make(const emu::span<OtherIndexTypes, N>& exts){
        std::size_t size=1;
        for(auto e :exts){size = size*e;}
        auto con = emu::make_container<int>(exts);
        EXPECT_EQ(con.size(), size);
        EXPECT_EQ(con.use_count(), 1);
    };

    TEST(MdContainer, make_mdcontainer)
    {
        {
            test_make<int>(12);
            test_make<int>(2,6);
            test_make<int>(2,3,2);
        }
        {
            auto d1 = std::array{12};
            emu::span ext1 = d1 ;
            test_make<int>(ext1);

            auto d2 = std::array{2,6};
            emu::span ext2 = d2 ;
            test_make<int>(ext2);

            auto d3 = std::array{2,3,2};
            emu::span ext3 = d3 ;
            test_make<int>(ext3);

        }
//        {
//            auto d1 = emu::make_container<std::size_t>(1);
//            d1[0]=12;
//            test_make<int>((emu::span<int>)d1);
//
//        }
    }

}