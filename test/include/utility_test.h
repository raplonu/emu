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

}

#endif //EMU_TEST_UTILITY_TEST_H