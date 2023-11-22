#pragma once

#include <emu/cublas/error.hpp>

#include <emu/cuda.hpp>
#include <emu/scoped.hpp>
#include <emu/device.hpp>

namespace emu
{

namespace cufft
{

    struct handle_t;

namespace handle
{

    using id_t = cufftHandle;

namespace detail
{

    id_t create()

    template<typename SizeT>
    id_t create(emu::span_t<SizeT> shape);

    void destroy(id_t id);

    struct Destroyer{
        void operator()(id_t id) const { destroy(id); }
    };

    void set_stream(id_t handle, ::cuda::stream::id_t mode);

    // cublasMath_t get_math_mode(id_t handle);

    // void set_math_mode(id_t handle, cublasMath_t mode);

    // cublasPointerMode_t get_pointer_mode(id_t handle);

    // void set_pointer_mode(id_t handle, cublasPointerMode_t mode);

} // namespace detail

    using ScopedHandle = emu::scoped_t<const id_t, detail::Destroyer>;

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
     * Create cufft handle_t on current device.
     */
    template<typename T>
    handle_t create(emu::span_t<int> shape) {

    }

    template<typename T>
    handle_t create(emu::span_t<size_t> shape) {

    }

    // template<typename In, typename Out, typename SizeT>
    // handle_t create(emu::span_t<SizeT> shape);

    /**
     * Create cufft handle_t on current device.
     */
    // template<typename SizeT>
    // handle_t create(::cuda::device_t device, emu::span_t<SizeT> shape);

    // template<typename In, typename Out, typename SizeT>
    // handle_t create(::cuda::device_t device, emu::span_t<SizeT> shape);

    handle_t wrap(id_t id, ::cuda::device_t device, bool take_ownership);

} // namespace handle

} // namespace cufft

} // namespace emu
