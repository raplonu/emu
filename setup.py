import os
import re
import sys
import json
import platform
import subprocess
from glob import glob
from pathlib import Path
from distutils import log
from shutil import copyfile

# from distutils.core import setup, Command
from distutils.version import LooseVersion
from setuptools.command.build_ext import build_ext
from setuptools import setup, Extension, find_packages
from setuptools.command.develop import develop as develop_orig
from setuptools.command.install import install as install_orig

package_name = 'emu'
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

# Default conan channel. Can be override adding `--install-option=--channel=<conan channel>`
glob_conan_channel = 'user/stable'

# Additional conan args. Usage: `--install-option="--glob_conan_install_args=<conan channel>"`
glob_conan_install_args = ''

def add_channel_command(base):
    class ChannelCommand(base):
        user_options = base.user_options + [
            ('channel=', None, 'conan channel'),
            ('conan-install-args=', None, 'conan args')]

        def initialize_options(self):
            base.initialize_options(self)
            self.channel = None
            self.conan_install_args = None

        def finalize_options(self):
            base.finalize_options(self)

        def run(self):
            global glob_conan_channel
            if not self.channel is None:
                glob_conan_channel = self.channel

            global glob_conan_install_args
            if not self.conan_install_args is None:
                glob_conan_install_args = self.conan_install_args

            base.run(self)

    return ChannelCommand

install = add_channel_command(install_orig)

develop_orig = add_channel_command(develop_orig)

class develop(develop_orig):
    def run(self):
        global glob_editable
        glob_editable = True

        develop_orig.run(self)

class ConanExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class ConanBuild(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        source_dir       = ext.sourcedir
        export_dir       = Path(os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name))))

        build_dir        = source_dir if glob_editable else self.build_temp
        cxx_build_dir    = f'{build_dir}/build'
        python_build_dir = f'{build_dir}/python/build'

        build_mode       = 'Debug' if self.debug else 'Release'

        conan_ref = f'{package_name}/{package_version}@{glob_conan_channel}'

        conan_install_args = [
            '-s', f'build_type={build_mode}',
            '--build', 'missing',
            *glob_conan_install_args.split()
        ]

        cmake_args = [
            f'-DCMAKE_BUILD_TYPE={build_mode}',
            f'-DPYTHON_EXECUTABLE={sys.executable}',
            f'-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={export_dir}'
        ]

        #######
        # C++ #
        #######

        # Building C++.

        # Install C++ dependencies.
        subprocess.check_call(['conan', 'install', source_dir, '-if', cxx_build_dir] + conan_install_args)
        # Configure & Build C++ sources.
        subprocess.check_call(['conan', 'build', source_dir, '-bf', cxx_build_dir])
        # Export C++ package into conan cache.
        if glob_editable:
            subprocess.check_call(['conan', 'editable', 'add', source_dir, conan_ref, '-l', f'{source_dir}/layout_gcc'])
        else:
            # Cannot export package if package already exist and is editable. Try delete it everytime.
            subprocess.check_call(['conan', 'editable', 'remove', conan_ref])
            subprocess.check_call(['conan', 'export-pkg', source_dir, glob_conan_channel, '-bf', cxx_build_dir, '-f'])

        ############
        # C++ wrap #
        ############

        # Configuring conanfile.txt.

        # Could not find a way to install the conan package for the wrapper without using conan file.
        # The solution since we don't know conan package name is to configure conanfile.txt.in and set package ref name.
        os.makedirs(f'{build_dir}/python', exist_ok=True)

        with open(f'{source_dir}/python/conanfile.txt.in') as conanfile_in:
            with open(f'{build_dir}/python/conanfile.txt', 'w+') as conanfile_out:
                conanfile_out.writelines(map(lambda l: l.format(conan_ref=conan_ref), conanfile_in.readlines()))

        # Building emuwrap.

        # Retrive C++ libraries from conan cache and export then into python package.
        subprocess.check_call(['conan', 'install', f'{build_dir}/python', '-if', python_build_dir] + conan_install_args)
        subprocess.check_call(['cmake', f'{source_dir}/python'] + cmake_args, cwd=python_build_dir)
        subprocess.check_call(['cmake', '--build', '.'], cwd=python_build_dir)

        # Exporting emu's libraries.

        data = json.load(open(f'{python_build_dir}/conanbuildinfo.json'))

        # Export emu's and its dependencies libraries.
        # TODO: check when emu will depend on dynamic libraries.
        for dep in data['dependencies']:
            for lib_path in dep['lib_paths']:
                if os.listdir(lib_path):
                    for lib in map(Path, glob(f'{lib_path}/*.so')):
                        # If in editable mode, export library as symbolic link, otherwise perform a plain copy.
                        (symlink if glob_editable else copyfile)(lib, export_dir / lib.name)

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
    ext_modules=[ConanExtension('emuwrap')],
    cmdclass=dict(install=install, develop=develop, build_ext=ConanBuild),
    install_requires=['funcy', 'boltons'] + extra_requires,
    zip_safe=False,
)
