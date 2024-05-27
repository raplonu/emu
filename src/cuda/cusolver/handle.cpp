#include <emu/cusolver/handle.hpp>

namespace emu::cusolver
{

namespace handle::detail
{
    id_t create() {
        id_t id = nullptr;
        throw_if_error(cusolverDnCreate(&id));
        return id;
    }

    void destroy(id_t id) {
        throw_if_error(cusolverDnDestroy(id));
    }

    void set_stream(id_t handle, ::cuda::stream::handle_t stream) {
            throw_if_error(cusolverDnSetStream(handle, stream));
    }

    ::cuda::stream::handle_t get_stream(id_t handle) {
            ::cuda::stream::handle_t stream = nullptr;
            throw_if_error(cusolverDnGetStream(handle, &stream));
            return stream;
    }

} // namespace handle::detail

handle_t::handle_t():
    id_(handle::detail::create(), true),
    device_id_(::cuda::device::current::get().id())
{}

handle_t::handle_t(handle::id_t id, ::cuda::device::id_t device_id, bool owning):
    id_(id, owning),
    device_id_(device_id)
{}

handle_t::handle_t(::cuda::device::id_t device_id):
    id_(emu::set_and_invoke(::cuda::device::get(device_id), handle::detail::create), /* owning = */ true),
    device_id_(device_id)
{}


void handle_t::set_stream(const ::cuda::stream_t & stream) const {
    handle::detail::set_stream(id(), stream.handle());
}


::cuda::stream_t handle_t::stream() const {
    return cuda::stream::wrap(device_id_, handle::detail::get_stream(id()), /* take_ownership = */ false);
}

namespace handle
{
    handle_t create() {
        return {};
    }

    handle_t create(const ::cuda::device_t& device) {
        return { device.id() };
    }

    handle_t wrap(id_t id, const ::cuda::device_t& device, bool take_ownership) {
        return { id, device.id(), take_ownership };
    }

} // namespace handle

} // namespace emu::cusolver
