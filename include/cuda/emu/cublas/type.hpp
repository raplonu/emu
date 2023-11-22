#pragma once

#include <emu/macro.hpp>
#include <emu/assert.hpp>

#include <cublas_v2.h>

namespace emu
{

namespace cublas
{
    namespace detail
    {
        template<typename T> struct AssociatedType;

        template<> struct AssociatedType<float>           { using type = float;  };
        template<> struct AssociatedType<double>          { using type = double; };
        template<> struct AssociatedType<cuComplex>       { using type = float;  };
        template<> struct AssociatedType<cuDoubleComplex> { using type = double; };

    } // namespace detail

    template<typename T>
    using AssociatedT = typename detail::AssociatedType<T>::type;

    enum class Operation {
        N,
        T,
        C,
        Hermitan,
        Conjg
    };

    enum class SideMode {
        Left,
        Right
    };

    enum class FillMode {
        Lower,
        Upper,
        Full
    };

    enum class DiagonalType {
        NonUnit,
        Unit
    };

    constexpr inline cublasOperation_t convert(Operation op) noexcept {
        switch (op)
        {
        case Operation::N:
            return CUBLAS_OP_N;
        case Operation::T:
            return CUBLAS_OP_T;
        case Operation::C:
        case Operation::Hermitan:
            return CUBLAS_OP_HERMITAN;
        case Operation::Conjg:
            return CUBLAS_OP_CONJG;
        }
        EMU_UNREACHABLE;
    }

    constexpr inline cublasSideMode_t convert(SideMode sm) noexcept {
        switch (sm)
        {
        case SideMode::Left:
            return CUBLAS_SIDE_LEFT;
        case SideMode::Right:
            return CUBLAS_SIDE_RIGHT;
        }
        EMU_UNREACHABLE;
    }

    constexpr inline cublasFillMode_t convert(FillMode fm) noexcept {
        switch (fm)
        {
        case FillMode::Lower:
            return CUBLAS_FILL_MODE_LOWER;
        case FillMode::Upper:
            return CUBLAS_FILL_MODE_UPPER;
        case FillMode::Full:
            return CUBLAS_FILL_MODE_FULL;
        }
        EMU_UNREACHABLE;
    }

    constexpr inline cublasDiagType_t convert(DiagonalType dt) noexcept {
        switch (dt)
        {
        case DiagonalType::NonUnit:
            return CUBLAS_DIAG_NON_UNIT;
        case DiagonalType::Unit:
            return CUBLAS_DIAG_UNIT;
        }
        EMU_UNREACHABLE;
    }

} // namespace cublas

} // namespace emu
