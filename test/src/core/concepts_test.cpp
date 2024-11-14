#include "emu/concepts.hpp"
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
    int value = 42;
    [[nodiscard]] int* begin() { return &value;}
    [[nodiscard]] int* end()   { return &value + 1;}
};

template<>
constexpr bool emu::spe::enable_relocatable_owning_range<Relocatable> = true;

namespace
{


    TEST(Concepts, RelocatableRange)
    {
        static_assert(    emu::cpts::relocatable_owning_range<std::vector<int>>);
        static_assert(not emu::cpts::relocatable_owning_range<std::span<int>  >);

        static_assert(not emu::cpts::relocatable_owning_range<std::array<int, 3>>);
        static_assert(not emu::cpts::relocatable_owning_range<int[ ]            >);
        static_assert(not emu::cpts::relocatable_owning_range<int[3]            >);

        static_assert(    emu::cpts::relocatable_owning_range<Relocatable   >);
    }

    TEST(Concepts, RelocatableRangeReference)
    {
        static_assert(not emu::cpts::relocatable_owning_range<std::vector<int> &      >);
        static_assert(not emu::cpts::relocatable_owning_range<std::vector<int> const &>);
        static_assert(    emu::cpts::relocatable_owning_range<std::vector<int> &&     >);

        static_assert(not emu::cpts::relocatable_owning_range<std::span<int> &      >);
        static_assert(not emu::cpts::relocatable_owning_range<std::span<int> const &>);
        static_assert(not emu::cpts::relocatable_owning_range<std::span<int> &&     >);

        static_assert(not emu::cpts::relocatable_owning_range<std::array<int, 3> &      >);
        static_assert(not emu::cpts::relocatable_owning_range<std::array<int, 3> const &>);
        static_assert(not emu::cpts::relocatable_owning_range<std::array<int, 3> &&     >);

        static_assert(    emu::cpts::relocatable_owning_range<Relocatable &      >);
        static_assert(    emu::cpts::relocatable_owning_range<Relocatable const &>);
        static_assert(    emu::cpts::relocatable_owning_range<Relocatable &&     >);
    }

    TEST(Concepts, ViewAndContainer)
    {
        // A span is an any_span but not an any_container.
        static_assert(    emu::cpts::any_span<emu::span<int>>);
        static_assert(not emu::cpts::any_container<emu::span<int>>);

        // A container is an any_span and also an any_container.
        static_assert(    emu::cpts::any_span<emu::container<int>>);
        static_assert(    emu::cpts::any_container<emu::container<int>>);
    }

} // namespace
