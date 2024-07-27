#include <gtest/gtest.h>
#include <initializer_list>
#include <sys/stat.h>
#include <utility_test.hpp>

#include <emu/container.hpp>
#include <emu/scoped.hpp>

#include <emu/info.hpp>
#include <emu/mdspan.hpp>

#include <vector>
#include <array>

struct shared_vector
{
    std::shared_ptr<int[]> data;
    std::size_t size;

    shared_vector(std::initializer_list<int> il)
        : data(new int[il.size()]), size(il.size())
    {
        std::copy(il.begin(), il.end(), data.get());
    }

    shared_vector(const shared_vector& other) = default;
    shared_vector(shared_vector&& other) = default;

    shared_vector& operator=(const shared_vector& other) = default;
    shared_vector& operator=(shared_vector&& other) = default;

    ~shared_vector() = default;

    int* begin() const { return data.get(); }
    int* end() const { return data.get() + size; }
};

template <>
constexpr bool emu::spe::enable_relocatable_owning_range<shared_vector> = true;

struct shared_vector_view : shared_vector, std::ranges::view_base
{
    using shared_vector::shared_vector;
};

namespace
{

    TEST(Container, Construct)
    {
        {
            const emu::container<int> con;

            EXPECT_EQ(con.data(), nullptr);
            EXPECT_EQ(con.size(), 0);
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            int arr[] = {1, 2, 3, 4, 5}; // NOLINT
            const emu::container con = arr;

            EXPECT_EQ(con.data(), arr);
            EXPECT_EQ(con.size(), 5);
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            std::array arr = {1, 2, 3, 4, 5};
            const emu::container con( arr );

            EXPECT_EQ(con.data(), arr.data());
            EXPECT_EQ(con.size(), arr.size());
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            //It's not possible to construct a container from a std::array rvalue.
            //because it is not possible to get the pointer after moving the array
            //into the capsule.
            static_assert(not std::constructible_from<emu::container<int>, std::array<int, 5>>);
            static_assert(not std::constructible_from<emu::container<int>, std::array<int, 5>&&>);
        }
        {
            std::vector vec(3, 0);

            emu::container con( vec );

            EXPECT_EQ(con.data(), vec.data());
            EXPECT_EQ(con.size(), vec.size());
            EXPECT_EQ(con.use_count(), 0);
        }

        {
            const emu::container con{ std::vector(5, 0) };

            EXPECT_NE(con.data(), nullptr);
            EXPECT_EQ(con.size(), 5);
            EXPECT_EQ(con.use_count(), 1);
        }
    }

    TEST(Container, DeductionGuideline)
    {
        using emu::copy;

        int data_holding{};
        std::array<int, 3> s_arr{};

        {
            std::vector<int> v{};

            emu::container c1{ v };
            static_assert(std::is_same_v<decltype(c1), emu::container<int>>);

            emu::container c2{ v.begin(), v.end() };
            static_assert(std::is_same_v<decltype(c2), emu::container<int>>);

            emu::container c3{ v.data(), v.data() + v.size() };
            static_assert(std::is_same_v<decltype(c3), emu::container<int>>);

            emu::container c4{ v, copy(data_holding) };
            static_assert(std::is_same_v<decltype(c4), emu::container<int>>);

            emu::container c5{ v.begin(), v.end(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c5), emu::container<int>>);

            emu::container c6{ v.data(), v.data() + v.size(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c6), emu::container<int>>);
        }
        {
            const std::vector<int> v{};

            emu::container c1{ v };
            static_assert(std::is_same_v<decltype(c1), emu::container<const int>>);

            emu::container c2{ v.begin(), v.end() };
            static_assert(std::is_same_v<decltype(c2), emu::container<const int>>);

            emu::container c3{ v.data(), v.data() + v.size() };
            static_assert(std::is_same_v<decltype(c3), emu::container<const int>>);

            emu::container c4{ v, copy(data_holding) };
            static_assert(std::is_same_v<decltype(c4), emu::container<const int>>);

            emu::container c5{ v.begin(), v.end(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c5), emu::container<const int>>);

            emu::container c6{ v.data(), v.data() + v.size(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c6), emu::container<const int>>);
        }
        {
            std::array<int, 3> arr{};

            emu::container c1{ arr };
            static_assert(std::is_same_v<decltype(c1), emu::container<int, 3>>);

            emu::container c2{ arr.begin(), arr.end() };
            static_assert(std::is_same_v<decltype(c2), emu::container<int>>);

            emu::container c3{ arr.data(), arr.data() + arr.size() };
            static_assert(std::is_same_v<decltype(c3), emu::container<int>>);

            emu::container c5{ arr.begin(), arr.end(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c5), emu::container<int>>);

            emu::container c6{ arr.data(), arr.data() + arr.size(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c6), emu::container<int>>);
        }
        {
            const std::array<int, 3> arr{};

            emu::container c1{ arr };
            static_assert(std::is_same_v<decltype(c1), emu::container<const int, 3>>);

            emu::container c2{ arr.begin(), arr.end() };
            static_assert(std::is_same_v<decltype(c2), emu::container<const int>>);

            emu::container c3{ arr.data(), arr.data() + arr.size() };
            static_assert(std::is_same_v<decltype(c3), emu::container<const int>>);

            emu::container c5{ arr.begin(), arr.end(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c5), emu::container<const int>>);

            emu::container c6{ arr.data(), arr.data() + arr.size(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c6), emu::container<const int>>);
        }
        {
            int tab[] = {1, 2, 3};

            emu::container c1{ tab };
            static_assert(std::is_same_v<decltype(c1), emu::container<int, 3>>);

            emu::container c2{ static_cast<int*>(tab), 3 };
            static_assert(std::is_same_v<decltype(c2), emu::container<int>>);

            emu::container c3{ static_cast<int*>(tab), tab + 3 };
            static_assert(std::is_same_v<decltype(c3), emu::container<int>>);
        }
        {
            const int tab[] = {1, 2, 3};

            emu::container c1{ tab };
            static_assert(std::is_same_v<decltype(c1), emu::container<const int, 3>>);

            emu::container c2{ static_cast<const int*>(tab), 3 };
            static_assert(std::is_same_v<decltype(c2), emu::container<const int>>);

            emu::container c3{ static_cast<const int*>(tab), tab + 3 };
            static_assert(std::is_same_v<decltype(c3), emu::container<const int>>);
        }
        {
            std::span<int, 3> s{s_arr};

            emu::container c1{ s };
            static_assert(std::is_same_v<decltype(c1), emu::container<int, 3>>);

            emu::container c2{ s.begin(), s.end() };
            static_assert(std::is_same_v<decltype(c2), emu::container<int>>);

            emu::container c3{ s.data(), s.data() + s.size() };
            static_assert(std::is_same_v<decltype(c3), emu::container<int>>);

            emu::container c4{ s, copy(data_holding) };
            static_assert(std::is_same_v<decltype(c4), emu::container<int, 3>>);

            emu::container c5{ s.begin(), s.end(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c5), emu::container<int>>);

            emu::container c6{ s.data(), s.data() + s.size(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c6), emu::container<int>>);
        }
        {
            std::span<const int, 3> s{s_arr};

            emu::container c1{ s };
            static_assert(std::is_same_v<decltype(c1), emu::container<const int, 3>>);

            emu::container c2{ s.begin(), s.end() };
            static_assert(std::is_same_v<decltype(c2), emu::container<const int>>);

            emu::container c3{ s.data(), s.data() + s.size() };
            static_assert(std::is_same_v<decltype(c3), emu::container<const int>>);

            emu::container c4{ s, copy(data_holding) };
            static_assert(std::is_same_v<decltype(c4), emu::container<const int, 3>>);

            emu::container c5{ s.begin(), s.end(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c5), emu::container<const int>>);

            emu::container c6{ s.data(), s.data() + s.size(), copy(data_holding) };
            static_assert(std::is_same_v<decltype(c6), emu::container<const int>>);
        }
        {
            emu::container c1{ std::vector<int>{} };
            static_assert(std::is_same_v<decltype(c1), emu::container<int>>);

            emu::container c2{ std::span<int>{} };
            static_assert(std::is_same_v<decltype(c2), emu::container<int>>);

            emu::container c3{ std::span<const int>{} };
            static_assert(std::is_same_v<decltype(c3), emu::container<const int>>);

        }
        {
            emu::container c1( {1,2,3} );
            static_assert(std::is_same_v<decltype(c1), emu::container<const int>>);
        }
    }

    TEST(Container, TakeOwnerShip)
    {


        {
            std::vector<int> obj{1,2,3};
            const auto& cref = obj;

            {
                emu::container<int> c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                emu::container<const int> c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                emu::container<int> c_from_rref{std::move(obj)};
                EXPECT_EQ(c_from_rref.use_count(), 1);
            }
        }
        {
            std::vector<int> data{1,2,3};
            std::span<int> obj{data};
            const auto& cref = obj;

            {
                emu::container<int> c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                emu::container<int> c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                emu::container<int> c_from_rref{std::move(obj)};
                EXPECT_EQ(c_from_rref.use_count(), 0);
            }
        }
        {
            std::array<int, 3> obj{1,2,3};
            const auto& cref = obj;

            {
                emu::container<int> c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                emu::container<const int> c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                static_assert(not std::constructible_from<emu::container<int>, std::array<int, 3>&&>);
            }
        }
        {
            std::vector<int> data{1,2,3};
            std::span<int> obj{data};
            const auto& cref = obj;

            {
                emu::container<int> c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                emu::container<int> c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                emu::container<int> c_from_rref{std::move(obj)};
                EXPECT_EQ(c_from_rref.use_count(), 0);
            }
        }
        {
            shared_vector obj{1,2,3};
            const auto& cref = obj;

            {
                emu::container<int> c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 1);
            }
            {
                emu::container<int> c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 1);
            }
            {
                emu::container<int> c_from_rref{std::move(obj)};
                EXPECT_EQ(c_from_rref.use_count(), 1);
            }
        }
    }


    TEST(Container, Wrap)
    {
        std::array arr = {1, 2, 3, 4, 5};

        const emu::container con = arr;

        EXPECT_EQ(con.data(), arr.data());
        EXPECT_EQ(con.size(), arr.size());
        EXPECT_EQ(con.use_count(), 0);
    }

    TEST(Container, AllocateSimple)
    {
        const auto con = emu::make_container<int>(5);

        EXPECT_NE(con.data(), nullptr);
        EXPECT_EQ(con.size(), 5);
    }

    TEST(Container, FreeCapsule)
    {
        const std::array arr = {1, 2, 3, 4, 5};

        bool called = false;

        {
            const emu::container con(arr.data(), arr.size(), emu::scoped([&called]() { called = true; }));
        }

        EXPECT_TRUE(called);
    }

    TEST(Container, TakeOwnership)
    {
        emu_test::spy_flag sf;

        int value = 1;
        emu_test::SpyContainer spy_con{&sf, &value};

        {
            // Take ownership of the container.
            const emu::container con = std::move(spy_con);

            EXPECT_EQ(con.use_count(), 1);

            EXPECT_EQ(con[0], 1);
            value = 0;
            EXPECT_EQ(con[0], 0);

            // The container is moved away.
            EXPECT_TRUE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        EXPECT_TRUE(sf.destroyed);

    }

    TEST(Container, TakeRef)
    {
        emu_test::spy_flag sf;

        int value = 1;
        emu_test::SpyContainer spy_con{&sf, &value};

        {
            // Take spy_con by reference.
            const emu::container con = spy_con;

            EXPECT_EQ(con.use_count(), 0);

            EXPECT_EQ(con[0], 1);
            value = 0;
            EXPECT_EQ(con[0], 0);

            // The container not moved nor copied.
            EXPECT_FALSE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        // and not destroyed.
        EXPECT_FALSE(sf.destroyed);

    }

    TEST(Container, ConstReferenceContainerTest)
    {
        emu_test::spy_flag sf;

        int value = 1;
        const emu_test::SpyContainer spy_con{&sf, &value};

        {
            // Take spy_con by constant reference.
            const emu::container con = spy_con;

            EXPECT_EQ(con.use_count(), 0);

            EXPECT_EQ(con[0], 1);
            value = 0;
            EXPECT_EQ(con[0], 0);

            // The container not moved nor copied.
            EXPECT_FALSE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        EXPECT_FALSE(sf.destroyed);
    }

} // namespace
