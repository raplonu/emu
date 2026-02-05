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

    void set_stream(id_t handle, emu::cuda::stream_id stream) {
            EMU_CHECK_OR_THROW(cusolverDnSetStream(handle, stream));
    }

    emu::cuda::stream_id get_stream(id_t handle) {
            emu::cuda::stream_id stream = nullptr;
            EMU_CHECK_OR_THROW(cusolverDnGetStream(handle, &stream));
            return stream;
    }

} // namespace handle::detail

handle_t::handle_t():
    id_(handle::detail::create(), true),
    device_(emu::cuda::current_device())
{}

handle_t::handle_t(handle::id_t id, emu::cuda::device_ref device, bool owning):
    id_(id, owning),
    device_(device)
{}

handle_t::handle_t(emu::cuda::device_ref device):
    id_(EMU_CUDA_SET_DEVICE_AND_ASSIGN(device, handle::detail::create()), true),
    device_(device)
{}


void handle_t::set_stream(::emu::cuda::stream_ref stream) const {
    handle::detail::set_stream(id(), stream.get());
}


::emu::cuda::stream_ref handle_t::stream() const {
    return ::emu::cuda::stream_ref(
        handle::detail::get_stream(id())
    );
}

namespace handle
{
    handle_t create() {
        return {};
    }

    handle_t create(emu::cuda::device_ref device) {
        return { device.get() };
    }

    handle_t wrap(id_t id, emu::cuda::device_ref device, bool take_ownership) {
        return { id, device.get(), take_ownership };
    }

} // namespace handle

} // namespace emu::cusolver
