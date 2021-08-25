#!/bin/bash
LOCAL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Exit on first error.
set -e

# Get dependencies and prepare build directory.
conan install $LOCAL_DIR -if $LOCAL_DIR/build $@

# Let conan invoke cmake configure.
conan build $LOCAL_DIR -bf $LOCAL_DIR/build -c
