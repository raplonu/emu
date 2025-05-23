#pragma once

#include <concepts>
#include <emu/type_traits.hpp>
#include <emu/detail/mdspan_types.hpp>

namespace emu::linealg
{

    template<typename T>
    struct value_or_pointer
    {
        using type = T;

        T value_;
        const T* pointer_;
        value_or_pointer() : value_(0), pointer_(nullptr) {}
        value_or_pointer(T value) : value_(value), pointer_(nullptr) {}
        value_or_pointer(const T* pointer) : value_(0), pointer_(pointer) {}

        const T* pointer() const {
            return pointer_ ? pointer_ : &value_;
        }
    };

    template<typename T>
    value_or_pointer(T value) -> value_or_pointer<T>;

    template<typename T>
    value_or_pointer(const T* pointer) -> value_or_pointer<T>;

    template<typename T>
    using v_or_p = value_or_pointer<T>;

    // Side on which a matrix is applied (used in trsm, symm, hemm, etc.)
    enum class side_mode {
        left,
        right
    };

    // Which triangle of a matrix is used (used in syrk, trsm, trmm, etc.)
    enum class uplo_mode {
        upper,
        lower
    };

    // Whether the matrix is transposed (used in gemm, gemv, syrk, etc.)
    enum class transpose_mode {
        none,
        transpose,
        conjugate_transpose
    };

    // Whether the diagonal is assumed to be all-ones (used in trsm, trmm, etc.)
    enum class diag_mode {
        unit,
        non_unit
    };

    // Whether the matrix is stored in row-major or column-major order
    enum class layout_mode {
        row_major,
        column_major
    };

    template<typename Mapping>
    layout_mode get_layout_mode(const Mapping& mapping)
    {
        if      constexpr (std::same_as<typename Mapping::layout_type, emu::layout_left>)
            return layout_mode::column_major;
        else if constexpr (std::same_as<typename Mapping::layout_type, emu::layout_right>)
            return layout_mode::row_major;
        else if constexpr (std::same_as<typename Mapping::layout_type, emu::layout_stride>)
            //TODO: Check if it is correct
            return mapping.stride(0) > mapping.stride(1)
                ? layout_mode::row_major
                : layout_mode::column_major;
        else
            static_assert(emu::dependent_false<Mapping>, "Unsupported mapping type");
    }

} // namespace emu::linealg
