# emu <img align="right" width="10%" src="image/emu.png">

[![Coverage Status](https://coveralls.io/repos/github/raplonu/emu/badge.svg?branch=rc1.0.0)](https://coveralls.io/github/raplonu/emu?branch=rc1.0.0)

`emu` is a library that gather a set of utilities for C++, CUDA and python.

## Reqirement

`emu` requires a c++20 compiler and the following tools:

- [cmake](https://cmake.org/) >= 3.24
- [cuda](https://developer.nvidia.com/cuda-toolkit) >= 12.1 (the one with device cuda graph feature)
- [conan](https://conan.io/) >= 2.0.0 or [vcpkg](https://vcpkg.io/) (experimental)
- [just](https://just.systems/) optional

`emu` requires `cmake` and `conan` to install dependencies and build.
Also, we recommend to use `just` to simplify the build process.

> Note: `conan` requires some configuration to work properly. See our [guide](https://github.com/raplonu/cosmic-center-index) or simply run:

```bash
curl -sS https://raw.githubusercontent.com/raplonu/cosmic-center-index/refs/heads/main/install.sh | bash
```

## Build

For CMake regulars, you can use the following commands to build the project:

```bash
# install dependencies
conan install . -b missing -c tools.cmake.cmaketoolchain:generator="Ninja Multi-Config"
# configure the project
cmake --preset conan-default
# build the project
cmake --build --preset release
```

> Note: The `emu` CMake presets are designed to be use with the Ninja Multi-Config generator (though it's not required). To enable it, set the environment variable: `CMAKE_GENERATOR="Ninja Multi-Config"` and add `tools.cmake.cmaketoolchain:generator = Ninja Multi-Config` in conan.

> Note: `missing` is used to build dependencies from source if binary is not available. `editable` is used to build local dependencies register using `conan editable add` command.

conan offers an alternative way to build the project using `conan build` command. This command will configure and build the project in a single step.

```bash
conan build . -b missing
```

Or use `just dev`.

## Use editable mode

After building the project, you can use `conan editable add` command to add the project as an editable package. This will allow you to modify the source code and rebuild the project without having to reinstall it.

```bash
conan editable add .
```

For incremental build, just run:

```bash
cmake --build --preset release
```

or use `just build`.

## Debug mode

To use project in debug mode use `conan create|build|install [...] -s build_type=Debug` commands and `cmake --build --preset debug` commands.
