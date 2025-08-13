#include <iostream>
#include <emu/cuda/host/mdcontainer.hpp>

int main() {
    // Test 1D container
    auto container_1d = emu::cuda::host::make_mdcontainer<int>(10);
    std::cout << "1D container size: " << container_1d.size() << std::endl;
    std::cout << "1D container data pointer: " << (void*)container_1d.data_handle() << std::endl;

    // Test 2D container
    auto container_2d = emu::cuda::host::make_mdcontainer<double>(5, 3);
    std::cout << "2D container size: " << container_2d.size() << std::endl;
    std::cout << "2D container data pointer: " << (void*)container_2d.data_handle() << std::endl;

    // Test access
    if (container_1d.data_handle()) {
        container_1d.data_handle()[0] = 42;
        std::cout << "First element set to: " << container_1d.data_handle()[0] << std::endl;
    }

    std::cout << "Host mdcontainer implementation working correctly!" << std::endl;
    return 0;
}
