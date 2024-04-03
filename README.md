# emu <img align="right" width="10%" src="image/emu.png">

`emu` is a library that gather a set of utilities for C++, CUDA and python.

## Requirement

`emu` requires a c++20 compiler and the following tools:

- [cmake](https://cmake.org/) >= 3.24
- [cuda](https://developer.nvidia.com/cuda-toolkit) >= 12.1 (the one with device cuda graph feature)
- [conan](https://conan.io/) >= 2.0.0
- [just](https://just.systems/) optional

`emu` requires `cmake` and by default `conan` to install dependencies and build.
Also, we recommend to use `just` to simplify the build process.

> Note: `conan` requires some configuration to work properly. See our [guide](https://gitlab.obspm.fr/jbernard/conan-packages) or simply run:

```bash
curl -sS https://gitlab.obspm.fr/jbernard/conan-packages/-/raw/main/install.sh | bash
```

## Build

```bash
# install emu
conan create . -b missing
# install emu-python
conan create python -b missing
```

or use `just install`.

This will create `emu` and `emu-python` packages in your local conan cache.

## Dev

```bash
conan editable add .
conan build . -b "missing"
# dev python
conan editable add python
conan build python -b "missing;editable"
```

or use `just dev`.

| Note: `missing` is used to build dependencies from source if binary is not available. `editable` is used to build local dependencies register using `conan editable add` command.

This will configure and build the project in `build/Release` folder by default and adds `emu` and `emu-python` as editable packages in your local conan cache. Other projects that rely on `emu` will compile and link against the lib and header in the current directory.

For incremental build, just run:

```bash
cmake --build --preset conan-release
# cmake doesn't allow to build a specific directory with a preset
(cd python; cmake --build --preset conan-release)
```

or use `just build`.

## Debug mode

To use project in debug mode add `-s build_type=Debug` to `conan create` or `conan build` commands and `--preset conan-debug` to `cmake --build` commands.
