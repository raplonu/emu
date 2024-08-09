#pragma once

#include <gtest/gtest.h>

namespace
{

    template<typename T>
    struct ViewTest : T, testing::Test { };

    TYPED_TEST_SUITE_P(ViewTest);

} // namespace
