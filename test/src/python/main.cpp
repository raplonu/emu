#include <gtest/gtest.h>
#include <emu/config.hpp>
#include <emu/pybind11.hpp>
#include <pybind11/embed.h> // everything needed for embedding
#include <gsl/pointers>

#include <fmt/format.h>

struct DLL_LOCAL GilEnvironment : ::testing::Environment {

    // start the interpreter and keep it alive
    pybind11::scoped_interpreter guard;
};

int main(int argc, char **argv) {
    testing::AddGlobalTestEnvironment(std::make_unique<GilEnvironment>().release());
    testing::InitGoogleTest(&argc, argv);

    auto res = RUN_ALL_TESTS();

    try {
        // The interpreter is automatically finalized when the guard goes out of scope.
        // But before, we can check for any Python errors that might have occurred.
        emu::pybind11::check_error();
    } catch (const pybind11::error_already_set& e) {
        fmt::print(stderr, "Python error occurred: {}\n", e.what());
        return 1; // Return a non-zero value to indicate an error
    }

    return res;
}
