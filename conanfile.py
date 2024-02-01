from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class EmuConan(ConanFile):
    name = 'emu'
    version = '1.0.0'

    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/cosmic/tools/emu'
    description = 'Set of utilities for C++, CUDA and python'

    exports_sources = 'CMakeLists.txt', 'include/*', 'src/*', 'test/*'

    settings = 'os', 'compiler', 'build_type', 'arch'

    options = {
        'shared'        : [True, False],
        'fPIC'          : [True, False],
        'cuda'          : [True, False],
    }

    default_options = {
        'shared'     : False,
        'fPIC'       : True,
        'cuda'       : False,
    }

    def requirements(self):
        self.requires('fmt/10.0.0', transitive_headers=True)
        self.requires('boost/1.82.0', transitive_headers=True)
        self.requires('ms-gsl/4.0.0', transitive_headers=True)
        self.requires('mdspan/0.6.0', transitive_headers=True)
        self.requires('tl-expected/1.1.0', transitive_headers=True)
        self.requires('tl-optional/1.1.0', transitive_headers=True)
        self.requires('half/2.2.0', transitive_headers=True)

        if self.options.cuda:
            self.requires('cuda-api-wrappers/0.6.3', transitive_headers=True)

            self.requires('matx/0.6.0', transitive_headers=True)

        self.test_requires('gtest/1.13.0')

    # Cannot be optional (link to the use of cuda or not).
    python_requires = 'conan_cuda/1.0.0'

    def layout(self):
        cmake_layout(self)
        self.cpp.source.components['core'].includedirs = ['include/core']
        self.cpp.build.components['core'].libdirs = ['.']

        if self.options.cuda:
            cuda_prop = self.python_requires['conan_cuda'].module.properties()

            self.cpp.source.components['cuda'].includedirs = ['include/cuda', cuda_prop.include]
            self.cpp.build.components['cuda'].libdirs = ['.', cuda_prop.library]
            self.cpp.build.components['cuda'].system_libs = ['cuda', 'cudart', 'cublas']

    generators = 'CMakeDeps'

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables['emu_build_cuda'] = self.options.cuda

        tc.generate()

    def build(self):
        cmake = CMake(self)

        cmake.configure()
        cmake.build()

        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.components['core'].libs = ['emucore']      # Name of the library for the 'LibA' component
        self.cpp_info.components['core'].requires = [
            'fmt::fmt',
            'boost::boost',
            'ms-gsl::_ms-gsl',
            'tl-expected::expected',
            'tl-optional::optional',
            'mdspan::mdspan',
            'half::half'
        ]

        if self.options.cuda:
            # Conan does not provide a cuda "package". conan_cuda allows to retrive
            # cuda lib dir and include dir and append it to emucuda
            cuda_prop = self.python_requires['conan_cuda'].module.properties()

            self.cpp_info.components['cuda'].libs = ['emucuda']
            self.cpp_info.components['cuda'].system_libs = ['cuda', 'cudart', 'cublas']
            self.cpp_info.components['cuda'].libdirs += [cuda_prop.library]
            self.cpp_info.components['cuda'].includedirs += [cuda_prop.include]
            self.cpp_info.components['cuda'].requires = [
                'core',
                'cuda-api-wrappers::cuda-api-wrappers',
                'matx::matx'
            ]
            self.cpp_info.components['cuda'].defines = ['EMU_CUDA', 'FMT_USE_CONSTEXPR=1']
            # not supported by conan.
            # self.cpp_info.components['cuda'].set_property('cudaflags', ['--extended-lambda', '--expt-relaxed-constexpr'])
