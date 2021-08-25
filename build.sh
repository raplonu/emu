#!/bin/sh
LOCAL_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Exit on first error.
set -e

conan build $LOCAL_DIR -bf $LOCAL_DIR/build -b
