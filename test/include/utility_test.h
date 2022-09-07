#ifndef EMU_TEST_UTILITY_TEST_H
#define EMU_TEST_UTILITY_TEST_H

namespace emu
{

    struct Spy
    {
        bool copied = false;
        bool moved = false;

        Spy() = default;

        Spy(const Spy& s):
            copied(true), moved(s.moved)
        {}

        Spy(Spy&& s) noexcept:
            copied(s.copied), moved(true)
        {}
    };

    struct spy_flag
    {
        bool copied = false;
        bool moved = false;
        bool destroyed = false;
    };

    struct Spy2
    {
        spy_flag *flag;

        Spy2(spy_flag *flag):
            flag(flag)
        {}

        Spy2(const Spy2& s):
            flag(s.flag)
        {
            flag->copied = true;
        }

        Spy2(Spy2&& s) noexcept:
            flag(s.flag)
        {
            flag->moved = true;
        }

        ~Spy2()
        {
            flag->destroyed = true;
        }
    };


}

#endif //EMU_TEST_UTILITY_TEST_H