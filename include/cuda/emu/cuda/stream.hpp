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

    inline handle_t create(bool synchronizes_with_default_stream)
    {
        handle_t handle;
        auto flags = synchronizes_with_default_stream ? cudaStreamDefault : cudaStreamNonBlocking;
        EMU_CHECK_OR_THROW_WHAT(cudaStreamCreateWithFlags(&handle, flags),
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

    stream_t create(device_t device, bool synchronizes_with_default_stream);

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

        bool ready() const {
            // Check if the stream is ready for more work
            return true; // Placeholder, actual implementation may vary
        }

        friend stream_t stream::create(device_t, bool);
        friend stream_t stream::wrap(stream::handle_t, bool);

    private:
        stream::scoped_handle handle_;
    };

namespace stream
{

    inline stream_t create(device_t device, bool synchronizes_with_default_stream = false) {
        device.make_current();

        return stream_t(stream::detail::create(synchronizes_with_default_stream), true);
    }

    inline stream_t wrap(stream::handle_t handle, bool take_ownership) {
        return stream_t(handle, take_ownership);
    }

} // namespace stream

} // namespace emu::cuda
