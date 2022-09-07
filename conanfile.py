from conans import ConanFile, tools
from conan.tools.cmake import CMake, CMakeToolchain
import os


def load(*filenames):
    for filename in filenames:
        if os.path.exists(filename):
            # Delete the return trail using [:-1].
            return [tools.load(filename)[:-1]]
    return []


class EmuConan(ConanFile):
    name = 'emu'
    version = '0.2.0'
    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/cosmic/tools/emu'
    description = 'Set of utilities for C++, CUDA and python'

    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'CMakeDeps'

    build_policy = 'missing'
    no_copy_source = True
    exports_sources = 'CMakeLists.txt', 'include/*', 'src/*', 'test/*', 'tools/*'

    requires = [
        'fmt/9.1.0',
        'boost/1.79.0',
        'ms-gsl/4.0.0',
        'mdspan/0.4.0@cosmic/stable',
        'tl-expected/1.0.0',
        'tl-optional/1.0.0'
    ]

    # Cannot be optional (link to the use of cuda or not).
    python_requires = 'cuda_arch/0.2.0@cosmic/stable'

    options = {
        'shared'        : [True, False],
        'fPIC'          : [True, False],
        'half'          : [True, False],
        # Build emu_cuda library.
        'cuda'          : [True, False],
        # Build emu_python library.
        'python'        : [True, False],
        # Specify the sm configuration used to build emu_cuda library.
        # This has no effect if cuda option is set to False.
        # cuda_sm can be set to
        # - 'Auto' detects local machine GPU compute arch at runtime.
        # NUM: Any number. Only those pairs are currently accepted by NVCC though:
        #    20 21 30 32 35 37 50 52 53 60 62
        # For more information refer to https://cmake.org/cmake/help/latest/variable/CMAKE_CUDA_ARCHITECTURES.html
        'cuda_sm'       : 'ANY',
        'python_version': 'ANY',
        'test'          : [True, False],
        'doc'           : [True, False]}

    default_options = {
        'shared'     : False,
        'fPIC'       : True,
        'half'       : False,
        'cuda'       : False,
        'python'     : False,
        'test'       : False,
        'doc'        : False,
        'cuda_sm'    : 'Auto'
    }

    def parse_cuda_compute_capabilities(self):
        cuda = self.python_requires['cuda_arch'].module

        if str(self.options.cuda_sm) == 'Auto':
            self.options.cuda_sm = cuda.compute_capabilities()

    def configure(self):
        if not self.options.cuda:
            self.options.remove('cuda_sm')
        else:
            self.parse_cuda_compute_capabilities()

        if not self.options.python:
            self.options.remove('python_version')

    def requirements(self):
        if self.options.cuda:
            self.requires('cuda-api-wrappers/0.4.3@cosmic/stable')

        if self.options.python:
            self.requires('pybind11/2.9.1@cosmic/stable')

        if self.options.half:
            self.requires('half/2.2.0')

        if self.options.test:
            self.requires('gtest/1.12.1', private=True)

    def generate(self):

        cmake = CMakeToolchain(self)

        cmake.variables['emu_build_cuda']   = self.options.cuda
        cmake.variables['emu_build_python'] = self.options.python
        cmake.variables['emu_half']         = self.options.half
        cmake.variables['emu_build_test']   = self.options.test
        # Ask the project to generate {target}_flags.txt with the C++ & CUDA flags in it if any.
        cmake.variables['emu_export_flags'] = True

        if self.options.cuda:
            cmake.variables['CMAKE_CUDA_ARCHITECTURES'] = self.options.cuda_sm

        if self.options.python:
            cmake.variables['PYBIND11_PYTHON_VERSION'] = self.options.python_version

        cmake.generate()

    def build(self):
        cmake = CMake(self)

        if self.should_configure:
            cmake.configure()
        if self.should_build:
            cmake.build()

        # We did not use `self.should_test` since it is enable by default.
        # But it can be still triggered independently using `-t` argument.
        if self.should_test and self.options.test:
            cmake.test()

    def package(self):
        self.copy('*.so', dst='lib', keep_path=False)
        self.copy('*.a' , dst='lib', keep_path=False)

        self.copy('*.h' , dst='include', src='include/core')

        if self.options.cuda:
            self.copy('*.h'  , dst='include', src='include/cuda')
            self.copy('*.cuh', dst='include', src='include/cuda')

        if self.options.python:
            self.copy('*.h'  , dst='include', src='include/python')

        # Each target export its public flags in {target}_flags.txt if there is any flags.
        # Flags are exported in package in order to be used by consumer.
        self.copy('*_flags.txt', dst='data', keep_path=False)

    def package_info(self):
        self.cpp_info.components['core'].libs = ['emucore']      # Name of the library for the 'LibA' component
        self.cpp_info.components['core'].requires = [
            'fmt::fmt',
            'boost::boost',
            'ms-gsl::_ms-gsl',
            'tl-expected::expected',
            'tl-optional::optional',
            'mdspan::mdspan'
        ]
        self.cpp_info.components['core'].cxxflags = load(f'{self.package_folder}/data/emucore_flags.txt', f'{self.package_folder}/build/emucore_flags.txt')
        self.cpp_info.components['core'].defines = [f'EMU_STRING_UTIL={1 if self.options.string_util else 0}']

        if self.options.cuda:
            self.cpp_info.components['cuda'].libs = ['emucuda']
            self.cpp_info.components['cuda'].requires = ['core', 'cuda-api-wrappers::cuda-api-wrappers']
            self.cpp_info.components['cuda'].cxxflags = load(f'{self.package_folder}/data/emucuda_flags.txt', f'{self.package_folder}/build/emucuda_flags.txt')
            self.cpp_info.components['cuda'].defines = ['EMU_CUDA']

        if self.options.python:
            # self.cpp_info.components['python'].libs = ['emupython']
            self.cpp_info.components['python'].requires = ['core', 'pybind11::headers']
            if self.options.cuda:
                self.cpp_info.components['python'].requires += ['cuda']
            self.cpp_info.components['python'].cxxflags = load(f'{self.package_folder}/data/emucuda_flags.txt', f'{self.package_folder}/build/emupython_flags.txt')
