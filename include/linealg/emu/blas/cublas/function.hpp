#pragma once

#include <cublas_v2.h>

namespace emu::linealg::cublas
{
    template<typename T> struct blas;

    template<>
    struct blas<__half> {

    };

    template<>
    struct blas<float> {
        // Level 1
        constexpr static auto amax = cublasIsamax;
        constexpr static auto amin = cublasIsamin;
        constexpr static auto asum = cublasSasum;
        constexpr static auto axpy = cublasSaxpy;
        constexpr static auto scal = cublasSscal;

        // Level 2
        constexpr static auto gemv = cublasSgemv;

        // Level 3
        constexpr static auto gemm = cublasSgemm;
        constexpr static auto syrk = cublasSsyrk;
        constexpr static auto trsm = cublasStrsm;
        constexpr static auto getrf_batched = cublasSgetrfBatched;
        constexpr static auto getri_batched = cublasSgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasSdgmm;
    };

    template<>
    struct blas<double> {
        // Level 1
        constexpr static auto amax = cublasIdamax;
        constexpr static auto amin = cublasIdamin;
        constexpr static auto asum = cublasDasum;
        constexpr static auto axpy = cublasDaxpy;
        constexpr static auto scal = cublasDscal;

        //  Level 2
        constexpr static auto gemv = cublasDgemv;

        // Level 3
        constexpr static auto gemm = cublasDgemm;
        constexpr static auto syrk = cublasDsyrk;
        constexpr static auto trsm = cublasDtrsm;
        constexpr static auto getrf_batched = cublasDgetrfBatched;
        constexpr static auto getri_batched = cublasDgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasDdgmm;
    };

    template<>
    struct blas<cuComplex> {
        // Level 1
        constexpr static auto amax = cublasIcamax;
        constexpr static auto amin = cublasIcamin;
        constexpr static auto asum = cublasScasum;
        constexpr static auto axpy = cublasCaxpy;
        constexpr static auto scal = cublasCscal;

        // Level 2
        constexpr static auto gemv = cublasCgemv;

        // Level 3
        constexpr static auto gemm = cublasCgemm;
        constexpr static auto syrk = cublasCsyrk;
        constexpr static auto trsm = cublasCtrsm;
        constexpr static auto getrf_batched = cublasCgetrfBatched;
        constexpr static auto getri_batched = cublasCgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasCdgmm;
    };

    template<>
    struct blas<cuDoubleComplex> {
        // Level 1
        constexpr static auto amax = cublasIzamax;
        constexpr static auto amin = cublasIzamin;
        constexpr static auto asum = cublasDzasum;
        constexpr static auto axpy = cublasZaxpy;
        constexpr static auto scal = cublasZscal;

        // Level 2
        constexpr static auto gemv = cublasZgemv;

        // Level 3
        constexpr static auto gemm = cublasZgemm;
        constexpr static auto syrk = cublasZsyrk;
        constexpr static auto trsm = cublasZtrsm;
        constexpr static auto getrf_batched = cublasZgetrfBatched;
        constexpr static auto getri_batched = cublasZgetriBatched;

        // Extension
        constexpr static auto dgmm = cublasZdgmm;
    };

} // namespace emu::linealg::cublas
