#pragma once

#include <emu/scoped.hpp>
#include <emu/cuda/error.hpp>
#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>

#include <cuda_runtime_api.h>

namespace emu::cuda
{
    struct event_t;

namespace event
{
    using handle_t = cudaEvent_t;

namespace detail
{
    inline handle_t create()
    {
        handle_t handle;
        EMU_CUDA_CHECK_THROW_ERROR(cudaEventCreateWithFlags(&handle, cudaEventDisableTiming));
        return handle;
    }

    inline void destroy(handle_t handle)
    {
        cudaEventDestroy(handle);
    }

    struct Destroyer
    {
        void operator()(handle_t handle) const { destroy(handle); }
    };

    inline void record(handle_t handle, stream::handle_t stream_handle)
    {
        EMU_CUDA_CHECK_THROW_ERROR(cudaEventRecord(handle, stream_handle));
    }

    inline void synchronize(handle_t handle)
    {
        EMU_CUDA_CHECK_THROW_ERROR(cudaEventSynchronize(handle));
    }

    inline void wait(handle_t handle, stream::handle_t stream_handle)
    {
        EMU_CUDA_CHECK_THROW_ERROR(cudaStreamWaitEvent(stream_handle, handle, 0));
    }

} // namespace detail

    using scoped_handle = scoped<handle_t, detail::Destroyer>;

} // namespace event

    struct event_t
    {
    public:
        event_t(event::handle_t handle, bool owning)
            : handle_(handle, owning)
        {}

        event_t(event_t &&) = default;
        event_t(const event_t &) = delete;

        event_t& operator=(event_t &&) = default;
        event_t& operator=(const event_t &) = delete;

        [[nodiscard]] event::handle_t handle() const noexcept { return handle_.value; }

        void record(const stream_t& stream) const {
            event::detail::record(handle_.value, stream.handle());
        }

        void synchronize() const {
            event::detail::synchronize(handle_.value);
        }

        void wait(const stream_t& stream) const {
            event::detail::wait(handle_.value, stream.handle());
        }

    private:
        event::scoped_handle handle_;
    };

namespace event
{
    inline event_t create(const device_t& device) {
        device::detail::set_current(device.id());
        return event_t(event::detail::create(), true);
    }

    inline event_t wrap(event::handle_t handle, bool take_ownership) {
        return event_t(handle, take_ownership);
    }

} // namespace event

} // namespace emu::cuda
