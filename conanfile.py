from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class EmuConan(ConanFile):
    name = 'emu'
    version = '1.0.0'

    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/cosmic/tools/emu'
    description = 'Set of utilities for C++, CUDA and python'

    settings = 'os', 'compiler', 'build_type', 'arch'

    exports_sources = 'CMakeLists.txt', 'include/*', 'src/*', 'test/*'

    # Generate the logic between shared and fPic
    implements = ['auto_shared_fpic']

    options = {
        'cuda'          : [True, False], # Build the emu cuda extension
        'python'        : [True, False], # Build the emu python tests, change nothing regarding the emu python extension
        'shared'        : [True, False],
        'fPIC'          : [True, False],
    }

    default_options = {
        'cuda'       : False,
        'python'     : False,
        'shared'     : False,
        'fPIC'       : True,
    }

    def requirements(self):
        self.requires('fmt/11.0.0', transitive_headers=True, transitive_libs=True)
        self.requires('boost/1.84.0', transitive_headers=True, transitive_libs=True)
        self.requires('ms-gsl/4.0.0', transitive_headers=True)
        self.requires('mdspan/0.6.0', transitive_headers=True)
        self.requires('tl-expected/1.1.0', transitive_headers=True)
        self.requires('tl-optional/1.1.0', transitive_headers=True)
        self.requires('half/2.2.0', transitive_headers=True)
        self.requires('dlpack/1.0', transitive_headers=True)
        self.requires('range-v3/0.12.0', transitive_headers=True)


        if self.options.cuda:
            self.requires('cuda-api-wrappers/0.7.0', transitive_headers=True)
            # self.requires('matx/0.8.0', transitive_headers=True)

        self.test_requires('gtest/1.13.0')

        if self.options.python:
            self.test_requires('pybind11/2.13.1')

    # Cannot be optional (link to the use of cuda or not).
    python_requires = 'conan_cuda/[>=1 <2]'

    def layout(self):
        cmake_layout(self)
        self.cpp.source.components['core'].includedirs = ['include/core']
        self.cpp.build.components['core'].libdirs = self.cpp.build.libdirs

        self.cpp.source.components['python'].includedirs = ['include/python']

        if self.options.cuda:
            cuda_prop = self.python_requires['conan_cuda'].module.properties()

            self.cpp.source.components['cuda'].includedirs = ['include/cuda', cuda_prop.include]
            self.cpp.build.components['cuda'].libdirs = [*self.cpp.build.libdirs, cuda_prop.library]
            self.cpp.build.components['cuda'].system_libs = ['cuda', 'cudart', 'cublas']


    generators = 'CMakeDeps'

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables['emu_build_cuda'] = self.options.cuda
        tc.variables['emu_build_python_test'] = self.options.python
        tc.variables['emu_boost_namespace'] = self.dependencies['boost'].options.namespace

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
        self.cpp_info.components['core'].libs = ['emucore']
        self.cpp_info.components['core'].requires = [
            'fmt::fmt',
            'boost::boost',
            'ms-gsl::_ms-gsl',
            'tl-expected::expected',
            'tl-optional::optional',
            'mdspan::mdspan',
            'half::half',
            'dlpack::dlpack',
            'range-v3::range-v3'

        ]

        self.cpp_info.components['core'].defines = ['EMU_BOOST_NAMESPACE={}'.format(self.dependencies['boost'].options.namespace)]

        self.cpp_info.components['python'].bindirs = []
        self.cpp_info.components['python'].libdirs = []
        self.cpp_info.components['python'].requires = ['core'] # , 'pybind11::pybind11'

        if self.options.cuda:
            # Conan does not provide a cuda 'package'. conan_cuda allows to retrieve
            # cuda lib dir and include dir and append it to emucuda
            cuda_prop = self.python_requires['conan_cuda'].module.properties()

            self.cpp_info.components['cuda'].libs = ['emucuda']
            self.cpp_info.components['cuda'].requires = [
                'core',
                'cuda-api-wrappers::cuda-api-wrappers',
                # 'matx::matx'
            ]
            #TODO: check if FMT_USE_CONSTEXPR is still needed to use {fmt} in .cu files
            self.cpp_info.components['cuda'].defines = ['EMU_CUDA', 'FMT_USE_CONSTEXPR=1']

            if not self.options.shared:
                # linker by default will not keep emu_cuda_device_pointer because it is not used explicitly.
                self.cpp_info.components['cuda'].exelinkflags = ['-Wl,-u,emu_cuda_device_pointer']

            self.python_requires['conan_cuda'].module.append_cuda(self.cpp_info.components['cuda'], ['cuda', 'cudart', 'cublas'])
