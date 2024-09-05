#include <gtest/gtest.h>
#include <test_utility/spy.hpp>

#include <emu/container.hpp>
#include <emu/scoped.hpp>

#include <emu/info.hpp>
#include <emu/mdspan.hpp>

#include <vector>
#include <array>

#include <test_utility/view/core.hpp>

#include <test_utility/view/view_test.hpp>
#include <test_utility/view/capsule_test.hpp>

namespace
{

    REGISTER_TYPED_TEST_SUITE_P(
        ViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        ViewConstruct, TakeOwnerShip, Wrap, FreeCapsule);

    struct container_of_int {

        using data_type = int;

        using view_type = emu::container<int>;
        using const_view_type = emu::container<const int>;

        template<typename DataHolder>
        static const_view_type from_ptr_and_size(const data_type *ptr, size_t size, DataHolder &&data_holder)
        {
            return const_view_type(ptr, size, std::forward<DataHolder>(data_holder));
        }

    };

    using ContainerTestsList = testing::Types<container_of_int>;

    INSTANTIATE_TYPED_TEST_SUITE_P(ContainerTests,    // Instance name
                                ViewTest,             // Test case name
                                ContainerTestsList);  // Type list

    TEST(Container, DeductionGuideline)
    {
        using emu::copy;

        const int data_holding{};
        std::array<int, 3> s_arr{};

        {
            std::vector<int> v{};

            const emu::container c1( v );
            static_assert(std::is_same_v<decltype(c1), const emu::container<int>>);

            const emu::container c2( v.begin(), v.end() );
            static_assert(std::is_same_v<decltype(c2), const emu::container<int>>);

            const emu::container c3( v.data(), v.data() + v.size() );
            static_assert(std::is_same_v<decltype(c3), const emu::container<int>>);

            const emu::container c4( v, copy(data_holding) );
            static_assert(std::is_same_v<decltype(c4), const emu::container<int>>);

            const emu::container c5( v.begin(), v.end(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c5), const emu::container<int>>);

            const emu::container c6( v.data(), v.data() + v.size(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c6), const emu::container<int>>);
        }
        {
            const std::vector<int> v{};

            const emu::container c1( v );
            static_assert(std::is_same_v<decltype(c1), const emu::container<const int>>);

            const emu::container c2( v.begin(), v.end() );
            static_assert(std::is_same_v<decltype(c2), const emu::container<const int>>);

            const emu::container c3( v.data(), v.data() + v.size() );
            static_assert(std::is_same_v<decltype(c3), const emu::container<const int>>);

            const emu::container c4( v, copy(data_holding) );
            static_assert(std::is_same_v<decltype(c4), const emu::container<const int>>);

            const emu::container c5( v.begin(), v.end(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c5), const emu::container<const int>>);

            const emu::container c6( v.data(), v.data() + v.size(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c6), const emu::container<const int>>);
        }
        {
            std::array<int, 3> arr{};

            const emu::container c1( arr );
            static_assert(std::is_same_v<decltype(c1), const emu::container<int, 3>>);

            const emu::container c2( arr.begin(), arr.end() );
            static_assert(std::is_same_v<decltype(c2), const emu::container<int>>);

            const emu::container c3( arr.data(), arr.data() + arr.size() );
            static_assert(std::is_same_v<decltype(c3), const emu::container<int>>);

            const emu::container c5( arr.begin(), arr.end(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c5), const emu::container<int>>);

            const emu::container c6( arr.data(), arr.data() + arr.size(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c6), const emu::container<int>>);
        }
        {
            const std::array<int, 3> arr{};

            const emu::container c1( arr );
            static_assert(std::is_same_v<decltype(c1), const emu::container<const int, 3>>);

            const emu::container c2( arr.begin(), arr.end() );
            static_assert(std::is_same_v<decltype(c2), const emu::container<const int>>);

            const emu::container c3( arr.data(), arr.data() + arr.size() );
            static_assert(std::is_same_v<decltype(c3), const emu::container<const int>>);

            const emu::container c5( arr.begin(), arr.end(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c5), const emu::container<const int>>);

            const emu::container c6( arr.data(), arr.data() + arr.size(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c6), const emu::container<const int>>);
        }
        {
            int tab[] = {1, 2, 3};

            const emu::container c1( tab );
            static_assert(std::is_same_v<decltype(c1), const emu::container<int, 3>>);

            const emu::container c2( static_cast<int*>(tab), 3 );
            static_assert(std::is_same_v<decltype(c2), const emu::container<int>>);

            const emu::container c3( static_cast<int*>(tab), static_cast<int*>(tab) + 3 );
            static_assert(std::is_same_v<decltype(c3), const emu::container<int>>);
        }
        {
            const int tab[] = {1, 2, 3};

            const emu::container c1( tab );
            static_assert(std::is_same_v<decltype(c1), const emu::container<const int, 3>>);

            const emu::container c2( static_cast<const int*>(tab), 3 );
            static_assert(std::is_same_v<decltype(c2), const emu::container<const int>>);

            const emu::container c3( static_cast<const int*>(tab), static_cast<const int*>(tab) + 3 );
            static_assert(std::is_same_v<decltype(c3), const emu::container<const int>>);
        }
        {
            std::span<int, 3> s{s_arr};

            const emu::container c1( s );
            static_assert(std::is_same_v<decltype(c1), const emu::container<int, 3>>);

            const emu::container c2( s.begin(), s.end() );
            static_assert(std::is_same_v<decltype(c2), const emu::container<int>>);

            const emu::container c3( s.data(), s.data() + s.size() );
            static_assert(std::is_same_v<decltype(c3), const emu::container<int>>);

            const emu::container c4( s, copy(data_holding) );
            static_assert(std::is_same_v<decltype(c4), const emu::container<int, 3>>);

            const emu::container c5( s.begin(), s.end(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c5), const emu::container<int>>);

            const emu::container c6( s.data(), s.data() + s.size(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c6), const emu::container<int>>);
        }
        {
            std::span<const int, 3> s{s_arr};

            const emu::container c1( s );
            static_assert(std::is_same_v<decltype(c1), const emu::container<const int, 3>>);

            const emu::container c2( s.begin(), s.end() );
            static_assert(std::is_same_v<decltype(c2), const emu::container<const int>>);

            const emu::container c3( s.data(), s.data() + s.size() );
            static_assert(std::is_same_v<decltype(c3), const emu::container<const int>>);

            const emu::container c4( s, copy(data_holding) );
            static_assert(std::is_same_v<decltype(c4), const emu::container<const int, 3>>);

            const emu::container c5( s.begin(), s.end(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c5), const emu::container<const int>>);

            const emu::container c6( s.data(), s.data() + s.size(), copy(data_holding) );
            static_assert(std::is_same_v<decltype(c6), const emu::container<const int>>);
        }
        {
            const emu::container c1( std::vector<int>{} );
            static_assert(std::is_same_v<decltype(c1), const emu::container<int>>);

            const emu::container c2( std::span<int>{} );
            static_assert(std::is_same_v<decltype(c2), const emu::container<int>>);

            const emu::container c3( std::span<const int>{} );
            static_assert(std::is_same_v<decltype(c3), const emu::container<const int>>);

        }
        {
            const emu::container c1{1,2,3};
            static_assert(std::is_same_v<decltype(c1), const emu::container<int>>);
        }
    }

    TEST(Container, AllocateSimple)
    {
        const auto con = emu::make_container<int>(5);

        EXPECT_NE(con.data(), nullptr);
        EXPECT_EQ(con.size(), 5);
    }


} // namespace
