#pragma once

#include <emu/type_traits.hpp>
#include <emu/utility.hpp>

#include <iterator>
#include <ranges>
#include <span>

namespace emu
{

    using std::dynamic_extent;

namespace detail
{

    template <typename T, size_t Extent, typename Validator>
    struct basic_span : protected std::span<T, Extent> {

        using base = std::span<T, Extent>;

        using element_type =     typename base::element_type;
        using value_type =       typename base::value_type;
        using size_type =        typename base::size_type;
        using difference_type =  typename base::difference_type;
        using pointer =          typename base::pointer;
        using const_pointer =    typename base::const_pointer;
        using reference =        typename base::reference;
        using const_reference =  typename base::const_reference;
        using iterator =         typename base::iterator;
        using reverse_iterator = typename base::reverse_iterator;

        static constexpr size_t extent = Extent;

        using validator_type = Validator;

        template <typename Type>
        inline static constexpr bool validate = Validator::template validate<Type>;

        constexpr basic_span() noexcept = default;

        template <std::contiguous_iterator It>
        constexpr explicit(extent != dynamic_extent)
            basic_span(It first, size_t count) noexcept
            : base(first, count) {}

        template <std::contiguous_iterator It, std::sized_sentinel_for<It> End>
            requires(not std::is_convertible_v<End, size_t>)
        constexpr explicit(extent != dynamic_extent)
            basic_span(It first, End last) noexcept(noexcept(last - first))
            : base(first, last) {}

        template <size_t ArrayExtent>
            requires(extent == dynamic_extent or extent == ArrayExtent)
        constexpr basic_span(tid<T> (&arr)[ArrayExtent]) noexcept : base(arr) {}

        template <typename U, size_t ArrayExtent>
            requires (extent == dynamic_extent or extent == ArrayExtent)
                 and (validate<std::array<U, ArrayExtent>>)
        constexpr basic_span(std::array<U, ArrayExtent> &arr) noexcept
            : base(arr) {}

        template <typename U, size_t ArrayExtent>
            requires (extent == dynamic_extent or extent == ArrayExtent)
                 and (validate<const std::array<U, ArrayExtent>>)
        constexpr basic_span(const std::array<U, ArrayExtent> &arr) noexcept
            : base(arr) {}

        template<std::ranges::contiguous_range Range>
        requires validate<Range>
        constexpr explicit(extent != dynamic_extent)
        basic_span(Range&& range)
            noexcept(noexcept(std::ranges::data(range))
		         and noexcept(std::ranges::size(range)))
            : base(EMU_FWD(range)) {}

        basic_span(const basic_span &) noexcept = default;

        template<typename OT, size_t OExtent>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_span(const basic_span<OT, OExtent, Validator> &other) noexcept
            : base(static_cast<const base&>(other)) {}

        template<typename OT, size_t OExtent>
        requires (extent == dynamic_extent
              or OExtent == dynamic_extent
               or extent == OExtent)
        constexpr explicit(extent != dynamic_extent && OExtent != dynamic_extent)
        basic_span(const std::span<OT, OExtent> &other) noexcept
            : base(other) {}

        ~basic_span() noexcept = default;

        constexpr basic_span &operator=(const basic_span &) noexcept = default;

        using base::begin;
        using base::end;
        using base::rbegin;
        using base::rend;

        using base::first;
        using base::last;
        using base::subspan;

        using base::front;
        using base::back;
        using base::operator[];
        using base::data;

        using base::size;
        using base::size_bytes;
        using base::empty;

    };

    template<typename Type, size_t ArrayExtent, typename Validator>
    basic_span(Type(&)[ArrayExtent])
        -> basic_span<Type, ArrayExtent, Validator>;

    template<typename Type, size_t ArrayExtent, typename Validator>
    basic_span(std::array<Type, ArrayExtent>&)
        -> basic_span<Type, ArrayExtent, Validator>;

    template<typename Type, size_t ArrayExtent, typename Validator>
    basic_span(const std::array<Type, ArrayExtent>&)
      -> basic_span<const Type, ArrayExtent, Validator>;

    template<std::contiguous_iterator Iter, typename End, typename Validator>
    basic_span(Iter, End)
      -> basic_span<std::remove_reference_t<std::iter_reference_t<Iter>>, dynamic_extent, Validator>;

    template<std::ranges::contiguous_range Range, typename Validator>
    basic_span(Range &&)
      -> basic_span<std::remove_reference_t<std::ranges::range_reference_t<Range&>>, dynamic_extent, Validator>;

    template <typename T, auto Extent>
    constexpr auto type_extent = Extent == dynamic_extent
                                    ? dynamic_extent
                                    : Extent / sizeof(T);

    template <typename T, auto Extent>
    constexpr auto byte_extent = Extent == dynamic_extent
                                    ? dynamic_extent
                                    : Extent * sizeof(T);


} // namespace detail

// Use of reinterpret_cast is safe here as it is used to cast between byte and T
// Note that you can convert T1 to byte and then to T2 using as_t. Be careful with this.
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)

    template <typename NewType, size_t Extent, typename Validator>
    constexpr auto as_t(detail::basic_span<byte, Extent, Validator> sp) noexcept
        -> detail::basic_span<NewType, detail::type_extent<NewType, Extent>, Validator>
    {
        auto data = reinterpret_cast<NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);

        return {data, size};
    }

    template <typename NewType, size_t Extent, typename Validator>
    constexpr auto as_t(detail::basic_span<const byte, Extent, Validator> sp) noexcept
        -> detail::basic_span<NewType, detail::type_extent<const NewType, Extent>, Validator>
    {
        auto data = reinterpret_cast<const NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);

        return {data, size};
    }

    template<typename Type, size_t Extent, typename Validator>
    constexpr auto
    as_bytes(detail::basic_span<Type, Extent, Validator> sp) noexcept
        -> detail::basic_span<const byte, detail::byte_extent<Type, Extent>, Validator>
    {
      auto data = reinterpret_cast<const byte*>(sp.data());
      auto size = sp.size_bytes();

      return {data, size};
    }

  template<typename Type, size_t Extent, typename Validator>
    requires (not is_const<Type>)
    constexpr auto
    as_writable_bytes(detail::basic_span<Type, Extent, Validator> sp) noexcept
        -> detail::basic_span<const byte, detail::byte_extent<Type, Extent>, Validator>
    {
      auto data = reinterpret_cast<byte*>(sp.data());
      auto size = sp.size_bytes();

      return {data, size};
    }

// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

    template <typename T, size_t Extent, typename Validator>
    auto as_auto_bytes(detail::basic_span<T, Extent, Validator> sp) {
        if constexpr (std::is_const_v<T>)
            return as_bytes(sp);
        else
            return as_writable_bytes(sp);
    }

} // namespace emu

// Opt-in to borrowed_range concept
template<typename ElementType, std::size_t Extent, typename Validator>
inline constexpr bool
std::ranges::enable_borrowed_range<emu::detail::basic_span<T, Extent, Validator>> = true;

// Opt-in to view concept
template<typename ElementType, std::size_t Extent, typename Validator>
inline constexpr bool
std::ranges::enable_view<emu::detail::basic_span<T, Extent, Validator>>
    = Extent == 0 || Extent == std::dynamic_extent;
