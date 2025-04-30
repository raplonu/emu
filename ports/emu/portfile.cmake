
set(SOURCE_PATH "/home/jbernard/workspace/next/emu")

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        cuda       build_cuda
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        -DBUILD_TESTING=OFF
)

vcpkg_cmake_install()
#
vcpkg_cmake_config_fixup(
    CONFIG_PATH "share/cmake/emu"
    DO_NOT_DELETE_PARENT_CONFIG_PATH
)

file(REMOVE_RECURSE
    "${CURRENT_PACKAGES_DIR}/debug/include"
    "${CURRENT_PACKAGES_DIR}/debug/share"
)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/share/cmake")

# file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/usage" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
vcpkg_install_copyright(FILE_LIST "/home/jbernard/workspace/next/emu/LICENSE")
