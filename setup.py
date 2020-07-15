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

glob_conan_install_args = ''

def add_channel_command(base):
    class ChannelCommand(base):
        user_options = base.user_options + [
            ('channel=', None, 'conan channel'),
            ('conan_install_args=', None, 'conan args')]


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
        conan_install_args = [
            '-o', 'emu:cuda=True',
            '-o', 'emu:python=True',
            '-o', f'emu:python_version={sys.version_info.major}.{sys.version_info.minor}',
            '-s', f'build_type={"Debug" if self.debug else "Release"}',
            '--build', 'missing',
            *glob_conan_install_args.split()
        ]

        sourcedir   = ext.sourcedir
        build_dir  = f'{sourcedir}/build' if glob_editable else self.build_temp

        # Install C++ dependencies.
        subprocess.check_call(['conan', 'install', sourcedir, '-if', build_dir] + conan_install_args)
        # Build C++ source.
        subprocess.check_call(['conan', 'build', sourcedir, '-bf', build_dir])
        # Export C++ package into conan cache.
        if glob_editable:
            subprocess.check_call(['conan', 'editable', 'add', sourcedir, f'{package_name}/{package_version}@{glob_conan_channel}', '-l', f'{sourcedir}/layout_gcc'])
        else:
            # Cannot export package if package already exist and is editable. Try delete it everytime.
            subprocess.check_call(['conan', 'editable', 'remove', f'{package_name}/{package_version}@{glob_conan_channel}'])
            subprocess.check_call(['conan', 'export-pkg', sourcedir, glob_conan_channel, '-bf', build_dir, '-f'])

        # Retrive C++ libraries from conan cache and export then into python package.
        subprocess.check_call(['conan', 'install', f'{package_name}/{package_version}@{glob_conan_channel}', '-g', 'json', '-if', build_dir] + conan_install_args)
        data = json.load(open(f'{build_dir}/conanbuildinfo.json'))

        extdir = Path(os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name))))

        # Export emu's and its dependencies libraries.
        # TODO: check when emu will depend on dynamic libraries.
        for dep in data['dependencies']:
            for lib_path in dep['lib_paths']:
                if os.listdir(lib_path):
                    for lib in map(Path, glob(f'{lib_path}/*.so')):
                        # If in editable mode, export library as symbolic link, otherwise perform a plain copy.
                        (symlink if glob_editable else copyfile)(lib, extdir / lib.name)

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
