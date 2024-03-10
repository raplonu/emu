#pragma once

#include <emu/hash.hpp>

#include <unordered_set>

namespace emu
{

    template<
        class Key,
        class Hash = emu::hash<Key>,
        class KeyEqual = std::equal_to<Key>,
        class Allocator = std::allocator<Key>
    >
    using unordered_set = std::unordered_set<Key, Hash, KeyEqual, Allocator>;

} // namespace emu
