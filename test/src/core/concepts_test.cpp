#include "emu/type_traits.hpp"
#include <concepts>
#include <gtest/gtest.h>
#include <ranges>

#include <emu/container.hpp>

#include <vector>
#include <array>
#include <span>

struct Relocatable : std::ranges::view_base
{
    int* begin() const { return nullptr;}
    int* end()   const { return nullptr;}
};

template<>
constexpr bool emu::spe::enable_relocatable_owning_range<Relocatable> = true;

namespace
{


    TEST(Concepts, RelocatableRange)
    {
        static_assert(emu::cpts::relocatable_owning_range<std::vector<int>> == true );
        static_assert(emu::cpts::relocatable_owning_range<std::span<int>  > == false);

        static_assert(emu::cpts::relocatable_owning_range<std::array<int, 3>> == false);
        static_assert(emu::cpts::relocatable_owning_range<int[ ]            > == false);
        static_assert(emu::cpts::relocatable_owning_range<int[3]            > == false);

        static_assert(emu::cpts::relocatable_owning_range<Relocatable   > == true );
    }

    TEST(Concepts, RelocatableRangeReference)
    {
        static_assert(emu::cpts::relocatable_owning_range<std::vector<int> &      > == false);
        static_assert(emu::cpts::relocatable_owning_range<std::vector<int> const &> == false);
        static_assert(emu::cpts::relocatable_owning_range<std::vector<int> &&     > == true );

        static_assert(emu::cpts::relocatable_owning_range<std::span<int> &      > == false);
        static_assert(emu::cpts::relocatable_owning_range<std::span<int> const &> == false);
        static_assert(emu::cpts::relocatable_owning_range<std::span<int> &&     > == false);

        static_assert(emu::cpts::relocatable_owning_range<std::array<int, 3> &      > == false);
        static_assert(emu::cpts::relocatable_owning_range<std::array<int, 3> const &> == false);
        static_assert(emu::cpts::relocatable_owning_range<std::array<int, 3> &&     > == false);

        static_assert(emu::cpts::relocatable_owning_range<Relocatable &      > == true );
        static_assert(emu::cpts::relocatable_owning_range<Relocatable const &> == true );
        static_assert(emu::cpts::relocatable_owning_range<Relocatable &&     > == true );
    }

} // namespace
