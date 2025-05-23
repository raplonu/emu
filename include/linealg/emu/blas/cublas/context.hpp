#pragma once

#include <emu/blas/blas_type.hpp>
#include <emu/blas/cublas/error.hpp>
#include <emu/blas/cublas/function.hpp>

#include <emu/cuda.hpp>
#include <emu/scoped.hpp>
#include <emu/cuda/device.hpp>

#include <emu/assert.hpp>

#include <cublas_v2.h>

namespace emu::linealg::cublas
{

    struct context;

    using handle_t = cublasHandle_t;

namespace detail
{
    handle_t create();

    void destroy(handle_t handle);

    struct Destroyer{
        void operator()(handle_t handle) const { destroy(handle); }
    };

    ::cuda::stream::handle_t get_stream(handle_t handle);

    void set_stream(handle_t handle, ::cuda::stream::handle_t stream);

    cublasMath_t get_math_mode(handle_t handle);

    void set_math_mode(handle_t handle, cublasMath_t mode);

    cublasPointerMode_t get_pointer_mode(handle_t handle);

    void set_pointer_mode(handle_t handle, cublasPointerMode_t mode);

} // namespace detail

    using scoped_handle = scoped<const handle_t, detail::Destroyer>;

    struct context
    {
        template<typename T>
        using blas_type = blas<T>;

        context();

        context(handle_t handle, ::cuda::device::id_t device_id, bool owning);

        context(::cuda::device::id_t device_id);

        constexpr context(context && o) = default;
        context(const context &) = delete;

        context& operator=(context &&) = default;
        context& operator=(const context &) = delete;

        [[nodiscard]] handle_t handle() const noexcept { return handle_.value; }
        [[nodiscard]] ::cuda::device::id_t device_id() const noexcept { return device_id_; }

        void set_stream(const ::cuda::stream_t & stream) const;

        [[nodiscard]] ::cuda::stream_t stream() const;

        void set_math_mode(cublasMath_t mode) const;

        [[nodiscard]] cublasMath_t math_mode() const;

        void set_pointer_mode(cublasPointerMode_t mode) const;

        [[nodiscard]] cublasPointerMode_t pointer_mode() const;

        context & enable();

        [[nodiscard]] const context & enable() const;

        ~context() = default;

        template <typename Fn, typename... Args>
        void execute(Fn fn, layout_mode , Args... args) const {
            cublas::throw_if_error(fn(handle(), args...));
        }

        static constexpr cublasOperation_t transpose(transpose_mode t) {
            switch (t) {
                case transpose_mode::none: return CUBLAS_OP_N;
                case transpose_mode::transpose: return CUBLAS_OP_T;
                case transpose_mode::conjugate_transpose: return CUBLAS_OP_C;
            }
            EMU_UNREACHABLE;
        }

        // Convert emu::uplo_mode to cublasFillMode_t
        static constexpr cublasFillMode_t uplo(uplo_mode u) {
            switch (u) {
                case uplo_mode::upper: return CUBLAS_FILL_MODE_UPPER;
                case uplo_mode::lower: return CUBLAS_FILL_MODE_LOWER;
            }
            EMU_UNREACHABLE;
        }

        // Convert emu::side_mode to cublasSideMode_t
        static constexpr cublasSideMode_t side(side_mode s) {
            switch (s) {
                case side_mode::left: return CUBLAS_SIDE_LEFT;
                case side_mode::right: return CUBLAS_SIDE_RIGHT;
            }
            EMU_UNREACHABLE;
        }

        // Convert emu::diag_mode to cublasDiagType_t
        static constexpr cublasDiagType_t diag(diag_mode d) {
            switch (d) {
                case diag_mode::unit: return CUBLAS_DIAG_UNIT;
                case diag_mode::non_unit: return CUBLAS_DIAG_NON_UNIT;
            }
            EMU_UNREACHABLE;
        }

        template<typename T>
        static constexpr const T* get_scalar(const value_or_pointer<T>& v) {
            return v.pointer();
        }

    private:
        scoped_handle handle_;
        ::cuda::device::id_t device_id_;
    };

    /**
     * Create cublas handle_t on current device.
     */
    context make_context();

    /**
     * Create cublas handle_t on current device.
     */
    context make_context(const ::cuda::device_t& device);

    context wrap_context(handle_t handle, const ::cuda::device_t& device, bool take_ownership);

} // namespace emu::linealg::cublas
