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

coverage:
    mkdir -p build/coverage
    gcovr -r . --html --html-details -o build/coverage/index.html

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

# Validate CI/CD configuration
validate-ci:
    ./scripts/validate-ci.sh

# Run code quality checks locally (requires clang-format and clang-tidy)
quality-check:
    @echo "Running code formatting check..."
    find src include -name "*.cpp" -o -name "*.hpp" -o -name "*.cu" -o -name "*.cuh" | \
    xargs clang-format --dry-run --Werror --style=file || echo "❌ Format check failed"
    @echo "Running static analysis..."
    find src include -name "*.cpp" -o -name "*.hpp" | \
    xargs clang-tidy -p=build/Debug --config-file=.clang-tidy || echo "❌ Static analysis found issues"
