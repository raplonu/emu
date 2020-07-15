# emu

# Install

```bash
git clone git@gitlab.obspm.fr:jbernard/emu.git
pip install emu
```

# Develop mode

## conan

By default, pip will install the emu C++ package in conan with `user/stable` channel.

You can edit this channel using the following command:
```bash
pip install emu --install-option=--channel=<custom channel>
```

## editable

emu can be installed in editable mode both for python and C++ using the following command.
```bash
pip install -e emu
```

This command will create an editable pip & conan package. That means the package directly refer to what is in `python`, `build` and `include` folders.
After any change in your C++ source code, simply do:

```bash
cd emu/build
make
```

or

```bash
cd emu
conan build . -bf build
```

If changes occur in python source code. No additional operation are needed.