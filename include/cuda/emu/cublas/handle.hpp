#ifndef EMU_CUBLAS_HANDLE_H
#define EMU_CUBLAS_HANDLE_H

#include <emu/cublas/error.hpp>

#include <emu/cuda.hpp>
#include <emu/scoped.hpp>
#include <emu/device.hpp>

namespace emu
{

namespace cublas
{

    struct handle_t;

namespace handle
{

    using id_t = cublasHandle_t;

namespace detail
{
    id_t create();

    void destroy(id_t id);

    struct Destroyer{
        void operator()(id_t id) const { destroy(id); }
    };

    ::cuda::stream::handle_t get_stream(id_t handle);

    void set_stream(id_t handle, ::cuda::stream::handle_t mode);

    cublasMath_t get_math_mode(id_t handle);

    void set_math_mode(id_t handle, cublasMath_t mode);

    cublasPointerMode_t get_pointer_mode(id_t handle);

    void set_pointer_mode(id_t handle, cublasPointerMode_t mode);

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

        handle::id_t id() const noexcept { return id_.value; }

        void set_stream(const ::cuda::stream_t & stream);

        ::cuda::stream_t stream() const;

        void set_math_mode(cublasMath_t mode);

        cublasMath_t math_mode() const;

        void set_pointer_mode(cublasPointerMode_t mode);

        cublasPointerMode_t pointer_mode() const;

        handle_t & enable();

        const handle_t & enable() const;

        ~handle_t() = default;

    private:
        handle::ScopedHandle id_;
        ::cuda::device::id_t device_id_;
    };

namespace handle
{
    /**
     * Create cublas handle_t on current device.
     */
    handle_t create();

    /**
     * Create cublas handle_t on current device.
     */
    handle_t create(::cuda::device_t device);

    handle_t wrap(id_t id, ::cuda::device_t device, bool take_ownership);

} // namespace handle

} // namespace cublas

} // namespace emu

#endif //EMU_CUBLAS_HANDLE_H