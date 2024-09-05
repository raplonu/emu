default: install

# Install emu
install *args:
    # unregister just in case or else consumer will continue to use the editable version
    just unregister

    conan create . -b missing {{args}}

dev *args:
    just register
    conan build . -b missing {{args}}
    @# copy compile_commands.json from either debug of release folder
    @-cp build/*/compile_commands.json build/

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
    rm -rf                            \
        build                         \
        CMakeUserPresets.json
