#include <emu/cufft/handle.hpp>
#include <emu/cuda/device.hpp>

namespace emu::cufft
{

namespace handle::detail
{
    id_t create() {
        id_t id;
        throw_if_error(cufftCreate(&id));
        return id;
    }

    void destroy(id_t id) {
        throw_if_error(cufftDestroy(id));
    }

    void set_stream(id_t handle, ::emu::cuda::stream::id_t stream) {
        throw_if_error(cufftSetStream(handle, stream));
    }

} // namespace handle::detail

handle_t::handle_t():
    id_(handle::detail::create(), true),
    device_id_(::emu::cuda::device_t::current().id())
{}

handle_t::handle_t(handle::id_t id, ::emu::cuda::device::id_t device_id, bool owning):
    id_(id, owning),
    device_id_(device_id)
{}

handle_t::handle_t(::emu::cuda::device::id_t device_id):
    id_([&]{
        ::emu::cuda::device_t(device_id).make_current();
        return handle::detail::create();
    }(), true),
    device_id_(device_id)
{}

void handle_t::set_stream(const ::emu::cuda::stream_t & stream) {
    handle::detail::set_stream(id(), stream.id());
}

handle_t & handle_t::enable() {
    ::emu::cuda::device_t(device_id_).make_current();
    return *this;
}

const handle_t & handle_t::enable() const {
    ::emu::cuda::device_t(device_id_).make_current();
    return *this;
}

namespace handle
{
    handle_t create() {
        return {};
    }

    handle_t wrap(id_t id, const ::emu::cuda::device_t& device, bool take_ownership) {
        return { id, device.id(), take_ownership };
    }

} // namespace handle

} // namespace emu::cufft
