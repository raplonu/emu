cmake_minimum_required(VERSION 3.18)

project(emu VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(emu_build_cuda "Build cuda module" OFF)

if (emu_build_cuda)
    enable_language(CUDA)
endif()

########################################################################
#
# Emu Core
#

add_library(emucore)

target_sources(emucore
    PRIVATE
        src/core/fun.cpp
)

target_sources(emucore
    PUBLIC FILE_SET HEADERS BASE_DIRS include/core FILES
        include/core/emu/blas.hpp
        include/core/emu/tuple.hpp
        include/core/emu/mdalgo.hpp
        include/core/emu/math.hpp
        include/core/emu/fun.hpp
        include/core/emu/assert.hpp
        include/core/emu/detail/error.hpp
        include/core/emu/config.hpp
        include/core/emu/range.hpp
        include/core/emu/functional/misc.hpp
        include/core/emu/info.hpp
        include/core/emu/cstring_view.hpp
        include/core/emu/fwd.hpp
        include/core/emu/hash.hpp
        include/core/emu/macro.hpp
        include/core/emu/mdcontainer.hpp
        include/core/emu/mdspan.hpp
        include/core/emu/numeric_type.hpp
        include/core/emu/type_name.hpp
        include/core/emu/type_traits.hpp
        include/core/emu/unordered_map.hpp
        include/core/emu/unordered_set.hpp
        include/core/emu/utility.hpp
        include/core/emu/scoped.hpp
        include/core/emu/capsule.hpp
        include/core/emu/concepts.hpp
        include/core/emu/container.hpp
        include/core/emu/span.hpp
        include/core/emu/storage.hpp
        include/core/emu/expected.hpp
        include/core/emu/functor.hpp
        include/core/emu/optional.hpp
        include/core/emu/ostream.hpp
)

target_compile_features(emucore PUBLIC cxx_std_20)

if(emu_build_cuda)
    target_compile_definitions(emucore PUBLIC EMU_CUDA)
endif()

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(mdspan REQUIRED)
find_package(tl-expected REQUIRED)
find_package(tl-optional REQUIRED)
find_package(half REQUIRED)

target_link_libraries(emucore PUBLIC
    Boost::boost
    fmt::fmt
    Microsoft.GSL::GSL
    tl::expected
    tl::optional
    std::mdspan
    half::half
)

add_library(emu::core ALIAS emucore)

install(TARGETS emucore
    FILE_SET HEADERS
)

########################################################################
#
# Emu CUDA
#

if (emu_build_cuda)
    message(CHECK_START "Emu - Configuring CUDA support")

    add_library(emucuda)

    target_sources(emucuda
        PRIVATE
            src/cuda/cublas/handle.cpp
            src/cuda/cublas/error.cpp
            src/cuda/cublas.cpp

            # src/cuda/cufft/handle.cpp
            # src/cuda/cufft/error.cpp
            # src/cuda/cufft.cpp

            src/cuda/cusolver/handle.cpp
            src/cuda/cusolver/error.cpp
            src/cuda/cusolver.cpp

            src/cuda/thrust/execution_policy.cu
    )

    target_sources(emucuda
        PUBLIC FILE_SET HEADERS BASE_DIRS include/cuda FILES
            include/cuda/emu/config.cuh
            include/cuda/emu/cublas.hpp
            include/cuda/emu/macro.cuh
            include/cuda/emu/thrust.cuh
            include/cuda/emu/cuda/allocator.hpp
            include/cuda/emu/cuda/mdspan.hpp
            include/cuda/emu/cuda/stream.hpp
            include/cuda/emu/cuda/container.hpp
            include/cuda/emu/cuda/device.hpp
            include/cuda/emu/cuda/device/span.hpp
            include/cuda/emu/cublas/error.hpp
            include/cuda/emu/cublas/type.hpp
            include/cuda/emu/cublas/handle.hpp
            include/cuda/emu/utility.cuh
            include/cuda/emu/atomic.cuh
            include/cuda/emu/cusolver.hpp
            include/cuda/emu/cufft/error.hpp
            include/cuda/emu/cufft/type.hpp
            include/cuda/emu/cufft/utility.hpp
            include/cuda/emu/cufft/handle.hpp
            include/cuda/emu/cusolver/error.hpp
            include/cuda/emu/cusolver/handle.hpp
            include/cuda/emu/memory.cuh
            include/cuda/emu/cufft.hpp
            include/cuda/emu/iterator/coordinate.cuh
            include/cuda/emu/iterator/function.cuh
            include/cuda/emu/thrust/execution_policy.cuh
            include/cuda/emu/cuda.hpp
            include/cuda/emu/tensor.hpp
    )

    target_include_directories(emucuda PUBLIC include/cuda)

    target_compile_options(emucuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            --extended-lambda
            --expt-relaxed-constexpr
        >
    )
    # Only needed for CUDA source file but it's easier to just set it
    target_compile_definitions(emucuda PUBLIC FMT_USE_CONSTEXPR=1)

    find_package(CUDAToolkit REQUIRED)
    find_package(cuda-api-wrappers REQUIRED)
    find_package(matx REQUIRED)

    target_link_libraries(emucuda PUBLIC
        emucore
        CUDA::cublas
        CUDA::cudart
        CUDA::cuda_driver
        cuda-api-wrappers::cuda-api-wrappers
        matx::matx
    )

    add_library(emu::cuda ALIAS emucuda)

    install(TARGETS emucuda
        FILE_SET HEADERS
    )

    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Test
#
include(CTest)
if (BUILD_TESTING)
    message(CHECK_START "Emu - Configuring test")
    add_subdirectory(test)
    message(CHECK_PASS "Done")
endif()
