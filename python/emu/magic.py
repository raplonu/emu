import sys
from io import StringIO
from typing import Any, List
from IPython import get_ipython
import pyperclip as clip
from pampy import match


class Capturing(List[Any]):

    def __enter__(self):
        self._stdout = sys.stdout
        sys.stdout = self._stringio = StringIO()
        return self

    def __exit__(self, *args):
        self.extend(self._stringio.getvalue().splitlines())
        del self._stringio    # free up some memory
        sys.stdout = self._stdout


def parse_args(arg):
    return match(arg,
                 int  , str,
                 tuple, lambda t: ' '.join(map(parse_args, t)),
                 slice, lambda s: f'{s.start}-{s.stop}')


class Exporter:
    '''Copy to clipboard lines provided as value or ranges in argument.

    In [1]: from emu.magic import export

    In [2]: a = 1

    In [3]: b = 2

    In [4]: c = x #Opps
    ---------------------------------------------------------------------------
    NameError                                 Traceback (most recent call last)
    <ipython-input-4-2ec591279030> in <module>
    ----> 1 c = x #Opps

    NameError: name 'x' is not defined

    In [5]: c = 3

    In [6]: export[2:3, 5] # Copy lines 2,3 and 5 in clipboard.
    '''
    def __getitem__(self, args):
        ipy = get_ipython()
        with Capturing() as output:
            ipy.magic(f'%history {parse_args(args)}')

        clip.copy('\n'.join(output))


export = Exporter()
# Alias of export
ex = export
