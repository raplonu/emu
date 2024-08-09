#pragma once

#include <gtest/gtest.h>

namespace
{

    template<typename T>
    struct PythonViewTest : T, testing::Test { };

    TYPED_TEST_SUITE_P(PythonViewTest);

} // namespace
