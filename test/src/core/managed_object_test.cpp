#include <gtest/gtest.h>
#include <test_utility/spy.hpp>

#include <emu/managed_object.hpp>

#include <string_view>

using namespace emu_test;

class ManagedSpy : public emu::managed_object<std::string, ManagedSpy>
{

    using base_t = emu::managed_object<std::string, ManagedSpy>;
    friend base_t;

    Spy spy;
protected:
    ManagedSpy(spy_flag *flag) : spy(flag) {}

public:

    static std::shared_ptr<ManagedSpy> create(std::string name, spy_flag *flag) {
        return try_emplace(std::move(name), flag).first;
    }

    static std::shared_ptr<ManagedSpy> open(std::string_view name) {
        return get(name);
    }

};

namespace
{
    TEST(ManagedObject, Lifetime)
    {
        spy_flag flag;

        auto sp1 = ManagedSpy::create("sp1", &flag);
        auto sp2 = ManagedSpy::open("sp1");

        EXPECT_EQ(sp1, sp2);

        EXPECT_FALSE(flag.copied) << "Should never be copied";
        EXPECT_FALSE(flag.moved) << "Should never be moved";
        EXPECT_FALSE(flag.destroyed) << "Shoule not be destroyed yet";

        sp1.reset();

        EXPECT_FALSE(flag.destroyed);

        sp2.reset();

        EXPECT_TRUE(flag.destroyed);

        auto sp3 = ManagedSpy::open("sp3");

        EXPECT_FALSE(sp3);
    }

    TEST(ManagedObject, SharedFromThis)
    {
        spy_flag flag;

        std::shared_ptr<ManagedSpy> sp1;
        {
            auto sp2 = ManagedSpy::create("sp1", &flag);

            sp1 = sp2->getptr();

            EXPECT_EQ(sp1, sp2);
        }

        EXPECT_FALSE(flag.copied) << "Should never be copied";
        EXPECT_FALSE(flag.moved) << "Should never be moved";
        EXPECT_FALSE(flag.destroyed) << "Shoule not be destroyed yet";

        sp1.reset();

        EXPECT_TRUE(flag.destroyed);
    }

} // namespace
