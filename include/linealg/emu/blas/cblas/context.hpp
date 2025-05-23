#pragma once

#include <emu/blas/blas_type.hpp>
#include <emu/blas/cblas/function.hpp>

#include <emu/assert.hpp>

#include <cblas.h>

namespace emu::linealg::cblas
{

    struct context
    {
        template<typename T>
        using blas_type = blas<T>;

        template <typename Fn, typename... Args>
        void execute(Fn fn, layout_mode l, Args... args) const {
            // No error handling in CBLAS

            // Test if the function takes a layout argument
            if constexpr (requires { fn(CblasRowMajor, args...); })
                // Call the function with the layout argument
                fn(layout(l), args...);
            else
                // Call the function without the layout argument
                fn(args...);
        }

        // Convert emu::transpose_mode to CBLAS_TRANSPOSE
        static constexpr CBLAS_TRANSPOSE transpose(transpose_mode t) {
            switch (t) {
                case transpose_mode::none: return CblasNoTrans;
                case transpose_mode::transpose: return CblasTrans;
                case transpose_mode::conjugate_transpose: return CblasConjTrans;
            }
            EMU_UNREACHABLE;
        }

        // Convert emu::uplo_mode to CBLAS_UPLO
        static constexpr CBLAS_UPLO uplo(uplo_mode u) {
            switch (u) {
                case uplo_mode::upper: return CblasUpper;
                case uplo_mode::lower: return CblasLower;
            }
            EMU_UNREACHABLE;
        }

        // Convert emu::side_mode to CBLAS_SIDE
        static constexpr CBLAS_SIDE side(side_mode s) {
            switch (s) {
                case side_mode::left: return CblasLeft;
                case side_mode::right: return CblasRight;
            }
            EMU_UNREACHABLE;
        }

        // Convert emu::diag_mode to CBLAS_DIAG
        static constexpr CBLAS_DIAG diag(diag_mode d) {
            switch (d) {
                case diag_mode::unit: return CblasUnit;
                case diag_mode::non_unit: return CblasNonUnit;
            }
            EMU_UNREACHABLE;
        }

        // CBLAS-specific: layout (row or column major)
        static constexpr CBLAS_LAYOUT layout(layout_mode l) {
            switch (l) {
                case layout_mode::row_major: return CblasRowMajor;
                case layout_mode::column_major: return CblasColMajor;
            }
            EMU_UNREACHABLE;
        }

        template<typename T>
        static constexpr T get_scalar(const value_or_pointer<T>& v) {
            return *(v.pointer());
        }

    };

} // namespace emu::linealg::cblas
