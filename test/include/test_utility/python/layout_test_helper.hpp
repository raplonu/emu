#pragma once

#include <emu/mdspan.hpp>

#include <utility>

namespace emu_test
{
    /**
     * @brief Generic agregate to hold a mapping along with the required data size
     *
     * @tparam MappingType
     */
    template<typename MappingType>
    struct MappingAndRequiredDataSize {
        MappingType mapping;
        std::size_t required_data_size; // Not the same as required_span_size since it accounts for begin and end offsets
    };

    struct layout_contiguous_helper {

        template<typename ExtentType>
        static auto get_mappings() {
            static_assert(ExtentType::rank() == 1, "layout_contiguous_helper only supports rank 1");

            using result_type = MappingAndRequiredDataSize<std::size_t>;

            return std::vector<result_type>{
                // {0, 0}, Note: zero-sized spans are does not produce expected behavior in pybind11.
                {1, 1},
                {5, 5},
                {10, 10},
                {100, 100}
            };
        }

    };

    struct layout_right_helper {

        using layout_type = emu::layout_right;

        template<typename ExtentType>
        static auto get_mappings() {

            using mapping_type = emu::layout_right::template mapping<ExtentType>;

            using result_type = MappingAndRequiredDataSize<mapping_type>;

            std::vector<result_type> mappings;
            if constexpr (ExtentType::rank() == 1) {
                // mappings.push_back({mapping_type(ExtentType{0}), 0});
                mappings.push_back({mapping_type(ExtentType{1}), 1});
                mappings.push_back({mapping_type(ExtentType{5}), 5});
            } else if constexpr (ExtentType::rank() == 2) {
                // mappings.push_back({mapping_type(ExtentType{0, 0}), 0});
                mappings.push_back({mapping_type(ExtentType{1, 1}), 1});
                mappings.push_back({mapping_type(ExtentType{3, 4}), 12});
            } else if constexpr (ExtentType::rank() == 3) {
                // mappings.push_back({mapping_type(ExtentType{0, 0, 0}), 0});
                mappings.push_back({mapping_type(ExtentType{1, 1, 1}), 1});
                mappings.push_back({mapping_type(ExtentType{2, 3, 4}), 24});
            }
            return mappings;
        }

    };


    struct layout_left_helper {

        using layout_type = emu::layout_left;

        template<typename ExtentType>
        static auto get_mappings() {

            using mapping_type = emu::layout_left::template mapping<ExtentType>;

            using result_type = MappingAndRequiredDataSize<mapping_type>;

            std::vector<result_type> mappings;
            if constexpr (ExtentType::rank() == 1) {
                // mappings.push_back({mapping_type(ExtentType{0}), 0});
                mappings.push_back({mapping_type(ExtentType{1}), 1});
                mappings.push_back({mapping_type(ExtentType{5}), 5});
            } else if constexpr (ExtentType::rank() == 2) {
                // mappings.push_back({mapping_type(ExtentType{0, 0}), 0});
                mappings.push_back({mapping_type(ExtentType{1, 1}), 1});
                mappings.push_back({mapping_type(ExtentType{3, 4}), 12});
            } else if constexpr (ExtentType::rank() == 3) {
                // mappings.push_back({mapping_type(ExtentType{0, 0, 0}), 0});
                mappings.push_back({mapping_type(ExtentType{1, 1, 1}), 1});
                mappings.push_back({mapping_type(ExtentType{2, 3, 4}), 24});
            }
            return mappings;
        }

    };

    struct layout_stride_helper {

        using layout_type = emu::layout_stride;

        template<typename ExtentType>
        static auto get_mappings() {

            using mapping_type = emu::layout_stride::template mapping<ExtentType>;

            using result_type = MappingAndRequiredDataSize<mapping_type>;

            std::vector<result_type> mappings;
            if constexpr (ExtentType::rank() == 1) {
                // mappings.push_back({mapping_type(ExtentType{0}, std::array{0}), 0});
                mappings.push_back({mapping_type(ExtentType{1}, std::array{1}), 1});
                mappings.push_back({mapping_type(ExtentType{5}, std::array{2}), 5});
            } else if constexpr (ExtentType::rank() == 2) {
                // mappings.push_back({mapping_type(ExtentType{0, 0}, std::array{0, 0}), 0});
                mappings.push_back({mapping_type(ExtentType{1, 1}, std::array{1, 2}), 1});
                mappings.push_back({mapping_type(ExtentType{3, 4}, std::array{5, 3}), 12});
            } else if constexpr (ExtentType::rank() == 3) {
                // mappings.push_back({mapping_type(ExtentType{0, 0, 0}, std::array{0, 0, 0}), 0});
                mappings.push_back({mapping_type(ExtentType{1, 1, 1}, std::array{1, 2, 3}), 1});
                mappings.push_back({mapping_type(ExtentType{2, 3, 4}, std::array{12, 4, 2}), 24});
            }
            return mappings;
        }

    };


    using ExtentTypeList = std::tuple< emu::dims<1>, emu::dims<2>, emu::dims<3> >;

    using LayoutHelperList = std::tuple<
        layout_right_helper,
        layout_left_helper,
        layout_stride_helper
    >;


} // namespace emu_test
