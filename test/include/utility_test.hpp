#pragma once

namespace emu
{
    struct spy_flag
    {
        bool copied = false;
        bool moved = false;
        bool destroyed = false;
    };

    struct Spy
    {

        spy_flag *flag;

        Spy(spy_flag *flag):
            flag(flag)
        {}

        Spy(Spy& s):
            flag(std::exchange(s.flag, nullptr))
        {
            if (flag) flag->copied = true;
        }

        Spy(Spy&& s) noexcept:
            flag(std::exchange(s.flag, nullptr))
        {
            if (flag) flag->moved = true;
        }

        ~Spy()
        {
            if (flag) flag->destroyed = true;
        }
    };

    struct SpyContainer : emu::Spy
    {
        using value_type = int;
        using element_type = int;

        SpyContainer(emu::spy_flag* sf, int *ptr)
            : emu::Spy(sf)
            , ptr(ptr)
        {}

        int* ptr;

        int*        data ()       { return ptr; }
        const int*  data () const { return ptr; }
        int*        begin()       { return ptr; }
        const int*  begin() const { return ptr; }
        int*        end  ()       { return ptr + 1; }
        const int*  end  () const { return ptr + 1; }
        std::size_t size () const { return 1; }
    };

    static_assert(std::ranges::range<SpyContainer>);
    static_assert(std::ranges::sized_range<SpyContainer>);
    static_assert(std::ranges::contiguous_range<SpyContainer>);

} // namespace emu

// destroying SpyContainer invalidates all references to its elements.
template <>
inline constexpr bool std::ranges::enable_borrowed_range<emu::SpyContainer> = false;
