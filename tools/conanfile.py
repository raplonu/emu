from conans import ConanFile

from cuda_arch import compute_capabilities

class Pkg(ConanFile):
    exports = 'cuda_arch.py'