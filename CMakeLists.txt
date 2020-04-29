cmake_minimum_required(VERSION 3.11)

option(emu_use_cuda   "Use cuda module" OFF)
option(emu_verbose    "Print emu info"  ON)
option(emu_build_test "Build unit test" OFF)

if (emu_verbose)
    message(STATUS "Emu - Configuring.")
endif()

if (emu_use_cuda)
    set(emu_language CUDA CXX)
else()
    set(emu_language CXX)
endif()

project(emu VERSION 0.1.0 LANGUAGES ${emu_language})

if (emu_verbose)
    message(STATUS "Emu - Conan setup.")
endif()

if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo_multi.cmake)
else()
    include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
endif()

conan_basic_setup(TARGETS)

########################################################################
#
# Emu Core.
#

add_library(emucore SHARED
    src/fun.cpp)

target_include_directories(emucore PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(emucore PUBLIC cxx_std_14)

target_link_libraries(emucore PUBLIC ${CONAN_TARGETS})

add_library(emu::core ALIAS emucore)

########################################################################
#
# Emu CUDA.
#

if (emu_use_cuda)
    if (emu_verbose)
        message(STATUS "Emu - CUDA support enabled.")
    endif()
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

    add_library(emucuda SHARED
        srcu/funu.cu
    )

    target_include_directories(emucuda PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

    target_compile_features(emucuda PUBLIC cxx_std_14)

    target_link_libraries(emucuda PUBLIC emucore ${CUDA_LIBRARIES} ${CONAN_TARGETS})

    target_compile_options(emucuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            ${ARCH_FLAGS}
            --expt-extended-lambda
            --expt-relaxed-constexpr
        >)

    add_library(emu::cuda ALIAS emucuda)
endif()

########################################################################
#
# Install.
#

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Emu)

if (emu_use_cuda)
    set(emu_targets emucore emucuda)
else()
    set(emu_targets emucore)
endif()

install(TARGETS ${emu_targets}
    EXPORT emu-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

set_target_properties(emucore PROPERTIES EXPORT_NAME Core)

if (emu_use_cuda)
    set_target_properties(emucuda PROPERTIES EXPORT_NAME Cuda)
endif()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})  

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
        message(STATUS "Exporting compile flags.")
    endif()

    get_target_property(TMP_CXX_FLAGS emucore COMPILE_OPTIONS)
    
    if(TMP_CXX_FLAGS)
        if (emu_verbose)
            message(STATUS "Exporting Emu::Core compile flags in EmuCoreFlags.txt.")
        endif()

        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Flags.txt.in
            ${CMAKE_CURRENT_BINARY_DIR}/EmuCoreFlags.txt @ONLY)

        install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/EmuCoreFlags.txt
            DESTINATION ${INSTALL_CONFIGDIR}
        )
    endif()

    if (emu_use_cuda)
        get_target_property(TMP_CXX_FLAGS emucuda COMPILE_OPTIONS)

        if(TMP_CXX_FLAGS)
            if (emu_verbose)
                message(STATUS "Exporting Emu::Cuda compile flags in EmuCudaFlags.txt.")
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

########################################################################
#
# Google Test.
#
# The tests are not built by default.  To build them, set the
# emu_build_test option to ON.  You can do it by running ccmake
# or specifying the -Demu_build_test=ON flag when running cmake.
if (emu_build_test)
    if (emu_verbose)
        message(STATUS "Emu - Building test.")
    endif()
    # This must be set in the root directory for the tests to be run by
    # 'make test' or ctest.
    enable_testing()
    add_subdirectory(test)
endif()
