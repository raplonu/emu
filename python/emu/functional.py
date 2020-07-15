import functools as ft
import funcy as fc
from funcy import partial, compose, rcompose

import  boltons.cacheutils as cu
# from cachetools.keys import hashkey

import operator

def to_function(fn):
    def to_function_impl(*args, **kwargs):
        return fn(*args, **kwargs)
    return to_function_impl

class CachedFunction(cu.CachedFunction):
    """This type is used by :func:`cached`, below. This Version fix an issue with
    cu.CachedFunction  where the `scoped` member is simply unused. `scoped` adds
    the function to the cache key.
    """
    def __call__(self, *args, **kwargs):
        cache = self.get_cache()
        arg = [self.func, *args] if self.scoped else args
        key = self.key_func(arg, kwargs, typed=self.typed)
        if key in cache:
            return cache[key]
        else:
            ret = cache[key] = self.func(*args, **kwargs)
            return ret

# Reuse the documentation of cu.cached
# @ft.wraps(cu.cached) # disabled for now
def cached(cache, scoped=True, typed=False, key=None):
    """Cache any function with the cache object of your choosing. Note
    that the function wrapped should take only `hashable`_ arguments.
    Args:
        cache (Mapping): Any :class:`dict`-like object suitable for
            use as a cache. Instances of the :class:`LRU` and
            :class:`LRI` are good choices, but a plain :class:`dict`
            can work in some cases, as well. This argument can also be
            a callable which accepts no arguments and returns a mapping.
        scoped (bool): Whether the function itself is part of the
            cache key.  ``True`` by default, different functions will
            not read one another's cache entries, but can evict one
            another's results. ``False`` can be useful for certain
            shared cache use cases. More advanced behavior can be
            produced through the *key* argument.
        typed (bool): Whether to factor argument types into the cache
            check. Default ``False``, setting to ``True`` causes the
            cache keys for ``3`` and ``3.0`` to be considered unequal.
    >>> my_cache = LRU()
    >>> @cached(my_cache)
    ... def cached_lower(x):
    ...     return x.lower()
    ...
    >>> cached_lower("CaChInG's FuN AgAiN!")
    "caching's fun again!"
    >>> len(my_cache)
    1
    .. _hashable: https://docs.python.org/2/glossary.html#term-hashable
    """
    def cached_func_decorator(func):
        return CachedFunction(func, cache, scoped=scoped, typed=typed, key=key)
    return cached_func_decorator

def rpartial(fun, *args2, **kwargs2):
    '''rpartial(func, *args, **keywords) - new function with reverse partial application
    of the given arguments and keywords.
    '''
    def rpartial_impl(*args1, **kwargs1):
        return fun(*args1, *args2, **kwargs1, **kwargs2)
    return rpartial_impl

def partial_apply(fun, *funs, **kwfuns):
    '''partial_apply(fun, *funs, **kwfuns) - new function with partial application
    of the given callable arguments and keywords.
    '''
    def partial_apply_impl(*args, **kwargs):
        return fun(*map_apply(funs), *args, **dict_apply(kwfuns), **kwargs)
    return partial_apply_impl

def rpartial_apply(fun, *funs, **kwfuns):
    '''rpartial_apply(fun, *funs, **kwfuns) - new function with reverse partial application
    of the given callable arguments and keywords.
    '''
    def rpartial_apply_impl(*args, **kwargs):
        return fun(*args, *map_apply(funs), **kwargs, **dict_apply(kwfuns))
    return rpartial_apply_impl

class Function:
    '''This type is used by :func:`boost_fn`, below.
    '''
    def __init__(self, fn):
        self.__fn = fn

    def __call__(self, *args, **kwargs):
        '''() operator'''
        return self.__fn(*args, **kwargs)

    def __mul__(self, ofn):
        '''* operator'''
        return Function(compose(self.__fn, Function.filter_fn(ofn)))

    def __or__(self, ofn):
        '''| operator'''
        return Function(rcompose(self.__fn, Function.filter_fn(ofn)))

    def __lshift__(self, arg):
        '''<< operator'''
        return Function(partial(self.__fn, arg))

    def __rshift__(self, arg):
        '''>> operator'''
        return Function(rpartial(self.__fn, arg))

    def __le__(self, fun):
        '''<= operator'''
        return Function(partial_apply(self.__fn, fun))

    def __ge__(self, fun):
        '''>= operator'''
        return Function(rpartial_apply(self.__fn, fun))

    @staticmethod
    def filter_fn(fn):
        return fn.__fn if isinstance(fn, Function) else fn

    def partial(self, *args, **kwargs):
        return Function(partial(self.__fn, *args, **kwargs))

    def rpartial(self, *args, **kwargs):
        return Function(rpartial(self.__fn, *args, **kwargs))

    def partial_apply(self, *args, **kwargs):
        return Function(partial_apply(self.__fn, *args, **kwargs))

    def rpartial_apply(self, *args, **kwargs):
        return Function(rpartial_apply(self.__fn, *args, **kwargs))

def boost_fn(fn):
    '''Enhanced function or callable with functional operators and member functions.
    Use `*` or `|` operator to respectively compose and reverse compose self
    with another callable. You can also generate new function with partial application
    of values or callables as following :

    |          | regular order | reverse order |
    |----------|---------------|---------------|
    | value    | partial       | rpartial      |
    | callable | partial_apply | rpatial_apply |

    Args:
        fn : a callable


    >>> @boost_fn
    ... def add(a, b):
    ...     return a + b
    ...
    >>> add_two = add << 2
    >>> add_two(1)
    3

    >>> @boost_fn
    ... def print2(p1, p2):
    ...     print(p1, ' and ', p2)
    ...
    >>> print_after_x = print2 << 'x'
    >>> print_after_x('y')
    x  and  y
    >>> print_before_x = print2 >> 'x'
    >>> print_before_x('y')
    y  and  x

    >>> @boost_fn
    ... def times(a, b):
    ...     return a * b
    ...
    >>> times_3_plus_2 = add_two * times << 3
    >>> times_3_plus_2(3)
    11 # 3 * 3 + 2
    '''
    return Function(fn)

# Alias of boost_fn
bf = boost_fn

### Boosted version of python built-in functions
b_abs =          bf(abs)
b_all =          bf(all)
b_any =          bf(any)
b_ascii =        bf(ascii)
b_bin =          bf(bin)
b_bool =         bf(bool)
b_breakpoint =   bf(breakpoint)
b_bytearray =    bf(bytearray)
b_bytes =        bf(bytes)
b_callable =     bf(callable)
b_chr =          bf(chr)
b_classmethod =  bf(classmethod)
b_compile =      bf(compile)
b_complex =      bf(complex)
b_delattr =      bf(delattr)
b_dict =         bf(dict)
b_dir =          bf(dir)
b_divmod =       bf(divmod)
b_enumerate =    bf(enumerate)
b_eval =         bf(eval)
b_exec =         bf(exec)
b_filter =       bf(filter)
b_float =        bf(float)
b_format =       bf(format)
b_frozenset =    bf(frozenset)
b_getattr =      bf(getattr)
b_globals =      bf(globals)
b_hasattr =      bf(hasattr)
b_hash =         bf(hash)
b_help =         bf(help)
b_hex =          bf(hex)
b_id =           bf(id)
b_input =        bf(input)
b_int =          bf(int)
b_isinstance =   bf(isinstance)
b_issubclass =   bf(issubclass)
b_iter =         bf(iter)
b_len =          bf(len)
b_list =         bf(list)
b_locals =       bf(locals)
b_map =          bf(map)
b_max =          bf(max)
b_memoryview =   bf(memoryview)
b_min =          bf(min)
b_next =         bf(next)
b_object =       bf(object)
b_oct =          bf(oct)
b_open =         bf(open)
b_ord =          bf(ord)
b_pow =          bf(pow)
b_print =        bf(print)
b_property =     bf(property)
b_range =        bf(range)
b_repr =         bf(repr)
b_reversed =     bf(reversed)
b_round =        bf(round)
b_set =          bf(set)
b_setattr =      bf(setattr)
b_slice =        bf(slice)
b_sorted =       bf(sorted)
b_staticmethod = bf(staticmethod)
b_str =          bf(str)
b_sum =          bf(sum)
b_super =        bf(super)
b_tuple =        bf(tuple)
b_type =         bf(type)
b_vars =         bf(vars)
b_zip =          bf(zip)

### Boosted version of functional components
b_first = bf(fc.first)
b_pred_all = bf(fc.all) # Differ from python `all` by using a predicate to perform evaluation.

@boost_fn
def void(*_, **__):
    '''Function that discard evething it takes.'''
    pass

@boost_fn
def dump_map(fn, *args):
    '''Map that directly evaluate the generator and discard the result.'''
    void(*map(fn, *args))

@boost_fn
def list_at(sequence, pos, default=None):
    '''Return the value at pos if pos is in the sequence, else default.
    '''
    return sequence[pos] if pos < len(sequence) else default

@boost_fn
def map_at(mappable, key, default=None):
    '''Return the value for key if key is in the dictionary, else default.
    '''
    return mappable.get(key, default)

@boost_fn
def map_at_or_implace(mappable, key, default=None):
    '''Return the value for key if key is in the dictionary, else implace default
    and return it.
    '''
    if not key in mappable:
        mappable[key] = default
    return mappable[key]

@boost_fn
def equal_to(arg1, arg2):
    '''Evaluate to `True` if arg1 and arg2 are equal `False` otherwise.
    '''
    return arg1 == arg2

# Return the first element of a sequence that respect the predicate.
first_filter = b_first * filter
# TODO are we sure to taking the pred
all_filter   = b_pred_all * filter

@boost_fn
def find_pos(seq, value):
    return list(seq).index(value)

# @boost_fn
# def field_match(key, value):
#     return (equal_to << value) * (at << key)

# @boost_fn
# def match_at(value, pos):
#     return (equal_to << value) * (at << pos)

@boost_fn
def pack(fn, *args):
    return fn(args)

@boost_fn
def unpack(fn, args):
    return fn(*args)


@boost_fn
def apply(fn, *args, **kwargs):
    return fn(*args, **kwargs)

@boost_fn
def rapply(*args, **kwargs):
    '''
    A function that will store arguments and use the last one as a function that will be call
    '''
    *args, fn = args
    return fn(*args, **kwargs)

zip_to_dict = b_dict * b_zip
map_to_list = b_list * b_map
map_apply = b_map << apply

def dict_apply(kwfuns):
    return zip_to_dict(kwfuns.keys(), map_apply(kwfuns.values()))


@boost_fn
def flat_map(range_, generator):
    for r in range_:
        for e in generator(r):
            yield e

def compose_gens(*gens):
    return lambda data :\
        ft.reduce(flat_map, [[data], *gens[::-1]])

@boost_fn
def explore(keys, data):
    return ft.reduce(operator.getitem, keys, data)

@boost_fn
def call_once(fn, *args, **kwargs):
    fn = partial(fn, *args, **kwargs)
    @boost_fn
    def call_once_impl():
        if not call_once_impl.called:
            call_once_impl.result = fn()
            call_once_impl.called = True
        return call_once_impl.result

    # Add the function member `called`
    call_once_impl.called = False
    return call_once_impl

@boost_fn
def fwd(x):
    return x

@boost_fn
def not_none_fwd_or(arg, fallback):
    return arg if arg is not None else fallback

def return_function(fn):
    def return_function_impl(*args, **kwargs):
        return fn
    return return_function_impl

@boost_fn
def first(range):
    res, *_ = range
    return res

@boost_fn
def tail(range):
    *_, last = range
    return last

transpose = unpack << (b_list * zip)
