# emu <img align="right" width="10%" src="image/emu.png">

`emu` is a library that gather a set of utilities for C++, CUDA and python.

## Requirement

`emu` requires a c++20 compiler and the following tools:

- [cmake](https://cmake.org/) >= 3.18 (or whatever)
- [cuda](https://developer.nvidia.com/cuda-toolkit) >= 12.1 (the one with device cuda graph feature)
- [conan](https://conan.io/) >= 2.0.0 with cosmis remote using `conan remote add cosmic https://odp2.jfrog.io/artifactory/api/conan/cosmic`
- [just](https://just.systems/) optional

`emu` requires `cmake` and by default `conan` to install dependencies and build.
Also, we recommend to use `just` to simplify the build process.

## Build

```bash
conan create . -b missing
# install python
conan create python -b missing
```

or use `just install`.

This will create `emu` packages in your local conan cache.

## Dev

```bash
conan build . -b missing
conan editable add .
# dev python
conan build python -b missing
conan editable add python
```

or use `just dev`.

This will configure and build the project in `build/Release` folder and adds `emu` as an editable package in your local conan cache.

For next build, just run:

```bash
cmake --build --preset conan-release
```

or use `just build`.
