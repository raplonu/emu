cmake_minimum_required(VERSION 3.25)

########################################################################
#
# Emu Project
#

project(emu VERSION 0.1.0 LANGUAGES CXX)

option(
    emu_build_python_test
    "Enable python test module. Default: OFF. Values: { ON, OFF }."
    OFF
)
option(
    emu_build_cuda
    "Enable cuda module. Default: OFF. Values: { ON, OFF }."
    OFF
)
option(
    emu_install_config_file_package
    "Install config file package. Default: OFF. Values: { ON, OFF }."
    ON
)

# other otions:
set(
    emu_boost_namespace
    "boost" CACHE STRING
    "Custom boost namespace name"
)

########################################################################
#
# Emu Core
#

add_library(emucore)
add_library(emu::core ALIAS emucore)

target_sources(emucore
    PRIVATE
        src/core/numeric_type.cpp
        src/core/error.cpp
        src/core/pointer.cpp
        src/core/type_name.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/associative_container.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/config.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/detail/basic_span.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/detail/mdspan_types.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/detail/basic_container.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/detail/basic_mdcontainer.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/detail/basic_mdspan.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/detail/dlpack_types.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/expected.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/functor.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/fwd.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/hash.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/host/location_policy.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/host/mdcontainer.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/host/mdspan.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/host/container.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/host/span.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/info.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/location_policy.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/macro.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/managed_object.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/math.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/mdcontainer.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/numeric_type.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/optional.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/storage.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/type_name.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/type_traits.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/ostream.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/scoped.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/container.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/span.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/mdalgo.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/mdspan.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/assert.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/capsule.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/concepts.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/cstring_view.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/debug.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/error.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/error/generation.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/pointer.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/string.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/core/emu/utility.hpp
)

set_target_properties(
    emucore
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON EXPORT_NAME core
)

target_compile_features(emucore PUBLIC cxx_std_20)

if(emu_build_cuda)
    target_compile_definitions(emucore PUBLIC EMU_CUDA)
endif()

target_compile_definitions(emucore PUBLIC EMU_BOOST_NAMESPACE=${emu_boost_namespace})

find_package(Boost CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(mdspan CONFIG REQUIRED)
find_package(half CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)
find_package(tl-optional CONFIG REQUIRED)
find_package(dlpack CONFIG REQUIRED)

target_link_libraries(emucore PUBLIC
    Boost::boost
    fmt::fmt
    Microsoft.GSL::GSL
    std::mdspan
    half::half
    tl::expected
    tl::optional
    dlpack::dlpack
)

########################################################################
#
# Emu CUDA
#

if (emu_build_cuda)
    enable_language(CUDA)

    add_library(emucuda)
    add_library(emu::cuda ALIAS emucuda)

    target_sources(emucuda
        PRIVATE
            src/cuda/cuda/error.cpp
            src/cuda/cuda/pointer.cpp
            # src/cuda/cufft.cpp
            # src/cuda/cufft/handle.cpp
            src/cuda/cusolver/handle.cpp
            src/cuda/cublas/handle.cpp
            src/cuda/cublas.cpp
            src/cuda/cusolver.cpp
        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/device/mdcontainer.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/device/mdspan.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/device/container.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/device/span.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/device/location_policy.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/allocator.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/device.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/error.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/event.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/memory.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/pointer.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/stream.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/managed/container.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/managed/location_policy.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/managed/mdcontainer.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/managed/mdspan.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/managed/span.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda/test/device_test.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cufft.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cufft/error.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cufft/type.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cufft/utility.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cufft/handle.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cusolver.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cusolver/error.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cusolver/handle.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cublas.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cublas/type.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cublas/error.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cublas/handle.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cuda/emu/cuda.hpp
    )

    set_target_properties(
        emucuda
        PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON EXPORT_NAME cuda
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

    # Remove it when the 3.1 version is released in CUDA toolkit
    find_package(nv-cccl REQUIRED CONFIG)

    target_link_libraries(emucuda PUBLIC
        emu::core
        CUDA::cublas
        CUDA::cusolver
        CUDA::cudart
        CCCL::CCCL # nv-cccl expose the CCCL::CCCL target.
    )

endif()

add_library(emupython INTERFACE)
add_library(emu::python ALIAS emupython)

target_sources(emupython INTERFACE FILE_SET HEADERS
    BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/bind/map.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/cstring_view.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/cuda/stream.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/detail/capsule.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/detail/layout_adaptor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/detail/location_adaptor.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/detail/mdspan_caster.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/mdcontainer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/mdspan.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/container.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/cast/span.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/format.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/python/emu/pybind11/numpy.hpp
)

set_target_properties(
    emupython
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON EXPORT_NAME python
)

# We do not link to pybind11 here, we let the consumer do it
target_link_libraries(emupython INTERFACE
    emu::core
)

if(emu_build_cuda)
    target_link_libraries(emupython INTERFACE emu::cuda)
endif()

# ########################################################################
# #
# # Install
include(GNUInstallDirs)

if(emu_build_cuda)
    set(INSTALL_TARGETS emucore emupython emucuda)
else()
    set(INSTALL_TARGETS emucore emupython)
endif()

install(
    TARGETS ${INSTALL_TARGETS}
    EXPORT emu-targets
    # COMPONENT emu
    DESTINATION ${CMAKE_INSTALL_LIBDIR}$<$<CONFIG:Debug>:/debug>
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}$<$<CONFIG:Debug>:/debug>
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(emu_install_config_file_package)
    include(CMakePackageConfigHelpers)

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/emu-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/emu-config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/emu"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/emu-config-version.cmake"
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/emu-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/emu-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/emu"
        COMPONENT emu
    )

    install(
        EXPORT emu-targets
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/emu"
        NAMESPACE emu::
        FILE emu-targets.cmake
        COMPONENT emu
        # EXPORT_PACKAGE_DEPENDENCIES # Use when it exists (see https://cmake.org/cmake/help/latest/command/export.html#exporting-targets-matching-install-export)
    )

endif()

########################################################################
#
# Test
#
include(CTest)
if (BUILD_TESTING)
    add_subdirectory(test)
endif()
