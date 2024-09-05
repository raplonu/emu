#pragma once

#include <emu/detail/dlpack_types.hpp>
#include <emu/detail/mdspan_types.hpp>
#include <emu/numeric_type.hpp>

#include <emu/error.hpp>
#include <emu/container.hpp>
#include <emu/capsule.hpp>
#include <emu/container.hpp>
#include <emu/location_policy.hpp>
// #include <emu/concepts.hpp>

#include <cstdint>
#include <span>
#include <climits>
#include <dlpack/dlpack.h>
#include <type_traits>
// #include <memory>

// Note: dlpack is a specification for sharing tensor data between frameworks.
// More info can be found at https://dmlc.github.io/dlpack/latest/
// For now, dlpack only allow read only data sharing. This is too limitation for our use case.
// More info about this issue can be found at https://dmlc.github.io/dlpack/latest/#roadmap

namespace emu
{

namespace dlpack
{
    constexpr bool use_byte_offset = false;

    // constexpr uint32_t major = 0;
    // constexpr uint32_t minor = 8;

    // constexpr inline version_t get_version() noexcept {
    //     return {major, minor};
    // }

    // constexpr bool version_compatible(const version_t& version) noexcept {
    //     return version.major == major;
    // }

namespace detail
{

    struct manager
    {
        capsule cap;
        capsule extents;
        capsule strides;
    };

    void managed_tensor_deleter(DLManagedTensor* dlmt) {
        auto* manager_ptr = static_cast<manager*>(dlmt->manager_ctx);
        // always check if manager_ptr is nullptr
        if (manager_ptr) delete manager_ptr;
        delete dlmt;
    }

    // void managed_tensor_versioned_deleter(DLManagedTensorVersioned* dlmtv) {
    //     auto* manager_ptr = static_cast<manager*>(dlmtv->manager_ctx);
    //     // always check if manager_ptr is nullptr
    //     if (manager_ptr) delete manager_ptr;
    //     delete dlmtv;
    // }

    constexpr std::span<byte> memory_region(byte* base_ptr, size_t element_size, std::span<const int64_t> shape, std::span<const int64_t> strides = {}) {
        int64_t lowest_offset = 0;
        int64_t highest_offset = 0;

        int64_t current_offset = 0;
        for (int64_t i = 0; i < shape.size(); ++i) {
            int64_t stride = strides.empty() ? 1 : strides[i];
            int64_t max_offset = (shape[i] - 1) * stride;
            current_offset += max_offset;

            lowest_offset = std::min(lowest_offset, current_offset);
            highest_offset = std::max(highest_offset, current_offset);
        }

        int64_t region_size = (highest_offset - lowest_offset + 1) * element_size;
        byte* region_start = base_ptr + (lowest_offset * element_size);

        return std::span<byte>(region_start, region_size);
    }

    template<typename Mapping>
    constexpr auto create_mapping_from_dltensor(const tensor_t& tensor) -> result< Mapping > {
        using extents_type = typename Mapping::extents_type;
        using layout_type = typename Mapping::layout_type;

        constexpr auto rank = Mapping::extents_type::rank();
        using array_t = std::array<size_t, rank>;


        EMU_TRUE_OR_RETURN_UN_EC(tensor.ndim == rank, error::dlpack_rank_mismatch);

        array_t extents; std::copy_n(tensor.shape, tensor.ndim, extents.begin());

        if constexpr ( std::same_as<layout_type, layout_right>
                    or std::same_as<layout_type, layout_left> ) {
            return Mapping(extents);

        } else if constexpr (std::same_as<layout_type, layout_stride> ) {
            array_t strides;
            // dlpack documentation explicity mention that strides can be NULL
            if (tensor.strides != NULL)
                std::copy_n(tensor.strides, tensor.ndim, strides.begin());
            else{
                //Case where strides are not provided, assume row major.
                std::partial_sum(extents.rbegin(), extents.rend() - 1, strides.rbegin() + 1, std::multiplies<>());
                strides.back() = 1;
            }

            return Mapping(extents_type(extents), strides);

        } else
            static_assert(dependent_false<layout_type>, "Layout is not supported.");
    }

} // namespace detail

    struct scoped_tensor
    {
        capsule data_capsule;

        container<int64_t> extents_;
        container<int64_t> strides_;

        tensor_t tensor_;

        scoped_tensor() = default;

        scoped_tensor(
            byte* data, device_t device, data_type_t dtype, uint64_t byte_offset,
            capsule data_capsule, container<int64_t> extents, container<int64_t> strides
        )
            : data_capsule(data_capsule), extents_(extents), strides_(strides),
              tensor_{
                .data=data, .device=device, .ndim=static_cast<int32_t>(extents_.size()), .dtype=dtype,
                .shape=extents_.data(), .strides=strides_.data(),
                .byte_offset=byte_offset
            }
        {}

        scoped_tensor(tensor_t tensor)
            : data_capsule{}
            , extents_{tensor.shape, static_cast<size_t>(tensor.ndim)}
            , strides_{}
            , tensor_{tensor}
        {
            if (tensor.strides)
                strides_ = container<int64_t>{tensor.strides, static_cast<size_t>(tensor.ndim)};
        }

        scoped_tensor(const scoped_tensor&) = default;
        scoped_tensor(scoped_tensor&&) = default;

        scoped_tensor& operator=(const scoped_tensor&) = default;
        scoped_tensor& operator=(scoped_tensor&&) = default;

        ~scoped_tensor() = default;

        constexpr byte* get() const noexcept {
            return static_cast<byte*>(tensor_.data) + (use_byte_offset ? tensor_.byte_offset : 0);
        }
        //Note: for now, we will not provide the following methods

        constexpr size_t element_size() const noexcept {
            return (tensor_.dtype.bits * tensor_.dtype.lanes) / CHAR_BIT;
        }

        constexpr std::span<byte> region() const noexcept {
            return detail::memory_region(get(), element_size(), extents(), strides());
        }
        constexpr size_t region_size() const {
            // Not rely less efficient to just compute the size of the region
            return region().size();
        }

        constexpr device_t device() const noexcept { return tensor_.device; }

        constexpr int32_t rank() const noexcept { return tensor_.ndim; }
        constexpr int32_t ndim() const noexcept { return tensor_.ndim; }

        constexpr data_type_t dtype() const noexcept {
            return tensor_.dtype;
        }

        constexpr std::span<int64_t> extents() const noexcept {
            return extents_;
        }

        constexpr bool has_strides() const noexcept {
            return not strides_.empty();
        }
        constexpr std::span<int64_t> strides() const noexcept {
            return strides_;
        }
        constexpr uint64_t byte_offset() const noexcept {
            return tensor_.byte_offset;
        }

        constexpr tensor_t tensor() const noexcept {
            return tensor_;
        }

        managed_tensor_t managed_tensor() const {
            detail::manager* manager_ptr = new detail::manager{
                .cap = data_capsule,
                .extents = extents_.capsule(),
                .strides = strides_.capsule()
            };

            return {
                .dl_tensor = tensor_,
                .manager_ctx = static_cast<void*>(manager_ptr),
                .deleter = detail::managed_tensor_deleter
            };
        }

        //Not available in 0.8
        // managed_tensor_versioned_t managed_tensor_versioned() const {
        //     detail::manager* manager_ptr = new detail::manager{
        //         .cap = data_capsule,
        //         .extents = extents.capsule(),
        //         .strides = strides.capsule()
        //     };

        //     return {
        //         .version = get_version(),
        //         .manager_ctx = static_cast<void*>(manager_ptr),
        //         .deleter = detail::managed_tensor_versioned_deleter
        //         .flags = 0,
        //         .dl_tensor = tensor,
        //     };
        // }
    };

} // namespace dlpack

namespace spe
{


    template<typename T>
    struct dlpack_adaptor;

    template<cpts::dlpack_supported_data_type T>
    struct dlpack_adaptor<T>
    {
        constexpr static auto dtype = dlpack::data_type<T>;

        static dlpack::scoped_tensor export_(const T& t) {
            using location_p = location_type_of<T>;

            auto b_ptr = const_cast<byte*>(reinterpret_cast<const byte*>(std::addressof(t)));

            return {
                b_ptr, location_p::device_of(b_ptr), dtype,
                /*byte_offset = */ 0, /*data_capsule = */ {},
                /* extents = */ {}, /* strides = */ {}
            };
        }

        static result<T&> import_(const dlpack::scoped_tensor& t) {
            using location_p = location_type_of<T>;

            auto ptr = reinterpret_cast<T*>(t.get());

            EMU_TRUE_OR_RETURN_UN_EC(t.rank() == 0, error::dlpack_rank_mismatch);
            EMU_TRUE_OR_RETURN_UN_EC(t.dtype() == dtype, error::dlpack_type_mismatch);

            EMU_TRUE_OR_RETURN_UN_EC(not t.has_strides(), error::dlpack_strides_not_supported);

            return *ptr;
        }

    };


    template<cpts::any_span SpanType>
        requires (not cpts::any_container<SpanType>)
    struct dlpack_adaptor<SpanType>
    {
        constexpr static auto dtype = dlpack::data_type<typename SpanType::value_type>;

        static dlpack::scoped_tensor export_(const SpanType& sp) {
            using location_p = location_type_of<SpanType>;

            auto b_ptr = reinterpret_cast<byte*>(sp.data());

            auto extents = make_container<int64_t>(1); extents[0] = sp.size();

            return {
                b_ptr, location_p::device_of(b_ptr), dtype,
                /*byte_offset = */ 0, /*data_capsule = */ {},
                /* extents = */ move(extents), /* strides = */ {}
            };
        }

        static result<SpanType> import_(const dlpack::scoped_tensor& t) {
            using location_p = location_type_of<SpanType>;

            auto ptr = reinterpret_cast<typename SpanType::value_type*>(t.get());

            EMU_TRUE_OR_RETURN_UN_EC(t.rank() == 1, error::dlpack_rank_mismatch);
            EMU_TRUE_OR_RETURN_UN_EC(t.dtype() == dtype, error::dlpack_type_mismatch);

            EMU_TRUE_OR_RETURN_UN_EC(not t.has_strides(), error::dlpack_strides_not_supported);

            return SpanType{ptr, static_cast<size_t>(t.extents()[0])};
        }
    };

    template<cpts::any_container ContainerType>
    struct dlpack_adaptor<ContainerType>
    {
        using base_adaptor = dlpack_adaptor<typename ContainerType::span_type>;

        static dlpack::scoped_tensor export_(const ContainerType& c) {
            auto result = base_adaptor::export_(c);

            result.data_capsule = c.capsule();

            return result;
        }

        static result<ContainerType> import_(const dlpack::scoped_tensor& t) {
            return base_adaptor::import_(t).map([&](auto&& span) {
                return ContainerType(span, t.data_capsule);
            });
        }
    };

    template<cpts::any_mdspan MdSpanType>
        requires (not cpts::any_mdcontainer<MdSpanType>)
    struct dlpack_adaptor<MdSpanType>
    {
        using mdpan_type = MdSpanType;
        using extents_type = typename mdpan_type::extents_type;
        using mapping_type = typename mdpan_type::mapping_type;
        using location_p = location_type_of<MdSpanType>;

        constexpr static auto dtype = dlpack::data_type<typename MdSpanType::value_type>;

        static dlpack::scoped_tensor export_(const MdSpanType& m) {

            auto b_ptr = reinterpret_cast<byte*>(m.data_handle());

            auto extents = make_container<int64_t>(m.rank());
            for (std::size_t i = 0; i < m.rank(); ++i) {
                extents[i] = m.extents().extent(i);
            }

            container<int64_t> strides;
            if constexpr (not MdSpanType::is_always_exhaustive()) {
                strides = make_container<int64_t>(m.rank());
                for (std::size_t i = 0; i < m.rank(); ++i) {
                    strides[i] = m.stride(i);
                }
            }

            return {
                b_ptr, location_p::device_of(b_ptr), dtype,
                /*byte_offset = */ 0, /*data_capsule = */ {},
                /* extents = */ move(extents), /* strides = */ move(strides)
            };
        }

        static result<mdpan_type> import_(const dlpack::scoped_tensor& t) {

            auto b_ptr = reinterpret_cast<typename mdpan_type::element_type*>(t.get());

            EMU_TRUE_OR_RETURN_UN_EC(t.rank() == extents_type::rank(), error::dlpack_rank_mismatch);
            EMU_TRUE_OR_RETURN_UN_EC(t.dtype() == dtype, error::dlpack_type_mismatch);

            if constexpr (not mdpan_type::is_always_exhaustive()) {
                EMU_TRUE_OR_RETURN_UN_EC(t.has_strides(), error::dlpack_strides_not_supported);
            }

            auto mappint = dlpack::detail::create_mapping_from_dltensor<mapping_type>(t.tensor());
            EMU_TRUE_OR_RETURN_ERROR(mappint);

            return mdpan_type(b_ptr, *mappint);
        }
    };

    template<cpts::any_mdcontainer MdContainerType>
    struct dlpack_adaptor<MdContainerType>
    {
        using base_adaptor = dlpack_adaptor<typename MdContainerType::mdspan_type>;

        static dlpack::scoped_tensor export_(const MdContainerType& m) {
            auto result = base_adaptor::export_(m);

            result.data_capsule = m.capsule();

            return result;
        }

        static result<MdContainerType> import_(const dlpack::scoped_tensor& t) {
            return base_adaptor::import_(t).map([&](auto&& mdspan) {
                return MdContainerType{mdspan, t.data_capsule};
            });
        }
    };

} // namespace spe

namespace dlpack
{

    template<typename T>
    scoped_tensor export_to_scoped_tensor(const T& t) {
        return spe::dlpack_adaptor<T>::export_(t);
    }

    template<typename T>
    auto import_from_scoped_tensor(const scoped_tensor& t) {
        return spe::dlpack_adaptor<T>::import_(t);
    }








//     void dlpack_unversioned_deleter(DLManagedTensor* dlmt) {

//     }

//     // template<cpts::mdspan MdSpan>
//     // DLManagedTensorVersioned dlpack(const MdSpan& m) {


//     //     return dlpack(m.data(), m.extents(), m.strides());
//     // }

//     template<cpts::mdspan MdSpan>
//     DLManagedTensor dlpack_unversioned(const MdSpan& m) {
//         return dlpack(m.data(), m.extents(), m.strides());
//     }

// namespace detail
// {

//     enum class dtype_code : uint8_t {
//         Int = 0, UInt = 1, Float = 2, Bfloat = 4, Complex = 5, Bool = 6
//     };

// } // namespace detail

// namespace spe
// {

//     template<typename T>
//     constexpr uint8_t type_code = [] { static_assert(false, "dlpack unsupported type"); return 0; }();

//     template<std::signed_integral T>
//     constexpr uint8_t type_code< T > = kDLInt;

//     template<std::unsigned_integral T>
//     constexpr uint8_t type_code< T > = kDLUInt;

//     template<std::floating_point T>
//     constexpr uint8_t type_code< T > = kDLFloat;

//     template<typename T>
//     constexpr uint8_t type_code< std::complex<T> > = kDLComplex;

// } // namespace spe

//     template<typename T>
//     DLDataType dl_data_type() {
//         return { spe::type_code<T>, sizeof(T) * 8, 1 };
//     }

//     template<cpts::mdspan MdSpan>
//     int64_t* allocate_shape(const MdSpan& mdspan)
//     {
//         int64_t* shape = new int64_t[mdspan.rank()];

//         for (std::size_t i = 0; i < mdspan.rank(); ++i) {
//             shape[i] = mdspan.extents().extent(i);
//         }

//         return shape;
//     }

//     template<cpts::mdspan MdSpan>
//     int64_t* allocate_stride(const MdSpan& mdspan)
//     {
//         if constexpr (m.is_always_exhaustive()) /* and */ if (m.is_exhaustive())
//             return nullptr;

//         int64_t* stride = new int64_t[mdspan.rank()];

//         for (std::size_t i = 0; i < mdspan.rank(); ++i) {
//             stride[i] = mdspan.stride(i);
//         }

//         return stride;
//     }

//     template<cpts::mdspan MdSpan>
//     DLTensor dl_tensor(MdSpan mdspan)
//     {
//         using location_type = location_type_of<MdSpan>;

//         void* data = static_cast<void*>( mdspan.data_handle() );

//         return {
//             .data = data
//             .device = location_type::dl_device_of(data)
//             .ndim = mdspan.rank()
//             .dtype = dl_data_type< typename MdSpan::element_type >()
//             .shape = allocate_shape(mdspan)
//             .strides = allocate_stride(mdspan)
//             .byte_offset = 0
//         };
//     }

//     template<cpts::mdcontainer MdContainer>
//     DLTensor dl_tensor(MdContainer mdcontainer)
//     {
//         using location_type = location_type_of<MdContainer>;


//     }

//     template<cpts::mdcontainer MdContainer>
//     DLManagedTensor dl_managed_unversioned(const MdContainer& mdcontainer)
//     {
//         using location_type = location_type_of<MdContainer>;

//         DLManagedTensor result;

//         auto [shape, stride, buffer] = shape_and_stride_of(mdspan);

//         ctx_manager ctx{.cap = mdcontainer.capsule(), .ss_buffer = move(buffer)};

//         return result;
//     }

} // namespace dlpack

} // namespace emu
