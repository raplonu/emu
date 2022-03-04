import os
import sys
import subprocess
from pathlib import Path

from setuptools.command.build_ext import build_ext
from setuptools import setup, Extension
from setuptools.command.develop import develop as develop_orig
from setuptools.command.install import install as install_orig

package_name = 'emu'
cxx_name = package_name
py_name = f'{package_name}wrap'

# here = Path(__file__).parent.resolve()

package_version = '0.2.0'

# Get the long description from the README file
# long_description = (here / 'README.md').read_text(encoding='utf-8')

# extra_requires = []

# Hack to let ConanBuild know if we are in install or develop mode
# Let develop_orig class modify it when editable mode is used
glob_editable = False

# Default conan channel. Usage: `--install-option=--channel=<conan channel>`
glob_conan_channel = 'cosmic/stable'

# Additional conan args. Usage: `--install-option="--conan-args=<conan channel>"`
glob_conan_args = ''


def add_commands(base):
    class HandleCommands(base):
        user_options = base.user_options + [
            ('channel=', None, 'conan channel'),
            ('conan-args=', None, 'conan args')]

        def initialize_options(self):
            base.initialize_options(self)
            self.channel = None
            self.conan_args = None

        def finalize_options(self):
            base.finalize_options(self)

        def run(self):
            global glob_conan_channel
            if self.channel is not None:
                glob_conan_channel = self.channel

            global glob_conan_args
            if self.conan_args is not None:
                glob_conan_args = self.conan_args

            base.run(self)

    return HandleCommands


install = add_commands(install_orig)
develop_orig = add_commands(develop_orig)


class develop(develop_orig):
    def run(self):
        global glob_editable
        glob_editable = True

        develop_orig.run(self)


class ConanExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


def call(command):
    subprocess.check_call(command.split())


def build_conan(source_dir, build_dir, ref, channel, args, editable):
    # Install C++ dependencies.
    call(f'conan install {source_dir} -if {build_dir} {args}')
    # Configure & Build C++ sources.
    call(f'conan build {source_dir} -bf {build_dir}')
    # Export C++ package into conan cache.
    if editable:
        call(f'conan editable add {source_dir} {ref}@{channel} -l {source_dir}/tools/layout_gcc')
    else:
        # Cannot export package if package already exist and is editable. Try delete it everytime.
        call(f'conan editable remove {ref}@{channel}')
        call(f'conan export-pkg {source_dir} {channel} -f -bf {build_dir}')


class ConanBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        p_version = sys.version_info
        version = f'{p_version.major}.{p_version.minor}'

        editable   = glob_editable
        channel    = glob_conan_channel

        build_dir  = ext.sourcedir if editable else self.build_temp
        export_dir = Path(os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name))))
        build_mode = 'Debug' if self.debug else 'Release'

        # Package C++
        cxx_source_dir = ext.sourcedir
        cxx_build_dir  = f'{build_dir}/build'
        cxx_ref = f'{cxx_name}/{package_version}'
        cxx_args = f'-b missing -s build_type={build_mode}\
                     -o {cxx_name}:python=True\
                     -o {cxx_name}:python_version={version}\
                     -o {cxx_name}:cuda=True\
                     {glob_conan_args}'

        build_conan(cxx_source_dir, cxx_build_dir, cxx_ref, channel, cxx_args, editable)

        # Package wrap
        py_source_dir  = f'{ext.sourcedir}/python'
        py_build_dir   = f'{build_dir}/python/build'

        call(f'conan install {cxx_ref}@{channel} -if {py_build_dir} -g CMakeToolchain -g cmake_find_package {cxx_args}')
        call(f'cmake {py_source_dir} -B {py_build_dir}\
               -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake\
               -DCMAKE_MODULE_PATH={py_build_dir}\
               -DCMAKE_INSTALL_PREFIX={export_dir}')
        call(f'cmake --build {py_build_dir}')
        call(f'cmake --install {py_build_dir}')


setup(
    ext_modules=[ConanExtension(py_name)],
    cmdclass=dict(install=install, develop=develop, build_ext=ConanBuild),
    include_package_data=True,
    test_suite='python.test',
)
