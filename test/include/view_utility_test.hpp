#pragma once

#include <emu/concepts.hpp>
#include <emu/type_traits.hpp>

#include <span>

namespace emu_test {


    template < emu::cpts::mdspan MD >
    void expect_rank(const MD &md, std::size_t expected_rank) {
        EXPECT_EQ(md.rank(), expected_rank);
    }

    template < std::ranges::contiguous_range CR >
    void expect_address(const CR &cr, emu::range_cv_value<CR>* expected_address) {
        EXPECT_EQ(std::ranges::data(cr), expected_address);
    }

    template < emu::cpts::mdspan MD>
    void expect_extents(const MD &md, std::span<std::size_t> expected_extents) {
        expect_rank(md, expected_extents.size());
        for (std::size_t i = 0; i < expected_extents.size(); i++) {
            EXPECT_EQ(md.extent(i), expected_extents[i]);
        }
    }

    template < emu::cpts::mdspan MD >
    void check_mdspan(const MD& md, std::size_t expected_rank, const emu::range_cv_value<MD>* expected_address, std::span<std::size_t> expected_extents) {
        expect_rank(md, expected_rank);
        expect_address(md, expected_address);
        expect_extents(md, expected_extents);
    }


    template < emu::cpts::capsule_owner CO >
    void expect_use_count(const CO &co, std::size_t expected_use_count) {
        EXPECT_EQ(co.use_count(), expected_use_count);
    }

    template < emu::cpts::mdcontainer MC>
    void check(const MC& mc, std::size_t expected_rank, const emu::range_cv_value<MC>* expected_address, std::span<std::size_t> expected_extents, std::size_t expected_use_count) {
        check_mdspan(mc, expected_rank, expected_address, expected_extents);

        expect_use_count(mc, expected_use_count);
    }

} // namespace emu_test
