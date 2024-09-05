#pragma once

#include <emu/macro.cuh>
#include <emu/math.hpp>

#include <cstdint>

namespace emu
{

namespace atomic
{

namespace detail
{

    template<std::size_t S>
    struct matching_word_impl;

#define EMU_MATCH_SIZE(TYPE, SIZE)                               \
    template<> struct matching_word_impl<SIZE> { using type = TYPE; }

    EMU_MATCH_SIZE(std::uint8_t,  1);
    EMU_MATCH_SIZE(std::uint16_t, 2);
    EMU_MATCH_SIZE(std::uint32_t, 4);
    EMU_MATCH_SIZE(std::uint64_t, 8);

#undef EMU_MATCH_SIZE

    template<typename T>
    using matching_word = typename matching_word_impl<sizeof(T)>::type;


    template<auto Op, typename T>
    EMU_DEVICE T atomic_op(T * address, T val)
    {
        using word_t = matching_word<T>;

        word_t* address_as_word = reinterpret_cast<word_t*>(address);
        word_t old = *address_as_word, assumed;

        do {
            assumed = old;
            auto res = Op(*reinterpret_cast<T*>(&assumed), val);
            word_t new_ = *reinterpret_cast<word_t*>(&res);
            old = atomicCAS(address_as_word, assumed, new_);

        // Note: uses integer comparison to avoid hang in case of NaN (since NaN != NaN)
        } while (assumed != old);

        return *reinterpret_cast<T*>(&old);
    }

    // template<typename Op> struct AtomicDispatch;

} // namespace detail

// #define EMU_ADD_ATOMIC_FUNCTION(FUN_NAME, OP_TYPE, CUDA_OP)                                                 \
// namespace detail {                                                                                          \
//     EMU_GENERATE_TRAITS_HAS(HasAtomic##FUN_NAME, T, CUDA_OP(std::declval<T*>(), std::declval<T>()));        \
//     template<>                                                                                              \
//     struct AtomicDispatch<OP_TYPE> {                                                                        \
//         template<typename T>                                                                                \
//         static EMU_DEVICE EnableIf<    HasAtomic##FUN_NAME<T>, T> call(T * addr, T val) noexcept {          \
//             return CUDA_OP(addr, val);                                                                      \
//         }                                                                                                   \
//         template<typename T>                                                                                \
//         static EMU_DEVICE EnableIf<not HasAtomic##FUN_NAME<T>, T> call(T * addr, T val) noexcept {          \
//             return atomic_op(addr, val, OP_TYPE{});                                                         \
//         }                                                                                                   \
//     };                                                                                                      \
// }                                                                                                           \
// template<typename T>                                                                                        \
// EMU_DEVICE T FUN_NAME(T * addr, T val) noexcept { detail::AtomicDispatch<OP_TYPE>::call(addr, val); }

#define EMU_ADD_ATOMIC_FUNCTION(FUN_NAME, MATH_OP, CUDA_OP)                \
template<typename T>                                                       \
EMU_DEVICE T FUN_NAME(T * addr, T val) noexcept {                          \
    if constexpr (requires { {CUDA_OP(addr, val)} -> std::same_as<T>; }) { \
        return CUDA_OP(addr, val);                                         \
    } else {                                                               \
        return detail::atomic_op<MATH_OP>(addr, val);                      \
    }                                                                      \
}

EMU_ADD_ATOMIC_FUNCTION(add , math::add , atomicAdd);
EMU_ADD_ATOMIC_FUNCTION(sub , math::sub , atomicSub);
EMU_ADD_ATOMIC_FUNCTION(mul , math::mul , atomicMul);
EMU_ADD_ATOMIC_FUNCTION(div , math::div , atomicDiv);
EMU_ADD_ATOMIC_FUNCTION(min , math::min , atomicMin);
EMU_ADD_ATOMIC_FUNCTION(max , math::max , atomicMax);
EMU_ADD_ATOMIC_FUNCTION(bor , math::bor , atomicOr );
EMU_ADD_ATOMIC_FUNCTION(band, math::band, atomicAnd);
EMU_ADD_ATOMIC_FUNCTION(bxor, math::bxor, atomicXor);

#undef EMU_ADD_ATOMIC_FUNCTION;

} // namespace atomic

} // namespace emu
