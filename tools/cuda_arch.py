#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
The cuda arch documentation

Usage:
    cuda_arch.py device_count
    cuda_arch.py compute_capabilities
    cuda_arch.py -i <device> <property>


Options:
    -h --help               Show this help message and exit

Based on https://gist.github.com/f0k/63a664160d016a491b2cbea15913d549 from Jan Schlüter
Author: Julien Bernard
"""

import sys
import ctypes

# Some constants taken from cuda.h
CUDA_SUCCESS = 0
CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT = 16
CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR = 39
CU_DEVICE_ATTRIBUTE_CLOCK_RATE = 13
CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE = 36


def ConvertSMVer2Cores(cc):
    # Returns the number of CUDA cores per multiprocessor for a given
    # Compute Capability version. There is no way to retrieve that via
    # the API, so it needs to be hard-coded.
    return {
        # Tesla
        '10':   8,      # SM 1.0
        '11':   8,      # SM 1.1
        '12':   8,      # SM 1.2
        '13':   8,      # SM 1.3
        # Fermi
        '20':  32,      # SM 2.0: GF100 class
        '21':  48,      # SM 2.1: GF10x class
        # Kepler
        '30': 192,      # SM 3.0: GK10x class
        '32': 192,      # SM 3.2: GK10x class
        '35': 192,      # SM 3.5: GK11x class
        '37': 192,      # SM 3.7: GK21x class
        # Maxwell
        '50': 128,      # SM 5.0: GM10x class
        '52': 128,      # SM 5.2: GM20x class
        '53': 128,      # SM 5.3: GM20x class
        # Pascal
        '60':  64,      # SM 6.0: GP100 class
        '61': 128,      # SM 6.1: GP10x class
        '62': 128,      # SM 6.2: GP10x class
        # Volta
        '70':  64,      # SM 7.0: GV100 class
        '72':  64,      # SM 7.2: GV11b class
        # Turing
        '75':  64,      # SM 7.5: TU10x class
        # Ampere
        '80':  64,       # SM 8.0: GA100 class
        '86':  64,       # SM 8.6: GA10X class
        '87':  64,       # SM 8.7: GA10B class
        # Hopper
        '90':  64,       # SM 9.0: GHX0X class
    }.get(cc, 64)   # unknown architecture, return a default value


def check_error(cuda, result):
    if result != CUDA_SUCCESS:
        error_str = ctypes.c_char_p()
        cuda.cuGetErrorString(result, ctypes.byref(error_str))
        print(f'Error {result}: {error_str.value.decode()}')
        sys.exit(1)


def init_cuda_lib():
    libnames = ('libcuda.so', 'libcuda.dylib', 'cuda.dll')
    for libname in libnames:
        try:
            cuda_lib = ctypes.CDLL(libname)
        except OSError:
            continue
        else:
            break
    else:
        raise OSError("could not load any of: " + ' '.join(libnames))

    class CudaCheck:
        def __init__(self, cuda):
            self.cuda = cuda

        def __getattr__(self, item):
            def proxy(*args, **kwargs):
                check_error(self.cuda, getattr(self.cuda, item)(*args, **kwargs))
            return proxy

    cuda = CudaCheck(cuda_lib)

    cuda.cuInit(0)

    return cuda


_cuda = None

def cuda():
    global _cuda
    if _cuda is None:
        _cuda = init_cuda_lib()
    return _cuda

class Device:
    def __init__(self, id):
        self.id = id
        self.device = ctypes.c_int()
        # result = cuda.cuDeviceGet(ctypes.byref(self.device), self.id)

    @property
    def name(self):
        name = b' ' * 100
        cuda().cuDeviceGetName(ctypes.c_char_p(name), len(name), self.device)
        return name.split(b'\0', 1)[0].decode()

    @property
    def compute_capability(self):
        cc_major = ctypes.c_int()
        cc_minor = ctypes.c_int()
        cuda().cuDeviceComputeCapability(ctypes.byref(cc_major), ctypes.byref(cc_minor), self.device)
        return f'{cc_major.value}{cc_minor.value}'

    @property
    def multiprocessor_count(self):
        cores = ctypes.c_int()
        cuda().cuDeviceGetAttribute(ctypes.byref(cores), CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, self.device)
        return cores.value

    @property
    def cuda_cores(self):
        return self.multiprocessor_count * ConvertSMVer2Cores(*self.compute_capability)

    @property
    def max_threads_per_multiprocessor(self):
        threads_per_core = ctypes.c_int()
        cuda().cuDeviceGetAttribute(ctypes.byref(threads_per_core), CU_DEVICE_ATTRIBUTE_MAX_THREADS_PER_MULTIPROCESSOR, self.device)
        return threads_per_core.value

    @property
    def clock_rate(self):
        clockrate = ctypes.c_int()
        cuda().cuDeviceGetAttribute(ctypes.byref(clockrate), CU_DEVICE_ATTRIBUTE_CLOCK_RATE, self.device)
        return clockrate.value

    @property
    def memory_clock_rate(self):
        clockrate = ctypes.c_int()
        cuda().cuDeviceGetAttribute(ctypes.byref(clockrate), CU_DEVICE_ATTRIBUTE_MEMORY_CLOCK_RATE, self.device)
        return clockrate.value


def get_device_count():
    device_count = ctypes.c_int()
    cuda().cuDeviceGetCount(ctypes.byref(device_count))
    return device_count.value


def compute_capabilities():
    # Get all compute capabilities and keep only unique ones.
    unique_ccs = set(map(lambda i : Device(i).compute_capability, range(get_device_count())))
    # format the set into a semicolon separated string.
    return ';'.join(map(str, unique_ccs))


if __name__ == "__main__":
    from docopt import docopt
    args = docopt(__doc__)

    if   args['device_count']:
        print(get_device_count())
    elif args['compute_capabilities']:
        print(compute_capabilities())
    elif args['-i']:
        d = Device(int(args['<device>']))
        device_attr = list(filter(lambda a : not a.startswith('_'), dir(Device)))
        if args['<property>'] in device_attr:
            print(getattr(d, args['<property>']))
        else:
            print(f'Error <property> must be either {" ".join(device_attr)}')
