default: install

# Install emu and emu-python
install *args:
    just cpp-install {{args}}
    just python-install {{args}}

# Install emu
cpp-install *args:
    conan create . -b missing {{args}}

# Install emu-python
python-install *args:
    conan create python -b missing {{args}}

dev *args:
    just cpp-dev {{args}}
    just python-dev {{args}}

cpp-dev *args:
    conan editable add .
    conan build . -b missing {{args}}

python-dev *args:
    conan editable add python
    conan build python -b missing {{args}}

build build_type="release":
    just cpp-build {{build_type}}
    just python-build {{build_type}}

cpp-build build_type="release":
    cmake --build --preset "conan-{{build_type}}"

python-build build_type="release":
    @cd python # stupid cmake does not allow to specify the directory...
    cmake --build --preset "conan-{{build_type}}"

# Run tests
test build_type="release":
    just cpp-test {{build_type}}
    just python-test {{build_type}}

cpp-test build_type="release":
    ctest --preset conan-{{build_type}}

python-test build_type="release":
    @cd python # stupid cmake does not allow to specify the directory...
    ctest --preset conan-{{build_type}}

# Register emu and emu-python as editable in conan
@register:
    conan editable add .
    conan editable add python

# Unregister emu and emu-python as editable in conan
@unregister:
    conan editable remove .
    conan editable remove python

# Clean build directory
@clean:
    just unregister
    rm -rf                            \
        build/*                       \
        python/build/*                \
        CMakeUserPresets.json         \
        python/CMakeUserPresets.json
