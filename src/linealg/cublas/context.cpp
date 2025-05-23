#include <emu/blas/cublas/context.hpp>

namespace emu::linealg::cublas
{

namespace detail
{
    handle_t create() {
        handle_t handle = nullptr;
        throw_if_error(cublasCreate(&handle));
        return handle;
    }

    void destroy(handle_t handle) {
        throw_if_error(cublasDestroy(handle));
    }

    ::cuda::stream::handle_t get_stream(handle_t handle) {
        ::cuda::stream::handle_t stream = nullptr;
        throw_if_error(cublasGetStream(handle, &stream));
        return stream;
    }

    void set_stream(handle_t handle, ::cuda::stream::handle_t stream) {
        throw_if_error(cublasSetStream(handle, stream));
    }

    cublasMath_t get_math_mode(handle_t handle) {
        cublasMath_t mode{};
        throw_if_error(cublasGetMathMode(handle, &mode));
        return mode;
    }

    void set_math_mode(handle_t handle, cublasMath_t mode) {
        throw_if_error(cublasSetMathMode(handle, mode));
    }

    cublasPointerMode_t get_pointer_mode(handle_t handle) {
        cublasPointerMode_t mode{};
        throw_if_error(cublasGetPointerMode(handle, &mode));
        return mode;
    }

    void set_pointer_mode(handle_t handle, cublasPointerMode_t mode) {
        throw_if_error(cublasSetPointerMode(handle, mode));
    }

} // namespace detail

    context::context():
        handle_(detail::create(), true),
        device_id_(::cuda::device::current::get().id())
    {}

    context::context(handle_t handle, ::cuda::device::id_t device_id, bool owning):
        handle_(handle, owning),
        device_id_(device_id)
    {}

    context::context(::cuda::device::id_t device_id):
        handle_(emu::set_and_invoke(::cuda::device::get(device_id), detail::create), true),
        device_id_(device_id)
    {}

    void context::set_stream(const ::cuda::stream_t & stream) const {
        detail::set_stream(handle(), stream.handle());
    }

    ::cuda::stream_t context::stream() const {
        auto pc = ::cuda::device::get(device_id_).primary_context(::cuda::do_not_hold_primary_context_refcount_unit);
        ::cuda::device::primary_context::detail_::increase_refcount(device_id_);

        return ::cuda::stream::wrap(
            device_id_,
            pc.handle(),
            detail::get_stream(handle()),
            false, ::cuda::do_hold_primary_context_refcount_unit);
    }

    void context::set_math_mode(cublasMath_t mode) const {
        detail::set_math_mode(handle(), mode);
    }

    cublasMath_t context::math_mode() const {
        return detail::get_math_mode(handle());
    }

    void context::set_pointer_mode(cublasPointerMode_t mode) const {
        detail::set_pointer_mode(handle(), mode);
    }

    cublasPointerMode_t context::pointer_mode() const {
        return detail::get_pointer_mode(handle());
    }

    context & context::enable() {
        make_current(device_id_);

        return *this;
    }

    const context & context::enable() const {
        make_current(device_id_);

        return *this;
    }

    context make_context() {
        return {};
    }

    context make_context(const ::cuda::device_t& device) {
        return { device.id() };
    }

    context wrap_context(handle_t handle, const ::cuda::device_t& device, bool take_ownership) {
        return { handle, device.id(), take_ownership };
    }

} // namespace emu::linealg::cublas
