#pragma once

#include <emu/fwd.hpp>
#include <emu/concepts.hpp>
#include <emu/assert.hpp>

#include <string>
#include <string_view>

#include <fmt/base.h>

namespace emu
{

    /**
     * @struct null_terminated_t
     * @brief A tag type used to select constructors that expect a null-terminated string.
     */
    struct null_terminated_t {};

    /**
     * @var null_terminated
     * @brief An instance of the null_terminated_t tag.
     */
    constexpr null_terminated_t null_terminated;

    /**
     * @class basic_cstring_view
     * @brief A non-owning view of a null-terminated string.
     *
     * This class extends `std::basic_string_view` to guarantee that the underlying
     * character sequence is null-terminated. It is useful for interfacing with
     * C-style APIs that require null-terminated strings, without the need to
     * create a `std::string`.
     *
     * @tparam CharT The character type.
     * @tparam Traits The character traits type.
     */
    template<typename CharT, typename Traits = std::char_traits<CharT> >
    struct basic_cstring_view : public std::basic_string_view<CharT, Traits>
    {
        using string_view_type = std::basic_string_view<CharT, Traits>;

        using traits_type = typename string_view_type::traits_type;
        using value_type = typename string_view_type::value_type;
        using pointer = typename string_view_type::pointer;
        using const_pointer = typename string_view_type::const_pointer;
        using reference = typename string_view_type::reference;
        using const_reference = typename string_view_type::const_reference;
        using const_iterator = typename string_view_type::const_iterator;
        using iterator = typename string_view_type::iterator;
        using const_reverse_iterator = typename string_view_type::const_reverse_iterator;
        using reverse_iterator = typename string_view_type::reverse_iterator;
        using size_type = typename string_view_type::size_type;
        using difference_type = typename string_view_type::difference_type;

        static constexpr size_type npos = string_view_type::npos;

        /**
         * @brief Default constructor. Creates an empty view.
         */
        constexpr basic_cstring_view() noexcept = default;

        /**
         * @brief Constructs a view from a pointer and a count, asserting null-termination.
         * @param s A pointer to the character array.
         * @param count The number of characters in the string.
         * @note This constructor asserts that `s[count]` is the null terminator.
         */
        constexpr basic_cstring_view(null_terminated_t, const CharT* s, size_type count ) noexcept
            : string_view_type(s, count)
        {
            EMU_ASSERT_MSG(s[count] == '\0', "basic_cstring_view: string is not null-terminated");
        }

        /**
         * @brief Constructs a view from a `std::basic_string_view`, asserting null-termination.
         * @param cv A `std::basic_string_view` to construct from.
         * @note This constructor asserts that the character after the view's end is the null terminator.
         */
        constexpr basic_cstring_view(null_terminated_t, string_view_type cv) noexcept
            : string_view_type(cv)
        {
            EMU_ASSERT_MSG(cv[cv.size()] == '\0', "basic_cstring_view: string is not null-terminated");
        }

        /**
         * @brief Constructs a view from a null-terminated C-style string.
         * @param s A pointer to a null-terminated character array.
         */
        constexpr basic_cstring_view( const CharT* s ) noexcept
            : string_view_type(s)
        {}

        /**
         * @brief Constructs a view from any object that satisfies the `c_string` concept.
         * @tparam CR The type of the C-string compatible object.
         * @param cr The C-string compatible object.
         */
        template<cpts::c_string CR>
            requires cpts::not_equivalent<CR, basic_cstring_view>
        constexpr basic_cstring_view(CR && cr) noexcept
            : string_view_type(cr.c_str(), cr.size())
        {
            EMU_ASSERT_MSG(cr.c_str()[cr.size()] == '\0', "basic_cstring_view: string is not null-terminated");
        }

        /**
         * @brief Deleted constructor to prevent construction from `nullptr`.
         */
        constexpr basic_cstring_view( std::nullptr_t ) = delete;

        constexpr basic_cstring_view( const basic_cstring_view& other ) noexcept = default;
        constexpr basic_cstring_view( basic_cstring_view&& other ) noexcept = default;

        constexpr basic_cstring_view& operator=( const basic_cstring_view& other ) noexcept = default;
        constexpr basic_cstring_view& operator=( basic_cstring_view&& other ) noexcept = default;

        constexpr ~basic_cstring_view() = default;

        /**
         * @brief Implicit conversion to `std::basic_string_view`.
         */
        constexpr operator string_view_type() const noexcept {
            return *this;
        }

        /**
         * @brief Deleted to prevent accidental modification of the view's start.
         */
        void remove_prefix( size_type n ) = delete;

        /**
         * @brief Returns a new `basic_cstring_view` that is a suffix of this view.
         * @param pos The starting position of the substring.
         * @return A new `basic_cstring_view` starting at `pos`.
         */
        [[nodiscard]] constexpr basic_cstring_view substr(size_type pos = 0) const {
            return {null_terminated, string_view_type::substr(pos)};
        }

        /**
         * @brief Returns a `std::basic_string_view` that is a substring of this view.
         * @param pos The starting position of the substring.
         * @param n The length of the substring.
         * @return A `std::basic_string_view` of the specified substring.
         */
        [[nodiscard]] constexpr string_view_type substr(size_type pos, size_type n) const {
            return string_view_type::substr(pos, n);
        }

        /**
         * @brief Returns a pointer to the underlying null-terminated character array.
         * @return A const pointer to the C-style string.
         */
        [[nodiscard]] constexpr const CharT* c_str() const noexcept {
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
    /**
     * @brief User-defined literal for creating a `basic_cstring_view<char>`.
     */
    constexpr basic_cstring_view<char>
    operator ""_csv( const char* str, std::size_t len ) noexcept {
        return {null_terminated, str, len};
    }

    /**
     * @brief User-defined literal for creating a `basic_cstring_view<wchar_t>`.
     */
    constexpr basic_cstring_view<wchar_t>
    operator ""_csv( const wchar_t* str, std::size_t len ) noexcept {
        return {null_terminated, str, len};
    }

    /**
     * @brief User-defined literal for creating a `basic_cstring_view<char8_t>`.
     */
    constexpr basic_cstring_view<char8_t>
    operator ""_csv( const char8_t* str, std::size_t len ) noexcept {
        return {null_terminated, str, len};
    }

    /**
     * @brief User-defined literal for creating a `basic_cstring_view<char16_t>`.
     */
    constexpr basic_cstring_view<char16_t>
    operator ""_csv( const char16_t* str, std::size_t len ) noexcept {
        return {null_terminated, str, len};
    }

    /**
     * @brief User-defined literal for creating a `basic_cstring_view<char32_t>`.
     */
    constexpr basic_cstring_view<char32_t>
    operator ""_csv( const char32_t* str, std::size_t len ) noexcept {
        return {null_terminated, str, len};
    }

} // namespace literals

    using cstring_view    = basic_cstring_view< char     >;
    using cwstring_view   = basic_cstring_view< wchar_t  >;
    using cu8string_view  = basic_cstring_view< char8_t  >;
    using cu16string_view = basic_cstring_view< char16_t >;
    using cu32string_view = basic_cstring_view< char32_t >;

    inline std::string fun(cstring_view csv) {
        return std::string(csv);
    }

namespace cpts
{

    template<typename T>
    concept string_view = specialization_of<T, std::basic_string_view>;

    template<typename T>
    concept cstring_view = specialization_of<T, basic_cstring_view >;

    template<typename T>
    concept any_string_view = string_view<T>
                           or cstring_view<T>;


} // namespace cpts


    static_assert(std::is_convertible_v<const cstring_view&, std::string_view>);


} // namespace emu

template<typename CharT, typename Traits>
struct std::hash<emu::basic_cstring_view<CharT, Traits>> : std::hash<std::basic_string_view<CharT, Traits>> {
    constexpr std::size_t operator()(emu::basic_cstring_view<CharT, Traits> csv) const noexcept {
        return std::hash<std::basic_string_view<CharT, Traits>>::operator()(csv);
    }
};

template<typename CharT, typename Traits>
struct fmt::formatter<emu::basic_cstring_view<CharT, Traits>, CharT>
    : fmt::formatter<std::basic_string_view<CharT, Traits>, CharT>
{

    using base = fmt::formatter<std::basic_string_view<CharT, Traits>, CharT>;
    using cstring_view = emu::basic_cstring_view<CharT, Traits>;
    using string_view = std::basic_string_view<CharT, Traits>;

    auto format(cstring_view v, format_context& ctx) const
        -> format_context::iterator
    {
        return base::format(static_cast<string_view>(v), ctx);
    }
};
