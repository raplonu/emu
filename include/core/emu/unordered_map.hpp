#pragma once

#include <emu/concepts.hpp>
#include <emu/cstring_view.hpp>

#include <unordered_map>
#include <string>
#include <string_view>

namespace emu
{

namespace detail
{

    template<typename CharT, typename Traits>
    struct string_hash
    {
        using hash_type = std::hash<std::basic_string_view<CharT, Traits> >;
        using is_transparent = void;

        constexpr std::size_t operator()(const CharT* str) const noexcept { return hash_type{}(str); }
        constexpr std::size_t operator()(std::basic_string_view<CharT, Traits> str) const noexcept { return hash_type{}(str); }
        constexpr std::size_t operator()(emu::basic_cstring_view<CharT, Traits> str) const noexcept { return hash_type{}(str); }
        constexpr std::size_t operator()(const std::basic_string<CharT, Traits>& str) const noexcept { return hash_type{}(str); }
    };

    template<typename T>
    struct hash : std::hash<T>
    {};

    template<cpts::c_string String>
    struct hash<String> : string_hash<typename String::value_type, typename String::traits_type>
    {};

} // namespace detail


    template<
        class Key,
        class T,
        class Hash = detail::hash<Key>,
        class KeyEqual = std::equal_to<>,
        class Allocator = std::allocator< std::pair<const Key, T> >
    >
    using unordered_map = std::unordered_map<Key, T, Hash, KeyEqual, Allocator>;

} // namespace emu
