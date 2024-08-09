#pragma once

#include <test_utility/spy.hpp>
#include <test_utility/view.hpp>
#include <test_utility/view/core.hpp>

#include <emu/scoped.hpp>

namespace
{

    TYPED_TEST_P(ViewTest, TakeOwnerShip)
    {
        using view_type = typename TestFixture::view_type;
        using const_view_type = typename TestFixture::const_view_type;
        using data_type = typename TestFixture::data_type;
        {
            std::vector<data_type> obj{1,2,3};
            const auto& cref = obj;

            {
                view_type c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                const_view_type c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                view_type c_from_rref(std::move(obj));
                EXPECT_EQ(c_from_rref.use_count(), 1);
            }
        }
        {
            std::vector<data_type> data{1,2,3};
            std::span<data_type> obj{data};
            const auto& cref = obj;

            {
                view_type c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                view_type c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                view_type c_from_rref{std::move(obj)};
                EXPECT_EQ(c_from_rref.use_count(), 0);
            }
        }
        {
            std::array<data_type, 3> obj{1,2,3};
            const auto& cref = obj;

            {
                view_type c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                const_view_type c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                static_assert(not std::constructible_from<view_type, std::array<data_type, 3>&&>);
            }
        }
        {
            std::vector<data_type> data{1,2,3};
            std::span<data_type> obj{data};
            const auto& cref = obj;

            {
                view_type c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 0);
            }
            {
                view_type c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 0);
            }
            {
                view_type c_from_rref{std::move(obj)};
                EXPECT_EQ(c_from_rref.use_count(), 0);
            }
        }
        {
            emu_test::shared_vector obj{1,2,3};
            const auto& cref = obj;

            {
                view_type c_from_obj{obj};
                EXPECT_EQ(c_from_obj.use_count(), 1);
            }
            {
                view_type c_from_cref{cref};
                EXPECT_EQ(c_from_cref.use_count(), 1);
            }
            {
                view_type c_from_rref{std::move(obj)};
                EXPECT_EQ(c_from_rref.use_count(), 1);
            }
        }
        {
            emu_test::spy_flag sf;

            int value = 1;
            emu_test::SpyContainer spy_con{&sf, &value};

            {
                // Take ownership of the container.
                const view_type con( std::move(spy_con) );

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
        {
            emu_test::spy_flag sf;

            int value = 1;
            emu_test::SpyContainer spy_con{&sf, &value};

            {
                // Take spy_con by reference.
                const view_type con( spy_con );

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
        {
            emu_test::spy_flag sf;

            int value = 1;
            const emu_test::SpyContainer spy_con{&sf, &value};

            {
                // Take spy_con by constant reference.
                const const_view_type con( spy_con );

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
    }

    TYPED_TEST_P(ViewTest, Wrap)
    {
        using view_type = typename TestFixture::view_type;
        using data_type = typename TestFixture::data_type;

        std::array<data_type, 5> arr = {1, 2, 3, 4, 5};

        const view_type con{ arr };

        emu_test::expect_address(con, arr.data());
        EXPECT_EQ(con.size(), arr.size());
        EXPECT_EQ(con.use_count(), 0);
    }

    TYPED_TEST_P(ViewTest, FreeCapsule)
    {
        using const_view_type = typename TestFixture::const_view_type;
        using data_type = typename TestFixture::data_type;

        const std::array<data_type, 5> arr = {1, 2, 3, 4, 5};

        bool called = false;

        {
            const_view_type con = TestFixture::from_ptr_and_size(arr.data(), arr.size(), emu::scoped([&called]() { called = true; }));
        }

        EXPECT_TRUE(called);
    }

} // namespace
