from conans import ConanFile

try:
    from cuda_arch import compute_capabilities
except :
    print('Error importing cuda_arch: cannot load cuda library.')

# def compute_capabilities():
#     import cuda_arch
#     return cuda_arch.compute_capabilities()

class Pkg(ConanFile):
    exports = 'cuda_arch.py'
