cmake_minimum_required(VERSION 3.18)

# Have to define this variable to let vcpkg know that we need gtest or not.
if(BUILD_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES "test")
endif()

# Note: This is not possible to build python test using vcpkg.
option(emu_build_python_test "Enable python test module" OFF)
option(emu_build_cuda "Enable cuda module" OFF)

# other otions:

# Note: This is not possible to change this value using vcpkg.
set(emu_boost_namespace "boost" CACHE STRING "Custom boost namespace name")

########################################################################
#
# Emu Project
#

project(emu VERSION 1.0.0 LANGUAGES CXX)

# Always generate compile_commands.json. Used by clangd and other tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# emu is not pedantic... yet
# It relies on `statement-expression extension` that is not standard
# See: https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
#
# Keep an eye on `https://wg21.link/P2561` for a possible alternative.
add_compile_options(-Wno-pedantic)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

########################################################################
#
# Emu Core
#

add_library(emucore)

target_sources(emucore
    PRIVATE
        src/core/error.cpp
        src/core/pointer.cpp
        src/core/dlpack.cpp
        src/core/numeric_type.cpp
)

target_sources(emucore PUBLIC FILE_SET HEADERS BASE_DIRS include/core FILES
    include/core/emu/blas.hpp
    include/core/emu/tuple.hpp
    include/core/emu/capsule.hpp
    include/core/emu/charconv.hpp
    include/core/emu/assert.hpp
    include/core/emu/detail/error.hpp
    include/core/emu/detail/basic_container.hpp
    include/core/emu/detail/basic_mdcontainer.hpp
    include/core/emu/detail/basic_mdspan.hpp
    include/core/emu/detail/basic_span.hpp
    include/core/emu/detail/dlpack_types.hpp
    include/core/emu/detail/mdspan_types.hpp
    include/core/emu/functional/misc.hpp
    include/core/emu/storage.hpp
    include/core/emu/optional.hpp
    include/core/emu/ostream.hpp
    include/core/emu/pointer.hpp
    include/core/emu/utility.hpp
    include/core/emu/managed_object.hpp
    include/core/emu/scoped.hpp
    include/core/emu/associative_container.hpp
    include/core/emu/range.hpp
    include/core/emu/concepts.hpp
    include/core/emu/config.hpp
    include/core/emu/const_string.hpp
    include/core/emu/container.hpp
    include/core/emu/cstring_view.hpp
    include/core/emu/debug.hpp
    include/core/emu/dlpack.hpp
    include/core/emu/error.hpp
    include/core/emu/owning_reference.hpp
    include/core/emu/hash.hpp
    include/core/emu/mdalgo.hpp
    include/core/emu/mdcontainer.hpp
    include/core/emu/mdspan.hpp
    include/core/emu/numeric_type.hpp
    include/core/emu/span.hpp
    include/core/emu/info.hpp
    include/core/emu/type_name.hpp
    include/core/emu/expected.hpp
    include/core/emu/functor.hpp
    include/core/emu/fwd.hpp
    include/core/emu/host/container.hpp
    include/core/emu/host/location_policy.hpp
    include/core/emu/host/mdcontainer.hpp
    include/core/emu/host/mdspan.hpp
    include/core/emu/host/span.hpp
    include/core/emu/location_policy.hpp
    include/core/emu/macro.hpp
    include/core/emu/math.hpp
    include/core/emu/type_traits.hpp
    include/core/tl/expected.hpp
    include/core/tl/optional.hpp
    include/core/dlpack/dlpack.h
    include/core/half.hpp
)


target_compile_features(emucore PUBLIC cxx_std_20)

if(emu_build_cuda)
    target_compile_definitions(emucore PUBLIC EMU_CUDA)
endif()

target_compile_definitions(emucore PUBLIC EMU_BOOST_NAMESPACE=${emu_boost_namespace})

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(mdspan REQUIRED)

target_link_libraries(emucore PUBLIC
    Boost::boost
    fmt::fmt
    Microsoft.GSL::GSL
    std::mdspan
)

add_library(emu::core ALIAS emucore)

install(TARGETS emucore
    EXPORT ${PROJECT_NAME}-targets
    FILE_SET HEADERS
)

########################################################################
#
# Emu CUDA
#

if (emu_build_cuda)
    enable_language(CUDA)

    add_library(emucuda)

    target_sources(emucuda
        PRIVATE
            src/cuda/cublas/handle.cpp
            src/cuda/cublas/error.cpp
            src/cuda/cublas.cpp

            src/cuda/cuda/pointer.cpp
            src/cuda/cuda/error.cpp

            # src/cuda/cufft/handle.cpp
            # src/cuda/cufft/error.cpp
            # src/cuda/cufft.cpp

            src/cuda/cusolver/handle.cpp
            src/cuda/cusolver/error.cpp
            src/cuda/cusolver.cpp

            src/cuda/thrust/execution_policy.cu
    )

    target_sources(emucuda PUBLIC FILE_SET HEADERS BASE_DIRS include/cuda FILES
        include/cuda/emu/config.cuh
        include/cuda/emu/macro.cuh
        include/cuda/emu/thrust.cuh
        include/cuda/emu/cuda/stream.hpp
        include/cuda/emu/cuda/device.hpp
        include/cuda/emu/cuda/device/container.hpp
        include/cuda/emu/cuda/device/location_policy.hpp
        include/cuda/emu/cuda/device/mdcontainer.hpp
        include/cuda/emu/cuda/device/mdspan.hpp
        include/cuda/emu/cuda/device/span.hpp
        include/cuda/emu/cuda/allocator.hpp
        include/cuda/emu/cuda/error.hpp
        include/cuda/emu/cublas/error.hpp
        include/cuda/emu/cublas/type.hpp
        include/cuda/emu/cublas/handle.hpp
        include/cuda/emu/utility.cuh
        include/cuda/emu/cufft/error.hpp
        include/cuda/emu/cufft/type.hpp
        include/cuda/emu/cufft/utility.hpp
        include/cuda/emu/cufft/handle.hpp
        include/cuda/emu/cusolver/error.hpp
        include/cuda/emu/cusolver/handle.hpp
        include/cuda/emu/cufft.hpp
        include/cuda/emu/iterator/coordinate.cuh
        include/cuda/emu/iterator/function.cuh
        include/cuda/emu/thrust/execution_policy.cuh
        include/cuda/emu/cublas.hpp
        include/cuda/emu/cusolver.hpp
        include/cuda/emu/tensor.hpp
        include/cuda/emu/atomic.cuh
        include/cuda/emu/cuda.hpp
        include/cuda/emu/memory.cuh
        include/cuda/emu/test/device_test.hpp
    )

    target_compile_options(emucuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            --extended-lambda
            --expt-relaxed-constexpr
        >
    )
    # Only needed for CUDA source file but it's easier to just set it
    target_compile_definitions(emucuda PUBLIC FMT_USE_CONSTEXPR=1)

    # Force the link of emu_cuda_device_pointer symbol in src/cuda/cuda/pointer.cpp
    if(NOT BUILD_SHARED_LIBS)
        target_link_options(emucuda INTERFACE "-Wl,-u,emu_cuda_device_pointer")
    endif()

    find_package(CUDAToolkit REQUIRED)
    find_package(cuda-api-wrappers CONFIG REQUIRED)
    # find_package(matx REQUIRED)

    target_link_libraries(emucuda PUBLIC
        emucore
        CUDA::cublas
        CUDA::cusolver
        CUDA::cudart
        CUDA::cuda_driver
        cuda-api-wrappers::runtime-and-driver
        # matx::matx
    )

    add_library(emu::cuda ALIAS emucuda)

    install(TARGETS emucuda
        FILE_SET HEADERS
    )
endif()

add_library(emupython INTERFACE)

target_sources(emupython INTERFACE FILE_SET HEADERS BASE_DIRS include/python FILES
    include/python/emu/pybind11/numpy.hpp
    include/python/emu/pybind11/cast/container.hpp
    include/python/emu/pybind11/cast/cstring_view.hpp
    include/python/emu/pybind11/cast/detail/capsule.hpp
    include/python/emu/pybind11/cast/detail/layout_adaptor.hpp
    include/python/emu/pybind11/cast/detail/location_adaptor.hpp
    include/python/emu/pybind11/cast/detail/mdspan_caster.hpp
    include/python/emu/pybind11/cast/mdcontainer.hpp
    include/python/emu/pybind11/cast/mdspan.hpp
    include/python/emu/pybind11/cast/span.hpp
    include/python/emu/pybind11/cast/cuda/stream.hpp
    include/python/emu/pybind11/common.hpp
    include/python/emu/pybind11/dltensdor.hpp
    include/python/emu/pybind11/format.hpp
    include/python/emu/bind/map.hpp
    include/python/emu/cast/common.hpp
    include/python/emu/pybind11.hpp
)

# We do not link to pybind11 here, we let the consumer do it
target_link_libraries(emupython INTERFACE
    emucore
)

if(emu_build_cuda)
    target_link_libraries(emupython INTERFACE emucuda)
endif()

add_library(emu::python ALIAS emupython)

install(TARGETS emupython
    FILE_SET HEADERS
)

# Installation help
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "share/cmake/${PROJECT_NAME}")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT)

# install(TARGETS emucore
#     EXPORT ${PROJECT_NAME}-targets
#     INCLUDES DESTINATION "${CMAKE_INSTALL_DATADIR}")

install(EXPORT ${PROJECT_NAME}-targets
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
    NAMESPACE emu::
    FILE "${PROJECT_NAME}-targets.cmake")

install(FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}")

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
