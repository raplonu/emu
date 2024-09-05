#include <gtest/gtest.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h> // everything needed for embedding
// #include <fmt/core.h>

namespace
{

    struct GilEnvironment : ::testing::Environment {

        // start the interpreter and keep it alive
        pybind11::scoped_interpreter guard;

    };

    gsl::owner<const testing::Environment* const> gil_env = testing::AddGlobalTestEnvironment(new GilEnvironment);

} // namespace
