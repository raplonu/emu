import subprocess as sp
from string import Formatter
from operator import add

from functional import boost_fn, Function, to_function, apply

@boost_fn
def get_stdout(obj):           return obj.stdout
@boost_fn
def get_stderr(obj):           return obj.stderr
@boost_fn
def get_args(obj):             return obj.args
@boost_fn
def get_returncode(obj):       return obj.returncode
@boost_fn
def get_check_returncode(obj): return obj.check_returncode()

def make_cmd(cmd):
    @boost_fn
    def run_cmd(*input):
        input = ' '.join(input) if len(input) else None
        return sp.run(cmd.split(), input=input, encoding='utf-8', capture_output=True)
    return run_cmd

_formatter = Formatter()

class DefaultFormatList(list):
    def __getitem__(self, pos):
        return list.__getitem__(self, pos) if pos < len(self) else '{}'

class DefaultFormatDict(dict):
    def __getitem__(self, key):
        return dict.__getitem__(self, key) if key in self else '{'+str(key)+'}'


@boost_fn
def pformat(field, *args, **kwargs):
    return _formatter.vformat(field, DefaultFormatList(args), DefaultFormatDict(kwargs))

def to_method(fn):
    return to_function(fn * apply)

@boost_fn
class Cmd(Function):
    def __init__(self, args):
        # Will return CompleteProcess object gather all the attributes bellow.
        self._args = args
        self.cp = make_cmd(args)

        self.append = Cmd * boost_fn(add) << (self._args + ' ')
        self.format = Cmd * pformat << self._args

        Function.__init__(self, self.cp)

    def __repr__(self):
        return self._args

    def __call__(self, *input):
        return self.cp(*input)

    def __add__(self, arg):
        '''
        operator + allow to append additional arguments at the end of the command
        '''
        return Cmd(self._args + ' ' + arg)

    def __mod__(self, arg):
        '''
        operator % allow to partially format the command with a the provided argument
        '''
        return Cmd(pformat(self._args, arg))

    stdout =           to_method(get_stdout)
    stderr =           to_method(get_stderr)
    args =             to_method(get_args)
    returncode =       to_method(get_returncode)
    check_returncode = to_method(get_check_returncode)


ls = Cmd('ls')
du = Cmd('du -b {}')
grep = Cmd('grep {}')