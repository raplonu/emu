#pragma once

#include <emu/scoped.hpp>
#include <emu/cuda/error.hpp>
#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>

#include <cuda_runtime_api.h>

namespace emu::cuda
{
    struct event_t;
    struct event_ref;

namespace event
{
    using handle_t = cudaEvent_t;

namespace detail
{
    inline handle_t create()
    {
        handle_t handle;
        EMU_CHECK_OR_THROW_WHAT(cudaEventCreateWithFlags(&handle, cudaEventDisableTiming),
                                     "Failed to create CUDA event");
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
        EMU_CHECK_OR_THROW_WHAT(cudaEventRecord(handle, stream_handle),
                                     "Failed to create CUDA event");
    }

    inline void synchronize(handle_t handle)
    {
        EMU_CHECK_OR_THROW_WHAT(cudaEventSynchronize(handle),
                                     "Failed to create CUDA event");
    }

    inline void wait(handle_t handle, stream::handle_t stream_handle)
    {
        EMU_CHECK_OR_THROW_WHAT(cudaStreamWaitEvent(stream_handle, handle, 0),
                                     "Failed to create CUDA event");
    }

} // namespace detail

    using scoped_handle = scoped<handle_t, detail::Destroyer>;

    event_t create(const device_t& device);

    event_t wrap(event::handle_t handle, bool take_ownership);

} // namespace event

    struct event_ref
    {
        using value_type = event::handle_t;

        constexpr event_ref() noexcept = default;

        constexpr event_ref(value_type handle) noexcept
            : handle_(handle)
        {}

        /// Disallow construction from an `int`, e.g., `0`.
        event_ref(int) = delete;

        /// Disallow construction from `nullptr`.
        event_ref(std::nullptr_t) = delete;

        [[nodiscard]] value_type get() const noexcept { return handle_; }

        void wait() const
        {
            event::detail::synchronize(handle_);
        }

        void wait(stream_ref stream) const
        {
            event::detail::wait(handle_, stream.get());
        }

    protected:
        value_type handle_{};
    };

    struct event_t
    {
    protected:
        event_t(event::handle_t handle, bool owning)
        : handle_(handle, owning)
        {}

    public:
        event_t(event_t &&) = default;
        event_t(const event_t &) = delete;

        event_t& operator=(event_t &&) = default;
        event_t& operator=(const event_t &) = delete;

        [[nodiscard]] event::handle_t get() const noexcept { return handle_.value; }

        [[nodiscard]] operator event_ref() const noexcept {
            return event_ref(handle_.value);
        }

        void record(const stream_t& stream) const {
            event::detail::record(handle_.value, stream.get());
        }

        void wait() const {
            event::detail::synchronize(handle_.value);
        }

        void wait(const stream_t& stream) const {
            event::detail::wait(handle_.value, stream.get());
        }

        friend event_t event::create(const device_t& device);
        friend event_t event::wrap(event::handle_t handle, bool take_ownership);

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
