#pragma once

#include <dlpack/dlpack.h>

#include <type_traits> // std::underlying_type_t

namespace emu::dlpack
{
    //Not available in 0.8
    // using version_t = DLPackVersion;

    using device_type_t = DLDeviceType;

    using device_t = DLDevice;

    using data_type_code_t = DLDataTypeCode;

    using data_type_t = DLDataType;

    using tensor_t = DLTensor;

    using managed_tensor_t = DLManagedTensor;

    //Not available in 0.8
    // using managed_tensor_versioned_t = DLManagedTensorVersioned;

} // namespace emu::dlpack

constexpr bool operator==(emu::dlpack::data_type_t const& lhs, emu::dlpack::data_type_t const& rhs) noexcept {
    return lhs.code == rhs.code && lhs.bits == rhs.bits && lhs.lanes == rhs.lanes;
}
