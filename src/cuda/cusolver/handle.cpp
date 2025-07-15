#include <emu/cusolver/handle.hpp>

namespace emu::cusolver
{

namespace handle::detail
{
    id_t create() {
        id_t id = nullptr;
        EMU_CHECK_OR_THROW(cusolverDnCreate(&id));
        return id;
    }

    void destroy(id_t id) {
        EMU_CHECK_OR_THROW(cusolverDnDestroy(id));
    }

    void set_stream(id_t handle, emu::cuda::stream::handle_t stream) {
            EMU_CHECK_OR_THROW(cusolverDnSetStream(handle, stream));
    }

    emu::cuda::stream::handle_t get_stream(id_t handle) {
            emu::cuda::stream::handle_t stream = nullptr;
            EMU_CHECK_OR_THROW(cusolverDnGetStream(handle, &stream));
            return stream;
    }

} // namespace handle::detail

handle_t::handle_t():
    id_(handle::detail::create(), true),
    device_id_(emu::cuda::device::current().id())
{}

handle_t::handle_t(handle::id_t id, emu::cuda::device::id_t device_id, bool owning):
    id_(id, owning),
    device_id_(device_id)
{}

handle_t::handle_t(emu::cuda::device::id_t device_id):
    id_([&]{
        ::emu::cuda::device::detail::set_current(device_id);
        return handle::detail::create();
    }(), true),
    device_id_(device_id)
{}


void handle_t::set_stream(const ::emu::cuda::stream_t & stream) const {
    handle::detail::set_stream(id(), stream.handle());
}


::emu::cuda::stream_t handle_t::stream() const {
    return ::emu::cuda::stream::wrap(
        handle::detail::get_stream(id()),
        false);
}

namespace handle
{
    handle_t create() {
        return {};
    }

    handle_t create(const ::emu::cuda::device_t& device) {
        return { device.id() };
    }

    handle_t wrap(id_t id, const ::emu::cuda::device_t& device, bool take_ownership) {
        return { id, device.id(), take_ownership };
    }

} // namespace handle

} // namespace emu::cusolver
