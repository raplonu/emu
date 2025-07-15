#pragma once

#include <emu/scoped.hpp>
#include <emu/cuda/error.hpp>
#include <emu/cuda/device.hpp>

#include <cuda/stream_ref>

#include <cuda_runtime_api.h>

namespace emu::cuda
{
    struct stream_t;

    using ::cuda::stream_ref;

namespace stream
{
    using handle_t = cudaStream_t;

namespace detail
{
    inline handle_t create()
    {
        handle_t handle;
        EMU_CHECK_OR_THROW_WHAT(cudaStreamCreateWithFlags(&handle, cudaStreamNonBlocking),
                                "Failed to create CUDA stream");
        return handle;
    }

    inline void destroy(handle_t handle)
    {
        cudaStreamDestroy(handle);
    }

    struct Destroyer
    {
        void operator()(handle_t handle) const { destroy(handle); }
    };

    inline void synchronize(handle_t handle)
    {
        EMU_CHECK_OR_THROW_WHAT(cudaStreamSynchronize(handle),
                                "Failed to create CUDA stream");
    }

    inline device::id_t get_device_id(handle_t handle)
    {
        device::id_t device_id;
        EMU_CHECK_OR_THROW_WHAT(cudaStreamGetDevice(handle, &device_id),
                                "Failed to create CUDA stream");
        return device_id;
    }

} // namespace detail

    using scoped_handle = scoped<handle_t, detail::Destroyer>;

    stream_t create(const device_t& device);

    stream_t wrap(stream::handle_t handle, bool take_ownership);

} // namespace stream

    struct stream_t
    {
    protected:
        stream_t(stream::handle_t handle, bool owning)
            : handle_(handle, owning)
        {}

    public:
        stream_t(stream_t &&) = default;
        stream_t(const stream_t &) = delete;

        stream_t& operator=(stream_t &&) = default;
        stream_t& operator=(const stream_t &) = delete;

        [[nodiscard]] stream::handle_t handle() const noexcept { return handle_.value; }
        [[nodiscard]] device::id_t device_id() const noexcept { return stream::detail::get_device_id(handle_.value); }

        [[nodiscard]] operator stream_ref() const noexcept {
            return stream_ref(handle_.value);
        }

        void wait() const {
            stream::detail::synchronize(handle_.value);
        }

        friend stream_t stream::create(const device_t& device);
        friend stream_t stream::wrap(stream::handle_t handle, bool take_ownership);

    private:
        stream::scoped_handle handle_;
    };

namespace stream
{
    inline stream_t create(const device_t& device) {
        device::detail::set_current(device.id());

        return stream_t(stream::detail::create(), true);
    }

    inline stream_t wrap(stream::handle_t handle, bool take_ownership) {
        return stream_t(handle, take_ownership);
    }

} // namespace stream

} // namespace emu::cuda
