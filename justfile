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
