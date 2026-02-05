#pragma once

#include <emu/scoped_base.hpp>
#include <emu/cuda/device.hpp>

#include <cuda/stream_ref>
#include <cuda/stream>

namespace emu::cuda
{

    using stream_id = cudaStream_t;

    /// Owning handle to a CUDA stream. Manages the lifetime of the stream and provides utility functions.
    using ::cuda::stream;
    /// Non-owning reference to a CUDA stream. Does not manage the lifetime of the stream.
    using ::cuda::stream_ref;

namespace detail
{

    struct stream_destroyer
    {
        void operator()(stream_ref ref) const {
            auto _ = stream::from_native_handle(ref.get()); // borrow and delete
        }
    };

} // namespace detail

    struct stream_handle : scoped_base<stream_ref, detail::stream_destroyer>
    {
    protected:
        using base = scoped_base<stream_ref, detail::stream_destroyer>;

    public:
        stream_handle(device_ref device, int priority = stream::default_priority)
        : base(stream(device, priority).release(), true)
        {}

        stream_handle(stream_ref ref, bool take_ownership)
            : base(ref, take_ownership)
        {}

        [[nodiscard]] static stream_handle from_native_handle(stream_id id, bool take_ownership)
        {
            return stream_handle(stream_ref(id), take_ownership);
        }

        // Disallow construction from an `int`, e.g., `0`.
        static stream_handle from_native_handle(int, bool) = delete;

        // Disallow construction from `nullptr`.
        static stream_handle from_native_handle(::std::nullptr_t, bool) = delete;
    };

} // namespace emu::cuda
