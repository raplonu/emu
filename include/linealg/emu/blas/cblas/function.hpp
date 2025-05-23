#pragma once

#include <cblas.h>
#include <complex>

namespace emu::linealg::cblas
{
    template<typename T> struct blas;

    template<>
    struct blas<float> {
        // Level 1
        constexpr static auto amax = cblas_isamax;
        constexpr static auto amin = cblas_isamin;
        constexpr static auto asum = cblas_sasum;
        constexpr static auto axpy = cblas_saxpy;
        constexpr static auto scal = cblas_sscal;

        // Level 2
        constexpr static auto gemv = cblas_sgemv;

        // Level 3
        constexpr static auto gemm = cblas_sgemm;
        constexpr static auto syrk = cblas_ssyrk;
        constexpr static auto trsm = cblas_strsm;

        // No getrf_batched / getri_batched / dgmm in standard CBLAS
    };

    template<>
    struct blas<double> {
        // Level 1
        constexpr static auto amax = cblas_idamax;
        constexpr static auto amin = cblas_idamin;
        constexpr static auto asum = cblas_dasum;
        constexpr static auto axpy = cblas_daxpy;
        constexpr static auto scal = cblas_dscal;

        // Level 2
        constexpr static auto gemv = cblas_dgemv;

        // Level 3
        constexpr static auto gemm = cblas_dgemm;
        constexpr static auto syrk = cblas_dsyrk;
        constexpr static auto trsm = cblas_dtrsm;
    };

    template<>
    struct blas<std::complex<float>> {
        // Level 1
        constexpr static auto amax = cblas_icamax;
        constexpr static auto amin = cblas_icamin;
        constexpr static auto asum = cblas_scasum;
        constexpr static auto axpy = cblas_caxpy;
        constexpr static auto scal = cblas_cscal;

        // Level 2
        constexpr static auto gemv = cblas_cgemv;

        // Level 3
        constexpr static auto gemm = cblas_cgemm;
        constexpr static auto syrk = cblas_csyrk;
        constexpr static auto trsm = cblas_ctrsm;
    };

    template<>
    struct blas<std::complex<double>> {
        // Level 1
        constexpr static auto amax = cblas_izamax;
        constexpr static auto amin = cblas_izamin;
        constexpr static auto asum = cblas_dzasum;
        constexpr static auto axpy = cblas_zaxpy;
        constexpr static auto scal = cblas_zscal;

        // Level 2
        constexpr static auto gemv = cblas_zgemv;

        // Level 3
        constexpr static auto gemm = cblas_zgemm;
        constexpr static auto syrk = cblas_zsyrk;
        constexpr static auto trsm = cblas_ztrsm;
    };


} // namespace emu::linealg::cblas
