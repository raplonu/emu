#include <emu/cublas/handle.hpp>

#include <emu/cuda/device.hpp>

namespace emu::cublas
{

namespace handle::detail
{
    id_t create() {
        id_t id = nullptr;
        throw_if_error(cublasCreate(&id));
        return id;
    }

    void destroy(id_t id) {
        throw_if_error(cublasDestroy(id));
    }

    emu::cuda::stream::handle_t get_stream(id_t handle) {
        emu::cuda::stream::handle_t stream = nullptr;
        throw_if_error(cublasGetStream(handle, &stream));
        return stream;
    }

    void set_stream(id_t handle, emu::cuda::stream::handle_t stream) {
        throw_if_error(cublasSetStream(handle, stream));
    }

    cublasMath_t get_math_mode(id_t handle) {
        cublasMath_t mode{};
        throw_if_error(cublasGetMathMode(handle, &mode));
        return mode;
    }

    void set_math_mode(id_t handle, cublasMath_t mode) {
        throw_if_error(cublasSetMathMode(handle, mode));
    }

    cublasPointerMode_t get_pointer_mode(id_t handle) {
        cublasPointerMode_t mode{};
        throw_if_error(cublasGetPointerMode(handle, &mode));
        return mode;
    }

    void set_pointer_mode(id_t handle, cublasPointerMode_t mode) {
        throw_if_error(cublasSetPointerMode(handle, mode));
    }

} // namespace handle::detail

handle_t::handle_t():
    id_(handle::detail::create(), true),
    device_id_(::emu::cuda::device::current().id())
{}

handle_t::handle_t(handle::id_t id, ::emu::cuda::device::id_t device_id, bool owning):
    id_(id, owning),
    device_id_(device_id)
{}

handle_t::handle_t(::emu::cuda::device::id_t device_id):
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
        false
    );
}

void handle_t::set_math_mode(cublasMath_t mode) const {
    handle::detail::set_math_mode(id(), mode);
}

cublasMath_t handle_t::math_mode() const {
    return handle::detail::get_math_mode(id());
}

void handle_t::set_pointer_mode(cublasPointerMode_t mode) const {
    handle::detail::set_pointer_mode(id(), mode);
}

cublasPointerMode_t handle_t::pointer_mode() const {
    return handle::detail::get_pointer_mode(id());
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

    handle_t create(const ::emu::cuda::device_t& device) {
        return { device.id() };
    }

    handle_t wrap(id_t id, const ::emu::cuda::device_t& device, bool take_ownership) {
        return { id, device.id(), take_ownership };
    }

} // namespace handle

} // namespace emu::cublas
