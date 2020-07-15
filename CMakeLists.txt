cmake_minimum_required(VERSION 3.11)

option(emu_build_cuda   "Build cuda module"     ON)
option(emu_verbose      "Print emu info"        ON)
option(emu_build_test   "Build unit test"       OFF)

message(STATUS "Emu - Configuring")

set(emu_language CXX)

if (emu_build_cuda)
    list(APPEND emu_language CUDA)
endif()

project(emu VERSION 0.1.0 LANGUAGES ${emu_language})

set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)


message(CHECK_START "Emu - Conan setup")

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

message(CHECK_PASS "Done")

########################################################################
#
# Emu Core.
#

add_library(emucore
    src/core/fun.cpp)

target_include_directories(emucore PUBLIC
    $<INSTALL_INTERFACE:include/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/core>
    ${CUDA_INCLUDE_DIRS}
)

target_compile_features(emucore PUBLIC cxx_std_14)

target_link_libraries(emucore PUBLIC
    CONAN_PKG::fmt
    CONAN_PKG::boost
    CONAN_PKG::half
    CONAN_PKG::ms-gsl
    # CONAN_PKG::abseil
    CONAN_PKG::tl-expected
    CONAN_PKG::tl-optional
)

add_library(emu::core ALIAS emucore)

########################################################################
#
# Emu CUDA.
#

if (emu_build_cuda)
    message(CHECK_START "Emu - Configuring CUDA support")

    find_package(CUDA REQUIRED)

    if (NOT emu_cuda_sm)
        set(emu_cuda_sm "Auto")
    endif()

    cuda_select_nvcc_arch_flags(ARCH_FLAGS ${emu_cuda_sm})
    if (emu_verbose)
        message(STATUS "Emu - cuda sm : ${ARCH_FLAGS_readable}")
    endif()

    # target_compile_options remove repeated arguments.
    # Since `-gencode` appears multiple times,
    # we replace it by `--generate-code=` and merge it
    # will the next argument since there are related.
    # This way it became unique and avoids difficulties using spaces.
    string(REPLACE "-gencode;" "--generate-code=" ARCH_FLAGS "${ARCH_FLAGS}")

    add_library(emucuda
        src/cuda/funu.cu
    )

    target_include_directories(emucuda PUBLIC
        $<INSTALL_INTERFACE:include/cuda>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cuda>
    )

    target_compile_features(emucuda PUBLIC cxx_std_14)

    target_link_libraries(emucuda PUBLIC
        emucore
        ${CUDA_LIBRARIES}
        CONAN_PKG::cuda-api-wrappers
    )

    target_compile_options(emucuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            ${ARCH_FLAGS}
            --expt-extended-lambda
            --expt-relaxed-constexpr
        >)

    add_library(emu::cuda ALIAS emucuda)
    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Google Test.
#
# The tests are not built by default.  To build them, set the
# emu_build_test option to ON.  You can do it by running ccmake
# or specifying the -Demu_build_test=ON flag when running cmake.
if (emu_build_test)
    # This must be set in the root directory for the tests to be run by
    # 'make test' or ctest.
    message(CHECK_START "Emu - Configuring test")
    enable_testing()
    add_subdirectory(test)
    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Install.
#

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Emu)

set(emu_targets emucore)

if (emu_build_cuda)
    list(APPEND emu_targets emucuda)
endif()

install(TARGETS ${emu_targets}
    EXPORT emu-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

set_target_properties(emucore PROPERTIES EXPORT_NAME Core)
install(DIRECTORY include/core/emu DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (emu_build_cuda)
    set_target_properties(emucuda PROPERTIES EXPORT_NAME Cuda)
    install(DIRECTORY include/cuda/emu DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

install(EXPORT emu-targets
    FILE
      EmuTargets.cmake
    NAMESPACE
      Emu::
    DESTINATION
        ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/EmuConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/EmuConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/EmuConfig.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.txt
    ${CMAKE_CURRENT_BINARY_DIR}/conaninfo.txt
    ${CMAKE_CURRENT_BINARY_DIR}/EmuConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/EmuConfigVersion.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

# Exporting COMPILE_OPTIONS if exists for emucore and emucuda
if(emu_export_flags)
    if (emu_verbose)
        message(STATUS "Exporting compile flags")
    endif()

    get_target_property(TMP_CXX_FLAGS emucore COMPILE_OPTIONS)

    if(TMP_CXX_FLAGS)
        if (emu_verbose)
            message(STATUS "Exporting Emu::Core compile flags in EmuCoreFlags.txt")
        endif()

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Flags.txt.in
            ${CMAKE_CURRENT_BINARY_DIR}/EmuCoreFlags.txt @ONLY)

        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/EmuCoreFlags.txt
            DESTINATION ${INSTALL_CONFIGDIR}
        )
    endif()

    if (emu_build_cuda)
        get_target_property(TMP_CXX_FLAGS emucuda COMPILE_OPTIONS)

        if(TMP_CXX_FLAGS)
            if (emu_verbose)
                message(STATUS "Exporting Emu::Cuda compile flags in EmuCudaFlags.txt")
            endif()
            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Flags.txt.in
                ${CMAKE_CURRENT_BINARY_DIR}/EmuCudaFlags.txt @ONLY
            )

            install(FILES
                ${CMAKE_CURRENT_BINARY_DIR}/EmuCudaFlags.txt
                DESTINATION ${INSTALL_CONFIGDIR}
            )
        endif()
    endif()
endif()

export(EXPORT emu-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/EmuTargets.cmake
    NAMESPACE Emu::)

#Register package in user's package registry
export(PACKAGE Emu)

