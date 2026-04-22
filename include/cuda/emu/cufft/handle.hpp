#pragma once

#include <emu/cufft/error.hpp>

#include <emu/cuda.hpp>
#include <emu/scoped.hpp>
#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>
#include <emu/span.hpp>

namespace emu::cufft
{

    struct handle_t;

namespace handle
{

    using id_t = cufftHandle;

namespace detail
{

    id_t create();

    void destroy(id_t id);

    struct Destroyer{
        void operator()(id_t id) const { destroy(id); }
    };

    void set_stream(id_t handle, ::emu::cuda::stream_id mode);

} // namespace detail

    using ScopedHandle = emu::scoped<const id_t, detail::Destroyer>;

} // namespace handle

    struct handle_t
    {
        handle_t();

        handle_t(handle::id_t id, ::emu::cuda::device_id device_id, bool owning);

        handle_t(::emu::cuda::device_id device_id);

        constexpr handle_t(handle_t && o) = default;
        handle_t(const handle_t &) = delete;

        handle_t& operator=(handle_t &&) = default;
        handle_t& operator=(const handle_t &) = delete;

        handle::id_t id() const noexcept { return id_.value(); }
        ::emu::cuda::device_id device_id() const noexcept { return device_id_; }

        void set_stream(const ::emu::cuda::stream_ref & stream);

        handle_t & enable();

        const handle_t & enable() const;

        ~handle_t() = default;

    private:
        handle::ScopedHandle id_;
        ::emu::cuda::device_id device_id_;
    };

namespace handle
{
    handle_t create();

    handle_t wrap(id_t id, const ::emu::cuda::device_ref& device, bool take_ownership);

} // namespace handle

} // namespace emu::cufft
