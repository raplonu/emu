# emu <img align="right" width="10%" src="image/emu.png">

`emu` is a library that gather a set of utilities for C++, CUDA and python.

## Requirement

`emu` requires `cmake` and by default `conan` to install dependencies and build.

## Quickstart

- Clone the repository

```bash
git clone git@gitlab.obspm.fr:cosmic/tools/emu.git
cd emu
```

- Install dependencies and build the project

```bash
./configure.sh
./build.sh
```

Arguments passed to `configure.sh` will be forwarded to `conan install` command.

You can also directly package the project locally

```bash
pip install .
```

It will install `emu` both in your python & conan cache.

## Developer mode

### Editable

emu can be installed in editable mode both for python and C++ using the following command:

```bash
pip install -e .
```

This command will create editable pip & conan packages. That means these packages directly refer to what is in `python`, `build` and `include` folders.

That mean if changes occur in python source code. No additional operations are needed.

On the other side, after any change in your C++ source code, you need to recompile emu libraries using e.g. `build.sh`.

### Conan

By default, pip will install the emu C++ package in conan with `cosmic/stable` channel.

You can edit this channel using the following command:

```bash
pip install ./emu --install-option=--channel=<custom channel>
```

Also, it is possible to pass argument to the conan install command using the following command:

```bash
pip install ./emu --install-option=--conan-args="<conan args>"
```
