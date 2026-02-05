#include <gtest/gtest.h>

#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>
#include <test_utility/python/capsule_test.hpp>
#include <test_utility/python/layout_test_helper.hpp>
#include <test_utility/python/accessor_test_helper.hpp>

#include <emu/mdcontainer.hpp>

#include <emu/pybind11/cast/mdspan.hpp>

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView,
        CppToPythonToCppCapsule
    );

    template<typename T, typename ExtentType, typename LayoutHelper>
    struct mdcontainer_test_helper : emu_test::ViewTestHelperBase< mdcontainer_test_helper<T, ExtentType, LayoutHelper> >
    {
        using accessor_helper = emu_test::default_accessor_test_helper;
        using layout_helper = LayoutHelper;

        using element_type = T;
        using extent_type = ExtentType;
        using layout_type = typename layout_helper::layout_type;
        using accessor_type = typename accessor_helper::template accessor_type<element_type>;
        using const_accessor_type = typename accessor_helper::template accessor_type<const element_type>;

        using view_type = emu::mdcontainer<element_type, ExtentType, layout_type, accessor_type>;
        using const_view_type = emu::mdcontainer<const element_type, ExtentType, layout_type, const_accessor_type>;

        static view_type get_view(std::unique_ptr<element_type[]>& data, auto mapping) {
            auto ptr = data.get();

            return view_type(ptr, std::move(data), mapping);
        }

        static const_view_type get_const_view(std::unique_ptr<element_type[]>& data, auto mapping) {
            auto ptr = data.get();

            return const_view_type(ptr, std::move(data), mapping);
        }
    };

    using MdContainerTestsList = emu_test::cartesian_product_test_type<
        mdcontainer_test_helper,
        emu_test::DataTypeList,
        emu_test::ExtentTypeList,
        emu_test::LayoutHelperList
    >;

    INSTANTIATE_TYPED_TEST_SUITE_P(MdContainerPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                MdContainerTestsList);  // Type list

} // namespace
