default: install

# Install emu
install *args:
    # unregister or else it will just compile the project
    just unregister
    conan create . -b missing {{args}}

dev *args:
    just register
    conan build . -b missing {{args}}

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
        build/*                       \
        CMakeUserPresets.json
