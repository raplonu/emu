from conans import ConanFile, CMake, tools
import os

class EmuwrapConan(ConanFile):
    name = 'emuwrap'
    version = '0.1'
    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/cosmic/tools/emu'
    description = 'C++/CUDA toolkit.'

    build_policy = 'missing'

    requires = ['pybind11/2.3.0@conan/stable']

    options = {'shared'         : [True, False],
               'fPIC'           : [True, False],
               'python_version' : 'ANY',
               'cxx_ref'        : 'ANY'}

    default_options = {'shared'         : True,
                       'fPIC'           : True,
                       'python_version' : '3.8',
                       'cxx_ref'        : f'emu/{version}@user/stable'}

    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'cmake'
    exports_sources = '*'

    def requirements(self):
        self.requires(str(self.options.cxx_ref))

    def _configure(self):
        cmake = CMake(self)

        cmake.definitions['PYBIND11_PYTHON_VERSION'] = self.options.python_version

        cmake.configure(source_folder='.')
        return cmake

    def build(self):
        cmake = self._configure()
        cmake.build()

    def package(self):
        self.copy("*.so", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ['emuwrap']
