#!/bin/sh
LOCAL_DIR="$(realpath --relative-to=$(pwd) $( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd ))"

# if ! command -v conan &> /dev/null
# then
#     echo "conan could not be found"
#     while true; do
#         read -p "Do you wish to install this program? [Yy/Nn]" yn
#         case $yn in
#             [Yy]* ) pip install conan --upgrade; break;;
#             [Nn]* ) exit;;
#             * ) echo "Please answer yes or no.";;
#         esac
#     done
# fi

conan remote list | grep cosmic || conan remote add cosmic https://odp2.jfrog.io/artifactory/api/conan/cosmic

conan profile list | grep default || conan profile new default --detect

conan profile update settings.compiler.libcxx=libstdc++11 default

echo "Current state"

conan remote list
conan profile show default
