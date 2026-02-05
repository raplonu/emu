#pragma once

#include <emu/scoped_base.hpp>
#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>

#include <cuda/stream> // cudsa event are defined in cuda/stream.

#include <chrono>

namespace emu::cuda
{

    using event_id = cudaEvent_t;

    using ::cuda::event;
    using ::cuda::event_ref;
    using ::cuda::timed_event;

    using ::cuda::event_flags;

namespace detail
{

    struct event_destroyer
    {
        void operator()(event_ref ref) const {
            auto _ = event::from_native_handle(ref.get()); // borrow and delete
        }
    };

} // namespace detail

    struct event_handle : scoped_base<event_ref, detail::event_destroyer>
    {
    protected:
        using base = scoped_base<event_ref, detail::event_destroyer>;

    public:
        event_handle(stream_ref stream, event_flags flags = event_flags::none)
            : base(event(stream, flags).release(), true)
        {}

        event_handle(device_ref device, event_flags flags = event_flags::none)
            : base(event(device, flags).release(), true)
        {}

        event_handle(event_ref ref, bool take_ownership)
            : base(ref, take_ownership)
        {}

        [[nodiscard]] static event_handle from_native_handle(event_id id, bool take_ownership)
        {
            return event_handle(event_ref(id), take_ownership);
        }

        // Disallow construction from an `int`, e.g., `0`.
        static event_handle from_native_handle(int, bool) = delete;

        // Disallow construction from `nullptr`.
        static event_handle from_native_handle(::std::nullptr_t, bool) = delete;
    };

    struct timed_event_handle : scoped_base<event_ref, detail::event_destroyer>
    {
    protected:
        using base = scoped_base<event_ref, detail::event_destroyer>;

    public:
        timed_event_handle(stream_ref stream, event_flags flags = event_flags::none)
            : base(timed_event(stream, flags).release(), true)
        {}

        timed_event_handle(device_ref device, event_flags flags = event_flags::none)
            : base(timed_event(device, flags).release(), true)
        {}

        timed_event_handle(event_ref ref, bool take_ownership)
            : base(ref, take_ownership)
        {}

        [[nodiscard]] static timed_event_handle from_native_handle(event_id id, bool take_ownership)
        {
            return timed_event_handle(event_ref(id), take_ownership);
        }

        // Disallow construction from an `int`, e.g., `0`.
        static timed_event_handle from_native_handle(int, bool) = delete;

        // Disallow construction from `nullptr`.
        static timed_event_handle from_native_handle(::std::nullptr_t, bool) = delete;

        [[nodiscard]] friend auto operator-(const timed_event_handle& end, const timed_event_handle& start)
        {
            auto start_tmp = timed_event::from_native_handle(start.get()); // borrow
            auto end_tmp = timed_event::from_native_handle(end.get()); // borrow

            auto result = end_tmp - start_tmp; // compute elapsed time, leave it to libcuxx's timed_event.

            [[maybe_unused]] auto _ = start_tmp.release(); // release borrow
                                  _ = end_tmp.release(); // release borrow

            return result;
        }

        [[nodiscard]] friend auto operator-(const timed_event_handle& end, const timed_event& start)
        {
            auto end_tmp = timed_event::from_native_handle(end.get()); // borrow

            auto result = end_tmp - start; // compute elapsed time, leave it to libcuxx's timed_event.

            [[maybe_unused]] auto _ = end_tmp.release(); // release borrow

            return result;
        }

        [[nodiscard]] friend auto operator-(const timed_event& end, const timed_event_handle& start)
        {
            auto start_tmp = timed_event::from_native_handle(start.get()); // borrow

            auto result = end - start_tmp; // compute elapsed time, leave it to libcuxx's timed_event.

            [[maybe_unused]] auto _ = start_tmp.release(); // release borrow

            return result;
        }
    };

} // namespace emu::cuda
