#include <gtest/gtest.h>
#include <emu/config.hpp>
#include <pybind11/embed.h> // everything needed for embedding
#include <gsl/pointers>

struct DLL_LOCAL GilEnvironment : ::testing::Environment {

    // start the interpreter and keep it alive
    pybind11::scoped_interpreter guard;

};

int main(int argc, char **argv) {
    testing::AddGlobalTestEnvironment(std::make_unique<GilEnvironment>().release());
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
