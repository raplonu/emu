#include <gtest/gtest.h>

#include <emu/host/container.hpp>
#include <emu/scoped.hpp>

#include <emu/info.hpp>
#include <emu/mdspan.hpp>

#include <vector>
#include <array>

struct DeviceRange {

    int i;

    int* begin() { return &i; }
    int* end() { return &i + 1; }

    [[nodiscard]] const int* begin() const { return &i; }
    [[nodiscard]] const int* end() const { return &i + 1; }

    [[nodiscard]] int* data() { return &i; }
    [[nodiscard]] const int* data() const { return &i; }

    [[nodiscard]] static std::size_t size() { return 1; }

};

template<>
inline constexpr bool emu::spe::enable_host_range<DeviceRange> = false;

namespace
{
    TEST(HostContainer, Construct)
    {

        {
            const emu::host::container<int> con;

            EXPECT_EQ(con.data(), nullptr);
            EXPECT_EQ(con.size(), 0);
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            int arr[] = {1, 2, 3, 4, 5}; // NOLINT
            const emu::host::container con = arr;

            EXPECT_EQ(con.data(), arr);
            EXPECT_EQ(con.size(), 5);
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            std::array arr = {1, 2, 3, 4, 5};
            const emu::host::container con( arr );

            EXPECT_EQ(con.data(), arr.data());
            EXPECT_EQ(con.size(), arr.size());
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            //It's not possible to construct a container from a std::array rvalue.
            //because it is not possible to get the pointer after moving the array
            //into the capsule.
            static_assert(not std::constructible_from<emu::host::container<int>, std::array<int, 5>>);
            static_assert(not std::constructible_from<emu::host::container<int>, std::array<int, 5>&&>);
        }
        {
            std::vector vec(3, 0);

            const emu::host::container con( vec );

            EXPECT_EQ(con.data(), vec.data());
            EXPECT_EQ(con.size(), vec.size());
            EXPECT_EQ(con.use_count(), 0);
        }

        {
            const emu::host::container con( std::vector(5, 0) );

            EXPECT_NE(con.data(), nullptr);
            EXPECT_EQ(con.size(), 5);
            EXPECT_EQ(con.use_count(), 1);
        }
        {
            static_assert(std::constructible_from<emu::host::container<int>, std::vector<int>>);

            static_assert(not std::constructible_from<emu::host::container<int>, DeviceRange>);
            static_assert(not std::constructible_from<emu::host::container<int>, DeviceRange&>);

            static_assert(not std::constructible_from<emu::host::container<const int>, DeviceRange>);
            static_assert(not std::constructible_from<emu::host::container<const int>, DeviceRange&>);

            static_assert(not std::constructible_from<emu::host::container<const int>, const DeviceRange>);
            static_assert(not std::constructible_from<emu::host::container<const int>, const DeviceRange&>);

            using DataHolder = int;

            static_assert(std::constructible_from<emu::host::container<int>, std::vector<int>, DataHolder>);
            static_assert(std::constructible_from<emu::host::container<int>, std::vector<int>&, DataHolder>);

            static_assert(std::constructible_from<emu::host::container<const int>, const std::vector<int>, DataHolder>);
            static_assert(std::constructible_from<emu::host::container<const int>, const std::vector<int>&, DataHolder>);

            static_assert(not std::constructible_from<emu::host::container<int>, DeviceRange, DataHolder>);
            static_assert(not std::constructible_from<emu::host::container<int>, DeviceRange&, DataHolder>);

            static_assert(not std::constructible_from<emu::host::container<const int>, const DeviceRange, DataHolder>);
            static_assert(not std::constructible_from<emu::host::container<const int>, const DeviceRange&, DataHolder>);


        }
    }

} // namespace
