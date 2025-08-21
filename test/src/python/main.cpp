#include <gtest/gtest.h>
#include <emu/config.hpp>
#include <emu/pybind11.hpp>
#include <pybind11/embed.h> // everything needed for embedding
#include <gsl/pointers>

#include <fmt/format.h>

struct DLL_LOCAL GilEnvironment : ::testing::Environment {

    // start the interpreter and keep it alive
    pybind11::scoped_interpreter guard;


    ~GilEnvironment() {
        // The interpreter is automatically finalized when the guard goes out of scope.
        // But before, we can check for any Python errors that might have occurred.
        emu::pybind11::check_error();
    }

};

int main(int argc, char **argv) {
    testing::AddGlobalTestEnvironment(std::make_unique<GilEnvironment>().release());
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
