#pragma once

#include <emu/concepts.hpp>
#include <emu/capsule.hpp>

#include <dlpack/dlpack.h>

#error "This header is total garbage and may be removed in the future. Do not rely on it."

// Note: dlpack is a specification for sharing tensor data between frameworks.
// More info can be found at https://dmlc.github.io/dlpack/latest/
// For now, dlpack only allow read only data sharing. This is too limitation for our use case.
// More info about this issue can be found at https://dmlc.github.io/dlpack/latest/#roadmap

namespace emu
{

    void dlpack_unversioned_deleter(DLManagedTensor* dlmt) {

    }

    // template<cpts::mdspan MdSpan>
    // DLManagedTensorVersioned dlpack(const MdSpan& m) {


    //     return dlpack(m.data(), m.extents(), m.strides());
    // }

    template<cpts::mdspan MdSpan>
    DLManagedTensor dlpack_unversioned(const MdSpan& m) {
        return dlpack(m.data(), m.extents(), m.strides());
    }

namespace detail
{

    enum class dtype_code : uint8_t {
        Int = 0, UInt = 1, Float = 2, Bfloat = 4, Complex = 5, Bool = 6
    };

} // namespace detail

namespace spe
{

    template<typename T>
    constexpr uint8_t type_code = [] { static_assert(false, "dlpack unsupported type"); return 0; }();

    template<std::signed_integral T>
    constexpr uint8_t type_code< T > = kDLInt;

    template<std::unsigned_integral T>
    constexpr uint8_t type_code< T > = kDLUInt;

    template<std::floating_point T>
    constexpr uint8_t type_code< T > = kDLFloat;

    template<typename T>
    constexpr uint8_t type_code< std::complex<T> > = kDLComplex;

} // namespace spe

    template<typename T>
    DLDataType dl_data_type() {
        return { spe::type_code<T>, sizeof(T) * 8, 1 };
    }

    template<cpts::mdspan MdSpan>
    int64_t* allocate_shape(const MdSpan& mdspan)
    {
        int64_t* shape = new int64_t[mdspan.rank()];

        for (std::size_t i = 0; i < mdspan.rank(); ++i) {
            shape[i] = mdspan.extents().extent(i);
        }

        return shape;
    }

    template<cpts::mdspan MdSpan>
    int64_t* allocate_stride(const MdSpan& mdspan)
    {
        if constexpr (m.is_always_exhaustive()) /* and */ if (m.is_exhaustive())
            return nullptr;

        int64_t* stride = new int64_t[mdspan.rank()];

        for (std::size_t i = 0; i < mdspan.rank(); ++i) {
            stride[i] = mdspan.stride(i);
        }

        return stride;
    }

    template<cpts::mdspan MdSpan>
    DLTensor dl_tensor(MdSpan mdspan)
    {
        using location_type = location_type_of<MdSpan>;

        void* data = static_cast<void*>( mdspan.data_handle() );

        return {
            .data = data
            .device = location_type::dl_device_of(data)
            .ndim = mdspan.rank()
            .dtype = dl_data_type< typename MdSpan::element_type >()
            .shape = allocate_shape(mdspan)
            .strides = allocate_stride(mdspan)
            .byte_offset = 0
        };
    }

    template<cpts::mdcontainer MdContainer>
    DLTensor dl_tensor(MdContainer mdcontainer)
    {
        using location_type = location_type_of<MdContainer>;


    }

    template<cpts::mdcontainer MdContainer>
    DLManagedTensor dl_managed_unversioned(const MdContainer& mdcontainer)
    {
        using location_type = location_type_of<MdContainer>;

        DLManagedTensor result;

        auto [shape, stride, buffer] = shape_and_stride_of(mdspan);

        ctx_manager ctx{.cap = mdcontainer.capsule(), .ss_buffer = move(buffer)};

        return result;
    }

} // namespace emu
