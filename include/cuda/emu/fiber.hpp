#pragma once

#include <emu/cuda.h>

namespace emu
{

namespace cuda
{
    struct fiber_t {
        ::cuda::stream_t stream;
        ::cuda::event_t  event;

        void synchronize(fiber_t & other) {

        }

        void notify(fiber_t & other) {

        }

        void wait(fiber_t & other) {

        }
    };

namespace fiber
{
    fiber_t create(
        device_t    device,
        bool        synchronizes_with_default_stream,
        priority_t  priority = stream::default_priority,
        bool        uses_blocking_sync = sync_by_busy_waiting, // Yes, that's the runtime default
        bool        records_timing     = do_record_timings,
        bool        interprocess       = not_interprocess
    );

    fiber_t wrap()
} // namespace fiber


} // namespace cuda

} // namespace emu
