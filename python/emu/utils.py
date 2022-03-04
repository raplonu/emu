from collections.abc import Iterable

from .operator import eq, ne
from .functional import apply


class AnyOf:

    def __init__(self, iterable: Iterable):
        self.__iterable = iterable

    def __eq__(self, other):
        return any(map(eq << other, self.__iterable))

    def __ne__(self, other):
        return any(map(ne << other, self.__iterable))

    def __call__(self, *args, **kwargs):
        return any(map(apply.rpartial(*args, **kwargs), self.__iterable))


def any_of(*args):
    return AnyOf(args)


class AllOf:

    def __init__(self, iterable: Iterable):
        self.__iterable = iterable

    def __eq__(self, other):
        return all(map(eq << other, self.__iterable))

    def __ne__(self, other):
        return all(map(ne << other, self.__iterable))

    def __call__(self, *args, **kwargs):
        return all(map(apply.rpartial(*args, **kwargs), self.__iterable))


def all_of(*args):
    return AllOf(args)
