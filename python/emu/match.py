from typing import Union


def AnyOf(*args):
    '''
    Same as Union but with values
    '''
    # types = map(type, args)
    class Toto(Union[int]):
        def __init__(self):
            self.__args__ = args
        __origin__ = Union
    return Toto()
