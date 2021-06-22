from conans import ConanFile, CMake, tools

def try_get(obj, attr, or_):
    try:
        return getattr(obj, attr)
    except:
        return or_

class EmuwrapConan(ConanFile):
    name = 'emuwrap'
    version = '0.1'
    license = 'MIT'
    author = 'Julien Bernard jbernard@obspm.fr'
    url = 'https://gitlab.obspm.fr/cosmic/tools/emu'
    description = 'C++/CUDA toolkit.'

    build_policy = 'missing'

    requires = ['pybind11/2.6.2']

    options = {'python_version': 'ANY',
               'cxx_ref'       : 'ANY'}

    default_options = {
        'python_version': '3.8',
        'cxx_ref'       : 'default'}

    settings = 'os', 'compiler', 'build_type', 'arch'
    generators = ['cmake', 'cmake_find_package']
    exports_sources = 'src/*'

    def requirements(self):
        if str(self.options.cxx_ref) != 'default':
            self.requires(str(self.options.cxx_ref))
        else:
            self.requires(f'emu/{self.version}@{try_get(self, "user", "cosmic")}/{try_get(self, "channel", "stable")}')

    def _configure(self):
        cmake = CMake(self)

        cmake.definitions['emuwrap_version']         = self.version
        cmake.definitions['PYBIND11_PYTHON_VERSION'] = self.options.python_version

        cmake.configure(source_folder='.')
        return cmake

    def build(self):
        # pybind11 adds `-Os` flag that nvcc did not know. Fix : removes pybind11::opt_size
        # Temporary fix until https://github.com/conan-io/conan-center-index/issues/5334 is resolved
        tools.replace_in_file(f'{self.build_folder}/Findpybind11.cmake',
            "set(pybind11_COMPONENTS pybind11::headers pybind11::embed pybind11::module pybind11::python_link_helper pybind11::windows_extras pybind11::lto pybind11::thin_lto pybind11::opt_size pybind11::python2_no_register pybind11::pybind11)",
            "set(pybind11_COMPONENTS pybind11::headers pybind11::embed pybind11::module pybind11::python_link_helper pybind11::windows_extras pybind11::lto pybind11::thin_lto pybind11::python2_no_register pybind11::pybind11)")

        cmake = self._configure()
        cmake.build()

    def package(self):
        self.copy("*.h" , dst="include", src="include")

    def package_id(self):
        self.info.header_only()
