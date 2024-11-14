#pragma once

#include <emu/hash.hpp>

#include <map>
#include <set>

#include <unordered_map>
#include <unordered_set>

namespace emu
{

    template<
        class Key,
        class T,
        class Compare = std::less<>,
        class Allocator = std::allocator<std::pair<const Key, T>>
    >
    using dict = std::map<Key, T, Compare, Allocator>;

    template<
        class Key,
        class T,
        class Hash = ::emu::hash<Key>,
        class KeyEqual = std::equal_to<>,
        class Allocator = std::allocator< std::pair<const Key, T> >
    >
    using unordered_map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

    template<
        class Key,
        class Compare = std::less<>,
        class Allocator = std::allocator<Key>
    >
    using set = std::set<Key, Compare, Allocator>;

    template<
        class Key,
        class Hash = emu::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator<Key>
    >
    using unordered_set = std::unordered_set<Key, Hash, KeyEqual, Allocator>;


} // namespace emu
