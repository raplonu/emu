#include <emu/cublas/handle.h>

#include <emu/device.h>

#include <emu/span.h>

namespace emu
{

namespace cufft
{

namespace handle
{

namespace detail
{
    id_t create() {
        id_t id;
        throw_if_error(cufftCreate(&id));
        return id;
    }

    void destroy(id_t id) {
        throw_if_error(cufftDestroy(id));
    }

    // ::cuda::stream::id_t get_stream(id_t handle) {
    //     ::cuda::stream::id_t stream;
    //     throw_if_error(cublasGetStream(handle, &stream));
    //     return stream;
    // }

    void set_stream(id_t handle, ::cuda::stream::id_t stream) {
        throw_if_error(cufftSetStream(handle, stream));
    }

    // cublasMath_t get_math_mode(id_t handle) {
    //     cublasMath_t mode;
    //     throw_if_error(cublasGetMathMode(handle, &mode));
    //     return mode;
    // }

    // void set_math_mode(id_t handle, cublasMath_t mode) {
    //     throw_if_error(cublasSetMathMode(handle, mode));
    // }

    // cublasPointerMode_t get_pointer_mode(id_t handle) {
    //     cublasPointerMode_t mode;
    //     throw_if_error(cublasGetPointerMode(handle, &mode));
    //     return mode;
    // }

    // void set_pointer_mode(id_t handle, cublasPointerMode_t mode) {
    //     throw_if_error(cublasSetPointerMode(handle, mode));
    // }

} // namespace detail

} // namespace handle

handle_t::handle_t():
    id_(handle::detail::create(), true),
    device_id_(::cuda::device::current::get().id())
{}

handle_t::handle_t(handle::id_t id, ::cuda::device::id_t device_id, bool owning):
    id_(id, owning),
    device_id_(device_id)
{}

handle_t::handle_t(::cuda::device::id_t device_id):
    id_(emu::set_and_invoke(::cuda::device::get(device_id), handle::detail::create), true),
    device_id_(device_id)
{}

void handle_t::set_stream(const ::cuda::stream_t & stream) {
    handle::detail::set_stream(id(), stream.handle());
}

// ::cuda::stream_t handle_t::stream() const {
//     return ::cuda::stream::detail_::wrap(device_id_, handle::detail::get_stream(id()), false);
// }

void handle_t::set_math_mode(cublasMath_t mode) {
    handle::detail::set_math_mode(id(), mode);
}

cublasMath_t handle_t::math_mode() const {
    return handle::detail::get_math_mode(id());
}

void handle_t::set_pointer_mode(cublasPointerMode_t mode) {
    handle::detail::set_pointer_mode(id(), mode);
}

cublasPointerMode_t handle_t::pointer_mode() const {
    return handle::detail::get_pointer_mode(id());
}

handle_t & handle_t::enable() {
    make_current(device_id_);

    return *this;
}

const handle_t & handle_t::enable() const {
    make_current(device_id_);

    return *this;
}

namespace handle
{
    handle_t create() {
        return {};
    }

    handle_t create(::cuda::device_t device) {
        return { device.id() };
    }

    handle_t wrap(id_t id, ::cuda::device_t device, bool take_ownership) {
        return { id, device.id(), take_ownership };
    }

} // namespace handle

} // namespace cufft

} // namespace emu