#!/bin/sh
LOCAL_DIR="$(realpath --relative-to=$(pwd) $( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd ))"

# Adds cosmic remote if missing.
conan remote list | grep cosmic || conan remote add cosmic https://odp2.jfrog.io/artifactory/api/conan/cosmic

# Adds default profile if missing.
conan profile list | grep default || conan profile new default --detect

# Set C++ abi to libstdc++11.
conan profile update settings.compiler.libcxx=libstdc++11 default

echo "Current state"

conan remote list
conan profile show default
