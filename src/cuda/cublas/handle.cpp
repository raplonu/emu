#include <emu/cublas/handle.hpp>

#include <emu/cuda/device.hpp>

namespace emu::cublas
{

namespace handle::detail
{
    id_t create() {
        id_t id = nullptr;
        EMU_CHECK_OR_THROW(cublasCreate(&id));
        return id;
    }

    void destroy(id_t id) {
        EMU_CHECK_OR_THROW(cublasDestroy(id));
    }

    emu::cuda::stream_id get_stream(id_t handle) {
        emu::cuda::stream_id stream = nullptr;
        EMU_CHECK_OR_THROW(cublasGetStream(handle, &stream));
        return stream;
    }

    void set_stream(id_t handle, emu::cuda::stream_id stream) {
        EMU_CHECK_OR_THROW(cublasSetStream(handle, stream));
    }

    cublasMath_t get_math_mode(id_t handle) {
        cublasMath_t mode{};
        EMU_CHECK_OR_THROW(cublasGetMathMode(handle, &mode));
        return mode;
    }

    void set_math_mode(id_t handle, cublasMath_t mode) {
        EMU_CHECK_OR_THROW(cublasSetMathMode(handle, mode));
    }

    cublasPointerMode_t get_pointer_mode(id_t handle) {
        cublasPointerMode_t mode{};
        EMU_CHECK_OR_THROW(cublasGetPointerMode(handle, &mode));
        return mode;
    }

    void set_pointer_mode(id_t handle, cublasPointerMode_t mode) {
        EMU_CHECK_OR_THROW(cublasSetPointerMode(handle, mode));
    }

} // namespace handle::detail

handle_t::handle_t():
    id_(handle::detail::create(), true),
    device_(::emu::cuda::current_device())
{}

handle_t::handle_t(handle::id_t id, ::emu::cuda::device_ref device, bool owning):
    id_(id, owning),
    device_(device)
{}

handle_t::handle_t(::emu::cuda::device_ref device)
    : id_(EMU_CUDA_SET_DEVICE_AND_ASSIGN(device, handle::detail::create()), true)
    , device_(device)
{}

void handle_t::set_stream(::emu::cuda::stream_ref stream) const {
    handle::detail::set_stream(id(), stream.get());
}

::emu::cuda::stream_ref handle_t::stream() const {
    return ::emu::cuda::stream_ref(
        handle::detail::get_stream(id())
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
    ::emu::cuda::set_current(device_);
    return *this;
}

const handle_t & handle_t::enable() const {
    ::emu::cuda::set_current(device_);
    return *this;
}

namespace handle
{
    handle_t create() {
        return {};
    }

    handle_t create(::emu::cuda::device_ref device) {
        return { device.get() };
    }

    handle_t wrap(id_t id, ::emu::cuda::device_ref device, bool take_ownership) {
        return { id, device.get(), take_ownership };
    }

} // namespace handle

} // namespace emu::cublas
