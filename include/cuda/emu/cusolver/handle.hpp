#pragma once

#include <emu/cusolver/error.hpp>

#include <emu/cuda.hpp>
#include <emu/scoped.hpp>
#include <emu/cuda/device.hpp>

namespace emu::cusolver
{

class handle_t;

namespace handle
{

    using id_t = cusolverDnHandle_t;

namespace detail
{
        id_t create();

        void destroy(id_t id);

        struct Destroyer{
            void operator()(id_t id) const { destroy(id); }
        };

        void set_stream(id_t handle, ::cuda::stream::handle_t stream);

        ::cuda::stream::handle_t get_stream(id_t handle);

} // namespace detail

    using ScopedHandle = scoped<const id_t, detail::Destroyer>;
} // namespace handle

struct handle_t
{
    handle_t();

    handle_t(handle::id_t id, ::cuda::device::id_t device_id, bool owning);

    handle_t(::cuda::device::id_t device_id);

    constexpr handle_t(handle_t && o) = default;
    handle_t(const handle_t &) = delete;

    handle_t& operator=(handle_t &&) = default;
    handle_t& operator=(const handle_t &) = delete;

    [[nodiscard]] handle::id_t id() const noexcept { return id_.value; }

    ~handle_t() = default;

    void set_stream(const ::cuda::stream_t & stream) const;
    [[nodiscard]] ::cuda::stream_t stream() const;

private:
    handle::ScopedHandle id_;
    ::cuda::device::id_t device_id_;
};


namespace handle
{
    /**
     * Create cusolver handle_t on current device.
     */
    handle_t create();

    /**
     * Create cusolver handle_t on current device.
     */
    handle_t create(const ::cuda::device_t& device);

    handle_t wrap(id_t id, const ::cuda::device_t& device, bool take_ownership);

} // namespace handle

} // namespace emu::cusolver
