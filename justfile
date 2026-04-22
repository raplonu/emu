default: install

# Install emu
install *args:
    # unregister to prevent usage of a forgotten editable package
    just unregister
    conan create . -b missing {{args}}

dev *args:
    just register
    conan build . -b missing -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" {{args}}

configure *args:
    conan install . -b missing -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config" {{args}}
    cmake --preset conan-default

build build_type="release":
    cmake --build --preset "conan-{{build_type}}"

verify_header build_type="release":
    cmake --preset "conan-{{build_type}}" --fresh -DCMAKE_VERIFY_INTERFACE_HEADER_SETS=ON
    cmake --build --preset "conan-{{build_type}}" --target all_verify_header_sets

fix_include build_type="release":
    cmake --preset "conan-{{build_type}}" --fresh -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="include-what-you-use"
    cmake --build build > iwyu.out 2>/dev/null
    fix_includes.py < iwyu.out

test build_type="release":
    ctest --preset conan-{{build_type}}

# Register emu as editable in conan
@register:
    conan editable add .

# Unregister emu as editable in conan
@unregister:
    conan editable remove .

# Clean build directory
@clean:
    just unregister
    rm -rf                                  \
        build                               \
        CMakeUserPresets.json               \
        test_package/build                  \
        test_package/CMakeUserPresets.json
