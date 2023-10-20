from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout

class EmuPythonConan(ConanFile):
    name = 'emu-python'
    version = '1.0.0'

    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/cosmic/tools/emu'
    description = 'Set of utilities for C++, CUDA and python'

    exports_sources = 'CMakeLists.txt', 'include/*', 'src/*', 'test/*'

    settings = 'os', 'compiler', 'build_type', 'arch'

    def requirements(self):
        self.requires('emu/1.0.0', transitive_headers=True)
        self.requires('pybind11/2.10.4', transitive_headers=True)

        self.test_requires('gtest/1.13.0')

    def layout(self):
        cmake_layout(self)

    generators = 'CMakeDeps', 'CMakeToolchain'

    def build(self):
        cmake = CMake(self)

        cmake.configure()
        cmake.build()

        cmake.test()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

        self.cpp_info.set_property("cmake_target_aliases", ["emu::python"])
