#pragma once

#include <type_traits>
#include <emu/detail/mdspan_types.hpp>

// Try to include CUDA accessor if available
// Do not rely on `EMU_CUDA` macro as it can be defined but user may still only use emu::core target.
#if __has_include(<cuda/mdspan>)
    #include <cuda/mdspan>
    #define HAS_CUDA_ACCESSORS
#endif // __has_include(<cuda/mdspan>)

namespace emu
{

namespace host
{


    template <typename Accessor>
    struct accessor : public Accessor
    {

        using base_accessor = Accessor;


    public:
        using offset_policy    = accessor<typename Accessor::offset_policy>;
        using data_handle_type = typename Accessor::data_handle_type;
        using reference        = typename Accessor::reference;
        using element_type     = typename Accessor::element_type;

    private:
        static constexpr bool is_access_noexcept =
            noexcept(::std::declval<Accessor>().access(::std::declval<data_handle_type>(), 0));

        static constexpr bool is_offset_noexcept =
            noexcept(::std::declval<Accessor>().offset(::std::declval<data_handle_type>(), 0));

    public:

        template<class Accessor2 = Accessor>
        requires ::std::is_default_constructible_v<Accessor2>
        constexpr accessor() noexcept(::std::is_nothrow_default_constructible_v<Accessor2>)
            : Accessor{}
        {}

        constexpr accessor(Accessor&& acc) noexcept(
            ::std::is_nothrow_move_constructible_v<Accessor>)
            : Accessor{::std::move(acc)}
        {}

        constexpr accessor(const Accessor& acc) noexcept(
            ::std::is_nothrow_copy_constructible_v<Accessor>)
            : Accessor{acc}
        {}

#ifdef HAS_CUDA_ACCESSORS

        template <typename OtherAccessor>
        accessor(const ::cuda::device_accessor<OtherAccessor>&) = delete;

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, const OtherAccessor&> and
            ::std::is_convertible_v<const OtherAccessor&, Accessor>
        constexpr accessor(const ::cuda::host_accessor<OtherAccessor>& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, const OtherAccessor&>)
            : Accessor{acc}
        {}

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, const OtherAccessor&> and
            (!::std::is_convertible_v<const OtherAccessor&, Accessor>)
        constexpr explicit accessor(const ::cuda::host_accessor<OtherAccessor>& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, const OtherAccessor&>)
            : Accessor{acc}
        {}

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, const OtherAccessor&> and
            ::std::is_convertible_v<const OtherAccessor&, Accessor>
        constexpr accessor(const ::cuda::managed_accessor<OtherAccessor>& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, const OtherAccessor&>)
            : Accessor{acc}
        {}

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, const OtherAccessor&> and
            (!::std::is_convertible_v<const OtherAccessor&, Accessor>)
        constexpr explicit accessor(const ::cuda::managed_accessor<OtherAccessor>& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, const OtherAccessor&>)
            : Accessor{acc}
        {}

#endif // HAS_CUDA_ACCESSORS

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, const OtherAccessor&> and
            ::std::is_convertible_v<const OtherAccessor&, Accessor>
        constexpr accessor(const accessor<OtherAccessor>& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, const OtherAccessor&>)
            : Accessor{acc}
        {}

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, const OtherAccessor&> and
            (!::std::is_convertible_v<const OtherAccessor&, Accessor>)
        constexpr explicit accessor(const accessor<OtherAccessor>& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, const OtherAccessor&>)
            : Accessor{acc}
        {}

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, OtherAccessor> and
            ::std::is_convertible_v<OtherAccessor, Accessor>
        constexpr accessor(accessor<OtherAccessor>&& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, OtherAccessor>)
            : Accessor{::std::move(acc)}
        {}

        template<typename OtherAccessor>
        requires ::std::is_constructible_v<Accessor, const OtherAccessor&> and
            (!::std::is_convertible_v<OtherAccessor, Accessor>)
        constexpr explicit accessor(accessor<OtherAccessor>&& acc) noexcept(
            ::std::is_nothrow_constructible_v<Accessor, OtherAccessor>)
            : Accessor{::std::move(acc)}
        {}

        constexpr reference access(data_handle_type p, size_t i) const noexcept(is_access_noexcept)
        {
            return Accessor::access(p, i);
        }

        [[nodiscard]] constexpr data_handle_type offset(data_handle_type p, size_t i) const
            noexcept(is_offset_noexcept)
        {
            return Accessor::offset(p, i);
        }

    };


} // namespace host

    template <typename>
    inline constexpr bool is_host_accessor_v = false;

    template <typename Accessor>
    inline constexpr bool is_host_accessor_v<host::accessor<Accessor>> = true;

    template <typename>
    inline constexpr bool is_host_accessible_v = false;

    template <typename Accessor>
    inline constexpr bool is_host_accessible_v<host::accessor<Accessor>> = true;

namespace cpts
{

    template <typename T>
    concept host_accessor = is_host_accessor_v<T>;

    template <typename T>
    concept host_accessible = is_host_accessible_v<T>;

} // namespace cpts

} // namespace emu
