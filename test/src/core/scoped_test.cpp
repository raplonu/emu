#include <gtest/gtest.h>

#include <emu/scoped.hpp>

#include <sstream>

namespace
{
    struct resource_t {
        bool open;
    };

    resource_t open_resource() {
        return {true};
    }

    void close_resource(resource_t& x) {
        x.open = false;
    }

    struct Closer{
        void operator()(resource_t& x) const {
            close_resource(x);
        }
    };

    /// This is the canonical example of scoped_t as a RAII resource owner.
    TEST(scoped_test, free_resource) {
        using namespace emu;
        resource_t r = open_resource();
        {
            auto s = scoped(std::ref(r), close_resource);
            EXPECT_TRUE(r.open);
        }
        EXPECT_FALSE(r.open);
    }

    /// This is the canonical example of scoped_t as a RAII resource owner.
    TEST(scoped_test, dont_free_resource) {
        using namespace emu;
        resource_t r = open_resource();
        {
            auto s = scoped(std::ref(r), close_resource, false);
            EXPECT_TRUE(r.open);
        }
        EXPECT_TRUE(r.open);
    }

    /// Here, scoped_t is used not to free a resource, but to restore an initial state.
    /// We want to restore the io flags on scope exit.
    TEST(scoped_test, restore_state) {
        using namespace emu;
        using namespace std;
        ostringstream ss;
        ss.setf(ios::dec);
        {
            // setf() returns the old flags and we store them in the scoped_t object.
            // The lambda will set them again.
            auto s = scoped(ss.setf(ios::hex, ios::basefield), [&](ios::fmtflags previous) {
            ss.setf(previous, ios::basefield);
            });
            ss << 255;
            EXPECT_EQ("ff", ss.str());
        }
        ss.str("");
        ss << 255;
        EXPECT_EQ("255", ss.str());
    }

    /// Sometimes, we don't have a value and just want something to be done on
    /// scope exit.
    TEST(scoped_test, scoped_void) {
        using namespace emu;
        using namespace std;
        ostringstream ss;
        {
            auto s = scoped([&]{ss << "out of scope";});
            ss << "in scope - ";
            EXPECT_EQ("in scope - ", ss.str());
        }
        EXPECT_EQ("in scope - out of scope", ss.str());
    }

    TEST(scoped_test, scoped_assign)
    {
        bool b = true;
        int i = 0;

        {
            EMU_ASSIGN_FOR_CURRENT_SCOPE(b, false);
            EMU_ASSIGN_FOR_CURRENT_SCOPE(i, 42);

            EXPECT_FALSE(b);
            EXPECT_EQ(i, 42);
        }

        EXPECT_TRUE(b);
        EXPECT_EQ(i, 0);
    }

    TEST(scoped_test, scoped_exit)
    {
        bool b = false;
        int i = 42;

        {
            EMU_INVOKE_AT_SCOPE_EXIT([&b]{ b = true; });
            EMU_INVOKE_AT_SCOPE_EXIT([&i]{ i = 0; });

            EXPECT_FALSE(b);
            EXPECT_EQ(i, 42);
        }

        EXPECT_TRUE(b);
        EXPECT_EQ(i, 0);
    }
} // namespace
