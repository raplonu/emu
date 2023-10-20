#pragma once

#include <emu/location.h>
#include <emu/cuda.h>
#include <emu/cuda/allocator.h>

namespace emu
{

namespace cuda::location
{

    struct device
    {
        template<typename T>
        using default_allocator_type = emu::cuda::allocator<T>;

        int id;

        device(): id(::cuda::device::current::get().id()) {}

        device(int id) noexcept: id(id) {}

        template<typename T>
        default_allocator_type<T> make_default_allocator() {
            return default_allocator_type<T>(::cuda::device::get(id));
        }

    };

} // namespace cuda::location

    template<>
    struct spe::is_location<cuda::location::device> : std::true_type {};

} // namespace emu
