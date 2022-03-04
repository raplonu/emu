import _emu


def add(i, j):
    ''' This is a function '''
    return _emu.add(i, j)


def fun():
    ''' This is a function '''
    _emu.fun()


class Toto:
    ''' This is a class '''
    def __init__(self, i):
        self.i = i

    def add(self, j):
        return self.i + j

    def fun(self):
        _emu.fun()
