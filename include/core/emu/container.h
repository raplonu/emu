#ifndef EMU_CORE_CONTAINER_H
#define EMU_CORE_CONTAINER_H

#include <emu/utility.h>

#include <emu/misc/location.h>

// Include for accessor types.
#include <emu/mdspan.h>
#include <emu/scoped.h>
#include <emu/type_traits.h>

#include <memory>

namespace emu
{

namespace detail
{

namespace container
{

    template <
        typename T,
        typename Location,
        typename AccessorPolicy
    >
    struct interface_t
    {
        using element_type = T;
        using location_type = Location;
        using accessor_type = AccessorPolicy;
        using pointer = typename accessor_type::pointer;

        virtual ~interface_t() = default;

        virtual pointer data() const      = 0;
        virtual std::size_t size() const  = 0;
        virtual Location location() const = 0;

    };

    template <
        typename T,
        typename Location,
        typename AccessorPolicy,
        typename Capsule
    >
    struct implementation_t : interface_t<T, Location, AccessorPolicy>
    {
        using element_type = T;
        using location_type = Location;
        using accessor_type = AccessorPolicy;
        using pointer = typename accessor_type::pointer;


        pointer data_;
        std::size_t size_;
        Location location_;
        Capsule capsule_;

        template<typename C>
        implementation_t(pointer data, std::size_t size, Location location, C&& capsule)
            : data_(data), size_(size), location_(location), capsule_(EMU_FWD(capsule))
        {}

        pointer data() const override           { return data_;     }
        std::size_t size() const override       { return size_;     }
        location_type location() const override { return location_; }

    };

} // namespace container

    template <
        typename T,
        typename Location,
        typename AccessorPolicy = mdspan::default_accessor<T>
    >
    struct container_t
    {
        using element_type = T;
        using location_type = Location;
        using accessor_type = AccessorPolicy;
        using pointer = typename accessor_type::pointer;

        std::shared_ptr<container::interface_t<element_type, location_type, accessor_type>> impl_;

        container_t(pointer data, std::size_t size, Location location = {})
            : impl_(std::make_shared<container::implementation_t<T, Location, accessor_type, bool>>(data, size, location, false))
        {}

        template<typename Capsule>
        container_t(pointer data, std::size_t size, Capsule&& capsule, Location location = {})
            : impl_(std::make_shared<container::implementation_t<T, Location, accessor_type, Capsule>>(data, size, location, EMU_FWD(capsule)))
        {}

        pointer data() const           { return impl_->data();     }
        std::size_t size() const       { return impl_->size();     }
        location_type location() const { return impl_->location(); }

    };

} // namespace detail

    template <typename T>
    using container_t = detail::container_t<T, location::host_t, mdspan::default_accessor<T>>;

namespace container
{

    template <typename T>
    constexpr auto create(std::size_t size)
    {
        std::allocator<T> allocator;
        auto data = allocator.allocate(size);

        return container_t<T>(
            data,
            size,
            scoped::create([allocator = mv(allocator), data, size]() mutable { allocator.deallocate(data, size); })
        );
    }

    template <typename T, typename Location>
    constexpr auto create(std::size_t size, Location location)
    {
        auto allocator = location.template make_default_allocator<T>();
        auto data = allocator.allocate(size);

        return container_t<T>(
            data,
            size,
            scoped::create([allocator = mv(allocator), data, size]() mutable { allocator.deallocate(data, size); })
        );
    }

    template <typename T>
    constexpr auto wrap(T* data, std::size_t size)
    {
        return container_t<T>(data, size);
    }

    template <typename T, typename Location>
    constexpr auto wrap(T* data, std::size_t size, Location location)
    {
        return detail::container_t<T, RemoveCVRef<Location>>(data, size, location);
    }

    template <typename T>
    constexpr auto wrap(emu::span_t<T> data)
    {
        return container_t<T>(data.data(), data.size());
    }

    template <typename T, typename Location>
    constexpr auto wrap(emu::span_t<T> data, Location location)
    {
        return detail::container_t<T, RemoveCVRef<Location>>(data.data(), data.size(), location);
    }

    template <typename Container>
    constexpr auto wrap(Container&& con)
    {
        using std::data, std::size;
        auto ptr = data(con);

        using element_type = typename std::pointer_traits<decltype(ptr)>::element_type;

        return container_t<element_type>(
            ptr,
            size(con),
            EMU_FWD(con)
        );
    }

    template <typename Container, typename Location>
    constexpr auto wrap(Container&& con, Location location)
    {
        using std::data, std::size;
        auto ptr = data(con);

        using element_type = typename std::pointer_traits<decltype(ptr)>::element_type;

        return detail::container_t<element_type, RemoveCVRef<Location>>(
            ptr,
            size(con),
            EMU_FWD(con),
            location
        );
    }

} // namespace container

} // namespace emu

#endif // EMU_CORE_CONTAINER_H