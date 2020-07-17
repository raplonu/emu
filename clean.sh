#!/bin/bash
LOCAL_DIR="$(realpath --relative-to=$(pwd) $( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd ))"

rm -rf                                  \
    $LOCAL_DIR/build                    \
    $LOCAL_DIR/python/build             \
    $LOCAL_DIR/python/*.so              \
    $LOCAL_DIR/python/*.egg-info        \
    $LOCAL_DIR/python/*/__pycache__