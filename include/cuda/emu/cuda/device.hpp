#pragma once

#include <emu/scoped.hpp>
#include <emu/cuda/error.hpp>

#include <cuda_runtime_api.h>

namespace emu::cuda
{
    struct device_t;

namespace device
{
    using id_t = int;

namespace detail
{
    inline id_t get_current()
    {
        id_t device_id;
        EMU_CUDA_CHECK_THROW_ERROR(cudaGetDevice(&device_id));
        return device_id;
    }

    inline void set_current(id_t device_id)
    {
        EMU_CUDA_CHECK_THROW_ERROR(cudaSetDevice(device_id));
    }

    inline void synchronize(id_t device_id)
    {
        EMU_CUDA_CHECK_THROW_ERROR(cudaSetDevice(device_id));
        EMU_CUDA_CHECK_THROW_ERROR(cudaDeviceSynchronize());
    }

} // namespace detail

} // namespace device

    struct device_t
    {
        device_t(device::id_t id) : id_(id) {}

        device::id_t id() const noexcept { return id_; }

        void make_current() const {
            device::detail::set_current(id_);
        }

        bool operator==(const device_t& other) const {
            return id_ == other.id_;
        }

        bool operator!=(const device_t& other) const {
            return !(*this == other);
        }

        void synchronize() const {
            device::detail::synchronize(id_);
        }

    private:
        device::id_t id_;
    };

namespace device
{
    inline device_t get(device::id_t id)
    {
        detail::set_current(id);
        return device_t(id);
    }

    inline device_t current()
    {
        return device_t(detail::get_current());
    }

} // namespace device

namespace devices
{
    inline std::size_t count()
    {
        int count;
        EMU_CUDA_CHECK_THROW_ERROR(cudaGetDeviceCount(&count));
        return static_cast<std::size_t>(count);
    }

}

} // namespace emu::cuda
