#!/bin/bash
# Project build directory relative to current directory.
BUILD_DIR="$(realpath --relative-to=$(pwd) $( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd ))/build"

# Exit on first error.
set -e

cd $BUILD_DIR
cmake --build . --parallel
