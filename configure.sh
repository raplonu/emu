#!/bin/bash
LOCAL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Exit on first error.
set -e

# Adds cosmic conan repository if it is not already the case.
conan remote list | grep cosmic || conan remote add cosmic https://odp2.jfrog.io/artifactory/api/conan/cosmic

# Get dependencies and prepare build directory.
# By default always build missing binary packages.
conan install $LOCAL_DIR -if $LOCAL_DIR/build -b missing $@

# Let conan invoke cmake configure.
conan build $LOCAL_DIR -bf $LOCAL_DIR/build -c
