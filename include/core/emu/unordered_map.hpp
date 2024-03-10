#pragma once

#include <emu/hash.hpp>

#include <unordered_map>

namespace emu
{

    template<
        class Key,
        class T,
        class Hash = ::emu::hash<Key>,
        class KeyEqual = std::equal_to<>,
        class Allocator = std::allocator< std::pair<const Key, T> >
    >
    using unordered_map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

} // namespace emu
