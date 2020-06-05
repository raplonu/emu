from conans import ConanFile, CMake, tools
import os

def load(filename):
    # Delete the return trail using [0:-1].
    return [tools.load(filename)[0:-1]] if os.path.exists(filename) else []

class EmuConan(ConanFile):
    name = 'emu'
    version = '0.1'
    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/jbernard/emu'
    description = 'C++/CUDA toolkit.'

    requires = [
        'fmt/6.1.2',
        'boost/1.71.0@conan/stable',
        'half/2.1.0@user/stable']

    options = {'shared':  [True, False],
               'fPIC':    [True, False],
               'test':    [True, False],
               # Build emu_cuda library.
               'cuda':    [True, False],
               # Specify the sm configuration used to build emu_cuda library.
               # This has no effect if cuda option is set to False.
               # cuda_sm can be set to
               # - 'Auto' detects local machine GPU compute arch at runtime.
               # - 'Common' and 'All' cover common and entire subsets of architectures
               # ARCH_AND_PTX : NAME | NUM.NUM | NUM.NUM(NUM.NUM) | NUM.NUM+PTX
               # NAME: Fermi Kepler Maxwell Kepler+Tegra Kepler+Tesla Maxwell+Tegra Pascal
               # NUM: Any number. Only those pairs are currently accepted by NVCC though:
               #    2.0 2.1 3.0 3.2 3.5 3.7 5.0 5.2 5.3 6.0 6.2
               # For more information refer to https://cmake.org/cmake/help/latest/module/FindCUDA.html
               'cuda_sm': 'ANY'}

    default_options = {'shared' : True,
                       'fPIC'   : True,
                       'test'   : False,
                       'cuda'   : True,
                       'cuda_sm': 'Auto',
                       'boost:header_only': True}

    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = 'cmake'
    exports_sources = '*'

    def requirements(self):
        if self.options.test:
            self.requires('gtest/1.8.1@bincrafters/stable')

    def _configure(self):
        cmake = CMake(self)
        cmake.definitions['emu_use_cuda'] = self.options.cuda
        cmake.definitions['emu_cuda_sm'] = self.options.cuda_sm
        cmake.definitions['emu_build_test'] = self.options.test

        # The project will generate EmuCoreFlags.txt & EmuCudaFlags.txt with the flags in it.
        cmake.definitions['emu_export_flags'] = True

        cmake.configure(source_folder='.')
        return cmake

    def build(self):
        cmake = self._configure()
        cmake.build()
        if self.options.test:
            cmake.test()

    def package(self):
        self._configure().install()

    def package_info(self):
        self.cpp_info.libs = ['emucore']

        self.cpp_info.cxxflags = load(f'{self.package_folder}/lib/cmake/Emu/EmuCoreFlags.txt')

        if self.options.cuda:
            self.cpp_info.libs += ['emucuda']

            self.cpp_info.cxxflags += load(f'{self.package_folder}/lib/cmake/Emu/EmuCudaFlags.txt')