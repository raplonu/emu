#pragma once

#include <cstdint>
#include <dlpack/dlpack.h>

#include <fmt/base.h>

#include <type_traits> // std::underlying_type_t
#include <string_view>

namespace emu::dlpack
{
    using version_t = DLPackVersion;

    using device_type_t = DLDeviceType;

    using device_t = DLDevice;

    using data_type_code_t = DLDataTypeCode;

    using data_type_t = DLDataType;

    /**
     * @brief Similar to data_type_t but allow much bigger data size. Better
     * suited for non numeric types.
     *
     *
     */
    struct data_type_ext_t
    {
        uint8_t code;

        uint64_t bits;

        uint16_t lanes;

        constexpr operator data_type_t() const {
            return {
                .code = code,
                .bits = static_cast<uint8_t>(bits),
                .lanes = lanes
            };
        }

        constexpr bool operator==(data_type_ext_t const& rhs) const noexcept {
            return code == rhs.code && bits == rhs.bits && lanes == rhs.lanes;
        }

    };

    inline data_type_ext_t from_data_type(data_type_t dt) {
        return {
            .code = dt.code,
            .bits = dt.bits,
            .lanes = dt.lanes
        };
    }

    inline data_type_t from_data_type_ext(data_type_ext_t dt) {
        return {
            .code = dt.code,
            .bits = static_cast<uint8_t>(dt.bits),
            .lanes = dt.lanes
        };
    }

    using tensor_t = DLTensor;

    using managed_tensor_t = DLManagedTensor;

    using managed_tensor_versioned_t = DLManagedTensorVersioned;

    using device_type_under_t = std::underlying_type_t<device_type_t>;
    using data_type_code_under_t = std::underlying_type_t<data_type_code_t>;

namespace dtype
{
    // All types listed here does not have to necessarily to make sense.
    // Most of them are fully supported by DLPack.
    // The exception are the types with bits >= 256.

    constexpr data_type_ext_t boolean = {kDLBool, 1, 1};

    constexpr data_type_ext_t int8   = {kDLInt, 8, 1};
    constexpr data_type_ext_t int16  = {kDLInt, 16, 1};
    constexpr data_type_ext_t int32  = {kDLInt, 32, 1};
    constexpr data_type_ext_t int64  = {kDLInt, 64, 1};
    constexpr data_type_ext_t int128 = {kDLInt, 128, 1};

    constexpr data_type_ext_t uint8   = {kDLUInt, 8, 1};
    constexpr data_type_ext_t uint16  = {kDLUInt, 16, 1};
    constexpr data_type_ext_t uint32  = {kDLUInt, 32, 1};
    constexpr data_type_ext_t uint64  = {kDLUInt, 64, 1};
    constexpr data_type_ext_t uint128 = {kDLUInt, 128, 1};

    constexpr data_type_ext_t float8   = {kDLFloat, 8, 1};
    constexpr data_type_ext_t float16  = {kDLFloat, 16, 1};
    constexpr data_type_ext_t float32  = {kDLFloat, 32, 1};
    constexpr data_type_ext_t float64  = {kDLFloat, 64, 1};
    constexpr data_type_ext_t float128 = {kDLFloat, 128, 1};

    constexpr data_type_ext_t bfloat16 = {kDLBfloat, 16, 1};

    constexpr data_type_ext_t complex32  = {kDLComplex, 32, 1};
    constexpr data_type_ext_t complex64  = {kDLComplex, 64, 1};
    constexpr data_type_ext_t complex128 = {kDLComplex, 128, 1};
    constexpr data_type_ext_t complex256 = {kDLComplex, 256, 1};

} // namespace dtype

} // namespace emu::dlpack

inline std::string_view format_as(DLDeviceType dt) {
    // this implementation need to be kept in sync with device_type_from_string in dlpack.hpp
    switch (dt) {
        case kDLCPU:         return "Cpu";
        case kDLCUDA:        return "Cuda";
        case kDLCUDAHost:    return "CudaHost";
        case kDLOpenCL:      return "OpenCL";
        case kDLVulkan:      return "Vulkan";
        case kDLMetal:       return "Metal";
        case kDLVPI:         return "Vpi";
        case kDLROCM:        return "Rocm";
        case kDLROCMHost:    return "RocmHost";
        case kDLExtDev:      return "ExtDev";
        case kDLCUDAManaged: return "CudaManaged";
        case kDLOneAPI:      return "OneApi";
        case kDLWebGPU:      return "WebGpu";
        case kDLHexagon:     return "Hexagon";
        case kDLMAIA:        return "Maia";
        default:             return "unknow";
    }
}

inline std::string_view format_as(DLDataTypeCode dtc) {
    // this implementation need to be kept in sync with data_type_code_from_string in dlpack.hpp
    switch (dtc) {
        case kDLInt:          return "int";
        case kDLUInt:         return "uint";
        case kDLFloat:        return "float";
        case kDLComplex:      return "complex";
        case kDLBfloat:       return "bfloat";
        case kDLOpaqueHandle: return "object";
        case kDLBool:         return "bool";
        default:              return "unknow";
    }
}


template<typename Char>
struct fmt::formatter<emu::dlpack::device_t, Char>
{
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
        return ctx.end();
    }

    auto format(const emu::dlpack::device_t& value, format_context& ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "device({}@{})", value.device_type, value.device_id);
    }

};

template<typename Char>
struct fmt::formatter<emu::dlpack::data_type_t, Char>
{
    constexpr auto parse(format_parse_context& ctx)
        -> format_parse_context::iterator
    {
        return ctx.end();
    }

    auto format(const emu::dlpack::data_type_t& value, format_context& ctx) const
        -> format_context::iterator
    {
        // ignore the lanes for now.
        ctx.advance_to(fmt::format_to(ctx.out(), "dtype('{}{}",
            static_cast<emu::dlpack::data_type_code_t>(value.code), value.bits
        ));

        if (value.lanes > 1) // when vectorized types.
            ctx.advance_to(fmt::format_to(ctx.out(), "[{}]", value.lanes));

        return fmt::format_to(ctx.out(), "')");
    }

};

template<typename Char>
struct fmt::formatter<emu::dlpack::data_type_ext_t, Char>
{
    constexpr auto parse(format_parse_context& ctx)
        -> format_parse_context::iterator
    {
        return ctx.end();
    }

    auto format(const emu::dlpack::data_type_ext_t& value, format_context& ctx) const
        -> format_context::iterator
    {
        // ignore the lanes for now.
        ctx.advance_to(fmt::format_to(ctx.out(), "dtype('{}{}",
            static_cast<emu::dlpack::data_type_code_t>(value.code), value.bits
        ));

        if (value.lanes > 1) // when vectorized types.
            ctx.advance_to(fmt::format_to(ctx.out(), "[{}]", value.lanes));

        return fmt::format_to(ctx.out(), "')");
    }

};

constexpr bool operator==(
    emu::dlpack::data_type_t const& lhs,
    emu::dlpack::data_type_t const& rhs) noexcept
{
    return lhs.code == rhs.code && lhs.bits == rhs.bits && lhs.lanes == rhs.lanes;
}
