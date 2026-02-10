cmake_minimum_required(VERSION 3.25)
project(test_package LANGUAGES CXX)

add_executable(${PROJECT_NAME} test_package.cpp)

find_package(emu REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE emu::emu)

find_package(CUDAToolkit QUIET)
if(CUDAToolkit_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart CUDA::cublas CUDA::cusolver CUDA::cusparse)
endif()
