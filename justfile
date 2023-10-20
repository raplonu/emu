default: install

# Install emu and emu-python
install *args:
    conan create . -b missing {{args}}
    conan create python -b missing {{args}}

# Install emu and emu-python in developer mode (editable)
dev *args:
    conan build . -b missing {{args}}
    conan editable add .
    conan build python -b missing {{args}}
    conan editable add python

# Simply configure and build project locally
configure *args:
    conan build . -b missing {{args}}

# Build project locally
build:
    cmake --build --preset conan-release

# Call make with arguments
make *args:
    make -C build/Release {{args}}

# Run tests
test:
    cmake --build --preset conan-release --target test

# Clean build directory
clean:
    @rm -rf                             \
        build/*                         \
        python/build/*                  \
        CMakeUserPresets.json           \
        python/CMakeUserPresets.json