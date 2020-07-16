# emu

emu is a library that gather a set of utilities for C++, CUDA and python.

# Install

To install emu both in your python & conan cache execute:

```bash
git clone git@gitlab.obspm.fr:jbernard/emu.git
pip install ./emu
```

# Develop mode

## Editable

emu can be installed in editable mode both for python and C++ using the following command:

```bash
pip install -e ./emu
```

This command will create editable pip & conan packages. That means these packages directly refer to what is in `python`, `build` and `include` folders.

That mean if changes occur in python source code. No additional operations are needed.

On the other side, after any change in your C++ source code, you need to recompile emu libraries:

```bash
cd emu/build
make
```

or

```bash
cd emu
conan build . -bf build
```

## Conan

By default, pip will install the emu C++ package in conan with `user/stable` channel.

You can edit this channel using the following command:

```bash
pip install ./emu --install-option=--channel=<custom channel>
```

Also, it is possible to pass argument to the conan install command using the following command:

```bash
pip install ./emu --install-option=--conan-args=="<conan args>"
```

