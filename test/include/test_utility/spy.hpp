#pragma once

#include <utility>
#include <ranges>

namespace emu_test
{
    struct spy_flag
    {
        bool copied = false;
        bool moved = false;
        bool destroyed = false;
    };

    struct Spy
    {

        mutable spy_flag *flag;

        Spy(spy_flag *flag):
            flag(flag)
        {}

        Spy(const Spy& s) noexcept:
            flag(std::exchange(s.flag, nullptr))
        {
            if (flag) flag->copied = true;
        }

        Spy(Spy&& s) noexcept:
            flag(std::exchange(s.flag, nullptr))
        {
            if (flag) flag->moved = true;
        }

        Spy& operator=(const Spy & s) noexcept {
            if (&s == this) return *this;

            flag = std::exchange(s.flag, nullptr);
            return *this;
        }

        Spy& operator=(Spy && s) noexcept {
            if (&s == this) return *this;

            flag = std::exchange(s.flag, nullptr);
            return *this;
        }

        ~Spy()
        {
            if (flag) flag->destroyed = true;
        }
    };

    struct SpyContainer : Spy
    {
        using value_type = int;
        using element_type = int;

        SpyContainer(spy_flag* sf, int *ptr)
            : Spy(sf)
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

    struct SpyAddress {
        SpyAddress **snitch = nullptr;

        SpyAddress(SpyAddress **snitch) : snitch(snitch) { *snitch = this; }

        SpyAddress(const SpyAddress& other) { *this = other; }
        SpyAddress(SpyAddress&& other) noexcept { *this = other; }

        SpyAddress& operator=(const SpyAddress & other) {
            snitch = other.snitch;
            *snitch = this;
            return *this;
        }

        SpyAddress& operator=(SpyAddress && other)  noexcept {
            snitch = other.snitch;
            *snitch = this;
            return *this;
        }

        ~SpyAddress() {
            *snitch = nullptr;
        }
    };

} // namespace emu_test

// destroying SpyContainer invalidates all references to its elements.
template <>
inline constexpr bool std::ranges::enable_borrowed_range<emu_test::SpyContainer> = true;
