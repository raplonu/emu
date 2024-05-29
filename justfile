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

enable-wall build_type="default":
    cmake --preset "conan-{{build_type}}" -DCMAKE_CXX_FLAGS="-Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-unused-but-set-variable"

disable-wall build_type="default":
    cmake --preset "conan-{{build_type}}" -DCMAKE_CXX_FLAGS=""

enable-tidy build_type="default":
    #!/usr/bin/env bash
    set -euxo pipefail
    ct_path=$(which clang-tidy)
    cmake --preset "conan-{{build_type}}" -DCMAKE_CXX_CLANG_TIDY="$ct_path;--warnings-as-errors"

disable-tidy build_type="default":
    cmake --preset "conan-{{build_type}}" -DCMAKE_CXX_CLANG_TIDY=""

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
