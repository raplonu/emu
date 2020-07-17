import os
import json
import subprocess
from glob import glob
from shutil import copy2
from pathlib import Path

from setuptools.command.build_ext import build_ext
from setuptools import setup, Extension, find_packages
from setuptools.command.develop import develop as develop_orig
from setuptools.command.install import install as install_orig

package_name = 'emu'
cxx_name = package_name
py_name = f'{package_name}wrap'

package_version = '0.1'

here = Path(__file__).parent.resolve()

# Get the long description from the README file
long_description = (here / 'README.md').read_text(encoding='utf-8')

extra_requires = []

try:
    out = subprocess.check_output(['conan', '--version'])
except OSError:
    extra_requires.append('conan')

def symlink(target, link_name):
    while True:
        try:
            os.symlink(target, link_name)
            break
        except FileExistsError:
            os.remove(link_name)

# Hack to let ConanBuild know if we are in install or develop mode
# Let develop_orig class modify it when editable mode is used
glob_editable = False

# Default conan channel. Usage: `--install-option=--channel=<conan channel>`
glob_conan_channel = 'user/stable'

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
            if not self.channel is None:
                glob_conan_channel = self.channel

            global glob_conan_args
            if not self.conan_args is None:
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
        call(f'conan editable add {source_dir} {ref}@{channel} -l {source_dir}/layout_gcc')
    else:
        # Cannot export package if package already exist and is editable. Try delete it everytime.
        call(f'conan editable remove {ref}@{channel}')
        call(f'conan export-pkg {source_dir} {channel} -f -bf {build_dir}')


class ConanBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)


    def build_extension(self, ext):
        editable   = glob_editable
        channel    = glob_conan_channel

        build_dir  = ext.sourcedir if editable else self.build_temp
        export_dir = Path(os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name))))
        build_mode = 'Debug' if self.debug else 'Release'

        # Package C++
        cxx_source_dir = ext.sourcedir
        cxx_build_dir  = f'{build_dir}/build'
        cxx_ref = f'{cxx_name}/{package_version}'
        cxx_args = f'-s build_type={build_mode} {glob_conan_args}'

        build_conan(cxx_source_dir, cxx_build_dir, cxx_ref, channel, cxx_args, editable)

        # Package wrap
        py_source_dir  = f'{ext.sourcedir}/python'
        py_build_dir   = f'{build_dir}/python/build'
        py_ref  = f'{py_name}/{package_version}'
        py_args  = f'{cxx_args} -o {py_name}:cxx_ref={cxx_ref}@{channel}'

        build_conan(py_source_dir, py_build_dir, py_ref, channel, py_args, editable)

        # Retrive C++ libraries from conan cache and export then into python package.
        call(f'conan install {py_ref}@{channel} -g json -if {py_build_dir} {py_args}')

        # Exporting wrapper and its dependencies libraries.

        data = json.load(open(f'{py_build_dir}/conanbuildinfo.json'))

        # TODO: check when cxx libs will depend on dynamic libraries.
        for dep in data['dependencies']:
            for lib_path in dep['lib_paths']:
                if os.listdir(lib_path):
                    for lib in map(Path, glob(f'{lib_path}/*.so')):
                        # If in editable mode, export library as symbolic link, otherwise perform a plain copy.
                        (symlink if editable else copy2)(lib, export_dir / lib.name)

setup(
    name=package_name,
    version='0.0.1',
    author='Julien Bernard',
    author_email='raplonu.jb@gmail.com',
    description='Python & C++ toolkit.',
    long_description=long_description,
    long_description_content_type='text/markdown',
    package_dir={'': 'python'},
    packages=find_packages(where='python'),
    ext_modules=[ConanExtension(py_name)],
    cmdclass=dict(install=install, develop=develop, build_ext=ConanBuild),
    install_requires=['funcy', 'boltons'] + extra_requires,
    zip_safe=False,
)
