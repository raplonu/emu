#pragma once

#include <emu/concepts.hpp>
#include <emu/assert.hpp>

#include <string_view>
#include <concepts>

namespace emu
{

    struct null_terminated_t {};
    constexpr null_terminated_t null_terminated;

    template<typename CharT, typename Traits = std::char_traits<CharT>>
    struct basic_cstring_view : public std::basic_string_view<CharT, Traits>
    {
        using string_view_type = std::basic_string_view<CharT, Traits>;

        using traits_type = string_view_type::traits_type;
        using value_type = string_view_type::value_type;
        using pointer = string_view_type::pointer;
        using const_pointer = string_view_type::const_pointer;
        using reference = string_view_type::reference;
        using const_reference = string_view_type::const_reference;
        using const_iterator = string_view_type::const_iterator;
        using iterator = string_view_type::iterator;
        using const_reverse_iterator = string_view_type::const_reverse_iterator;
        using reverse_iterator = string_view_type::reverse_iterator;
        using size_type = string_view_type::size_type;
        using difference_type = string_view_type::difference_type;

        static constexpr size_type npos = string_view_type::npos;

        constexpr basic_cstring_view() noexcept = default;
        constexpr basic_cstring_view( const basic_cstring_view& other ) noexcept = default;

        constexpr basic_cstring_view(null_terminated_t, const CharT* s, size_type count ) noexcept
            : string_view_type(s, count)
        {
            EMU_ASSERT_MSG(s[count] == '\0', "basic_cstring_view: string is not null-terminated");
        }

        constexpr basic_cstring_view(null_terminated_t, string_view_type cv) noexcept
            : string_view_type(cv)
        {
            EMU_ASSERT_MSG(cv[cv.size()] == '\0', "basic_cstring_view: string is not null-terminated");
        }


        constexpr basic_cstring_view( const CharT* s ) noexcept
            : string_view_type(s)
        {}

        template<cpts::c_string CR>
            requires cpts::not_equivalent<CR, basic_cstring_view>
        constexpr basic_cstring_view(CR && cr) noexcept
            : string_view_type(cr.c_str(), cr.size())
        {
            EMU_ASSERT_MSG(cr.c_str()[cr.size()] == '\0', "basic_cstring_view: string is not null-terminated");
        }

        constexpr basic_cstring_view( std::nullptr_t ) = delete;

        constexpr ~basic_cstring_view() = default;

        void remove_prefix( size_type n ) = delete;

        constexpr basic_cstring_view substr(size_type pos = 0) const {
            return {null_terminated, string_view_type::substr(pos)};
        }
        constexpr string_view_type substr(size_type pos, size_type n) const {
            return string_view_type::substr(pos, n);
        }

        constexpr const CharT* c_str() const noexcept {
            return string_view_type::data();
        }

    };

    template< class CharT, class Traits >
    constexpr auto operator<=>( basic_cstring_view<CharT, Traits> lhs, basic_cstring_view<CharT, Traits> rhs ) noexcept {
        using sv_t = basic_cstring_view<CharT, Traits>::string_view_type;
        return static_cast<sv_t>(lhs) <=> static_cast<sv_t>(rhs);
    }

namespace literals
{
    constexpr basic_cstring_view<char>
    operator ""_csv( const char* str, std::size_t len ) noexcept {
        return basic_cstring_view<char>(null_terminated, str, len);
    }

    constexpr basic_cstring_view<wchar_t>
    operator ""_csv( const wchar_t* str, std::size_t len ) noexcept {
        return basic_cstring_view<wchar_t>(null_terminated, str, len);
    }

    constexpr basic_cstring_view<char8_t>
    operator ""_sv( const char8_t* str, std::size_t len ) noexcept {
        return basic_cstring_view<char8_t>(null_terminated, str, len);
    }

    constexpr basic_cstring_view<char16_t>
    operator ""_sv( const char16_t* str, std::size_t len ) noexcept {
        return basic_cstring_view<char16_t>(null_terminated, str, len);
    }

    constexpr basic_cstring_view<char32_t>
    operator ""_sv( const char32_t* str, std::size_t len ) noexcept {
        return basic_cstring_view<char32_t>(null_terminated, str, len);
    }

} // namespace literals

    using cstring_view     = basic_cstring_view< char     >;
    using c_wstring_view   = basic_cstring_view< wchar_t  >;
    using c_u8string_view  = basic_cstring_view< char8_t  >;
    using c_u16string_view = basic_cstring_view< char16_t >;
    using c_u32string_view = basic_cstring_view< char32_t >;

} // namespace emu

template<typename CharT, typename Traits>
struct std::hash<emu::basic_cstring_view<CharT, Traits>> : std::hash<std::basic_string_view<CharT, Traits>> {
    constexpr std::size_t operator()(emu::basic_cstring_view<CharT, Traits> csv) const noexcept {
        return std::hash<std::basic_string_view<CharT, Traits>>::operator()(csv);
    }
};

template<typename CharT, typename Traits>
struct fmt::formatter<emu::basic_cstring_view<CharT, Traits>, CharT> : fmt::formatter<std::basic_string_view<CharT, Traits>, CharT> {

    using base = fmt::formatter<std::basic_string_view<CharT, Traits>, CharT>;
    using cstring_view = emu::basic_cstring_view<CharT, Traits>;
    using string_view = std::basic_string_view<CharT, Traits>;

    template <typename FormatContext>
    auto format(cstring_view v, FormatContext& ctx) {
        return base::format(static_cast<string_view>(v), ctx);
    }
};