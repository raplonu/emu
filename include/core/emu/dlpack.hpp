#pragma once

#include <emu/detail/dlpack_types.hpp>
#include <emu/detail/mdspan_types.hpp>
#include <emu/numeric_type.hpp>

#include <emu/concepts.hpp>
#include <emu/utility.hpp>
#include <emu/error.hpp>
#include <emu/container.hpp>
#include <emu/capsule.hpp>
#include <emu/scoped.hpp>
#include <emu/container.hpp>
#include <emu/location_policy.hpp>
#include <emu/cstring_view.hpp>

#include <range/v3/range/conversion.hpp>

#include <cstdint>
#include <span>
#include <climits>
#include <dlpack/dlpack.h>
#include <type_traits>
#include <utility>
// #include <memory>

// Note: dlpack is a specification for sharing tensor data between frameworks.
// More info can be found at https://dmlc.github.io/dlpack/latest/

namespace emu
{

namespace spe
{

    template<typename T>
    struct dlpack_adaptor;

} // namespace spe

namespace dlpack
{
    constexpr bool use_byte_offset = false;

    constexpr inline version_t get_version() noexcept {
        return {DLPACK_MAJOR_VERSION, DLPACK_MINOR_VERSION};
    }

    [[nodiscard]] constexpr inline bool flag_read_only(uint64_t flags) noexcept {
        return (flags & DLPACK_FLAG_BITMASK_READ_ONLY) != 0;
    }

    [[nodiscard]] constexpr inline bool flag_is_copied(uint64_t flags) noexcept {
        return (flags & DLPACK_FLAG_BITMASK_IS_COPIED) != 0;
    }

    inline result<device_type_t> device_type_from_string(std::string_view sv)
    {
        // this implementation need to be kept in sync with format_as(DLDeviceType) in dlpack_types.hpp

        if (sv == "Cpu") return kDLCPU;
        if (sv == "Cuda") return kDLCUDA;
        if (sv == "CudaHost") return kDLCUDAHost;
        if (sv == "OpenCL") return kDLOpenCL;
        if (sv == "Vulkan") return kDLVulkan;
        if (sv == "Metal") return kDLMetal;
        if (sv == "Vpi") return kDLVPI;
        if (sv == "Rocm") return kDLROCM;
        if (sv == "RocmHost") return kDLROCMHost;
        if (sv == "ExtDev") return kDLExtDev;
        if (sv == "CudaManaged") return kDLCUDAManaged;
        if (sv == "OneApi") return kDLOneAPI;
        if (sv == "WebGpu") return kDLWebGPU;
        if (sv == "Hexagon") return kDLHexagon;
        if (sv == "Maia") return kDLMAIA;

        return make_unexpected(errc::dlpack_unkown_device_type);
    }

    inline result<data_type_code_t> data_type_code_from_string(std::string_view sv)
    {
        // this implementation need to be kept in sync with format_as(DLDataTypeCode) in dlpack_types.hpp

        if(sv == "int") return kDLInt;
        if(sv == "uint") return kDLUInt;
        if(sv == "float") return kDLFloat;
        if(sv == "object") return kDLOpaqueHandle;
        if(sv == "bfloat") return kDLBfloat;
        if(sv == "complex") return kDLComplex;
        if(sv == "bool") return kDLBool;

        return make_unexpected(errc::dlpack_unkown_data_type_code);
    }


namespace detail
{

    struct manager
    {
        capsule cap;
        capsule extents;
        capsule strides;
    };

    void managed_tensor_deleter(DLManagedTensor* dlmt);

    void managed_tensor_versioned_deleter(DLManagedTensorVersioned* dlmtv);

    constexpr inline auto managed_tensor_destructor = [](auto& managed_tensor) {
        managed_tensor.deleter(&managed_tensor);
    };

    using scoped_managed_tensor = scoped<managed_tensor_t, decltype(managed_tensor_destructor)>;
    using scoped_managed_tensor_versioned = scoped<managed_tensor_versioned_t, decltype(managed_tensor_destructor)>;

    constexpr std::span<byte> memory_region(byte* base_ptr, size_t element_size, std::span<const int64_t> shape, std::span<const int64_t> strides = {}) {
        int64_t lowest_offset = 0;
        int64_t highest_offset = 0;

        int64_t current_offset = 0;
        for (size_t i = 0; i < shape.size(); ++i) {
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

        EMU_TRUE_OR_RETURN_UN_EC(tensor.ndim == rank, errc::dlpack_rank_mismatch);

        array_t extents; std::copy_n(tensor.shape, tensor.ndim, extents.begin());

        if constexpr ( std::same_as<layout_type, layout_right>
                    or std::same_as<layout_type, layout_left> ) {
            return Mapping(extents);

        } else if constexpr (std::same_as<layout_type, layout_stride> ) {
            array_t strides;
            // dlpack documentation explicity mention that strides can be NULL.
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

    private:
        capsule data_capsule_;

        container<int64_t> extents_;
        container<int64_t> strides_;

        tensor_t tensor_ = {};

        uint64_t flags = 0;

    public:
        template<typename T>
        friend struct spe::dlpack_adaptor;

        scoped_tensor() = default;

        scoped_tensor(
            byte* data, device_t device, data_type_t dtype, uint64_t byte_offset,
            capsule data_capsule, container<int64_t> extents, container<int64_t> strides,
            uint64_t flags = 0
        )
            : data_capsule_(std::move(data_capsule)), extents_(std::move(extents)), strides_(std::move(strides)),
              tensor_{
                .data=data, .device=device, .ndim=static_cast<int32_t>(extents_.size()), .dtype=dtype,
                .shape=extents_.data(), .strides=strides_.data(),
                .byte_offset=byte_offset
            },
            flags(flags)
        {}

        scoped_tensor(
            const byte* data, device_t device, data_type_t dtype, uint64_t byte_offset,
            capsule data_capsule, container<int64_t> extents, container<int64_t> strides
        )
            : data_capsule_(std::move(data_capsule)), extents_(std::move(extents)), strides_(std::move(strides)),
              tensor_{
                // we const cast but this information is not
                // loss. It is represented by the flag `DLPACK_FLAG_BITMASK_READ_ONLY`
                .data=const_cast<byte*>(data),
                .device=device, .ndim=static_cast<int32_t>(extents_.size()), .dtype=dtype,
                .shape=extents_.data(), .strides=strides_.data(),
                .byte_offset=byte_offset
            },
            flags(DLPACK_FLAG_BITMASK_READ_ONLY)
        {}

        scoped_tensor(capsule data_capsule, container<int64_t> extents, container<int64_t> strides,
                      tensor_t tensor, uint64_t flags)
            : data_capsule_(std::move(data_capsule))
            , extents_(std::move(extents))
            , strides_(std::move(strides))
            , tensor_(tensor), flags(flags)
        {}

        // scoped_tensor(tensor_t tensor)
        //     : extents_{tensor.shape, static_cast<size_t>(tensor.ndim)}
        //     , strides_{}
        //     , tensor_{tensor}
        // {
        //     if (tensor.strides)
        //         strides_ = container<int64_t>{tensor.strides, static_cast<size_t>(tensor.ndim)};
        // }

        scoped_tensor(const scoped_tensor&) = default;
        scoped_tensor(scoped_tensor&&) = default;

        scoped_tensor& operator=(const scoped_tensor&) = default;
        scoped_tensor& operator=(scoped_tensor&&) = default;

        ~scoped_tensor() = default;

        [[nodiscard]] constexpr byte* get() const noexcept {
            return static_cast<byte*>(tensor_.data) + (use_byte_offset ? tensor_.byte_offset : 0);
        }

        [[nodiscard]] constexpr capsule data_capsule() const & noexcept {
            return data_capsule_;
        }

        [[nodiscard]] constexpr capsule data_capsule() && noexcept {
            return std::move(data_capsule_);
        }

        [[nodiscard]] constexpr size_t element_size() const noexcept {
            return (tensor_.dtype.bits * tensor_.dtype.lanes) / CHAR_BIT;
        }

        [[nodiscard]] constexpr std::span<byte> region() const noexcept {
            return detail::memory_region(get(), element_size(), extents(), strides());
        }
        [[nodiscard]] constexpr size_t region_size() const {
            // Not rely less efficient to just compute the size of the region
            return region().size();
        }

        [[nodiscard]] constexpr device_t device() const noexcept { return tensor_.device; }

        [[nodiscard]] constexpr int32_t rank() const noexcept { return tensor_.ndim; }
        [[nodiscard]] constexpr int32_t ndim() const noexcept { return tensor_.ndim; }

        [[nodiscard]] constexpr data_type_t dtype() const noexcept {
            return tensor_.dtype;
        }

        [[nodiscard]] constexpr std::span<int64_t> extents() const noexcept {
            return extents_;
        }

        [[nodiscard]] constexpr bool has_strides() const noexcept {
            return not strides_.empty();
        }
        [[nodiscard]] constexpr std::span<int64_t> strides() const noexcept {
            return strides_;
        }
        [[nodiscard]] constexpr uint64_t byte_offset() const noexcept {
            return tensor_.byte_offset;
        }

        [[nodiscard]] constexpr tensor_t tensor() const noexcept {
            return tensor_;
        }

        [[nodiscard]] constexpr bool read_only() const noexcept {
            return flag_read_only(flags);
        }

        [[nodiscard]] constexpr bool is_copied() const noexcept {
            return flag_is_copied(flags);
        }

        [[nodiscard]] detail::scoped_managed_tensor managed_tensor() const {
            auto manager_ptr = std::make_unique<detail::manager>(
                data_capsule(), extents_.capsule(), strides_.capsule()
            );

            return managed_tensor_t{
                .dl_tensor = tensor_,
                .manager_ctx = static_cast<void*>(manager_ptr.release()),
                .deleter = detail::managed_tensor_deleter
            };
        }

        [[nodiscard]] detail::scoped_managed_tensor_versioned managed_tensor_versioned() const {
            auto manager_ptr = std::make_unique<detail::manager>(
                data_capsule(), extents_.capsule(), strides_.capsule()
            );

            return managed_tensor_versioned_t{
                .version = get_version(),
                .manager_ctx = static_cast<void*>(manager_ptr.release()),
                .deleter = detail::managed_tensor_versioned_deleter,
                .flags = flags,
                .dl_tensor = tensor_
            };
        }

        [[nodiscard]] static scoped_tensor from_managed_tensor(const managed_tensor_versioned_t& managed, bool borrow = false) {
            //TODO: assert version is compatible.
            const auto& tensor = managed.dl_tensor;

            auto extents = make_container<int64_t>(tensor.ndim);
            std::copy_n(tensor.shape, tensor.ndim, extents.data());

            container<int64_t> strides;
            if (tensor.strides != NULL) {
                strides = make_container<int64_t>(tensor.ndim);
                std::copy_n(tensor.strides, tensor.ndim, strides.data());
            }

            capsule data_capsule;
            if (borrow) { // Now scoped_tensor owns managed as a reference counted resource.
                data_capsule = capsule(scoped(managed, [](auto& managed){ managed.deleter(&managed); }));
            }

            return scoped_tensor{
                /* data_capsule = */ std::move(data_capsule),
                /* extents = */ std::move(extents),
                /* strides = */ std::move(strides),
                /* tensor = */ tensor,
                /* flags = */ managed.flags
            };
        }

    };

} // namespace dlpack

namespace spe
{
    template<typename T>
    struct dlpack_adaptor
    {
        constexpr static auto dtype = dlpack::data_type<T>;

        constexpr static bool keep_capsule = false;

        template<typename TT>
        static dlpack::scoped_tensor export_(TT&& t) {
            using location_p = location_type_of<T>;

            auto *b_ptr = b_ptr_of(std::addressof(t));

            return {
                b_ptr, location_p::device_of(b_ptr), dtype,
                /*byte_offset = */ 0, /*data_capsule = */ {},
                /* extents = */ {}, /* strides = */ {},
                /* flags = */ (is_const<TT> ? DLPACK_FLAG_BITMASK_READ_ONLY : 0)
            };
        }

        static result<T&> import_(const dlpack::scoped_tensor& t) {

            EMU_TRUE_OR_RETURN_UN_EC(is_const<T> or not t.read_only(), errc::dlpack_read_only);

            EMU_TRUE_OR_RETURN_UN_EC(t.rank() == 0, errc::dlpack_rank_mismatch);
            EMU_TRUE_OR_RETURN_UN_EC(t.dtype() == dtype, errc::dlpack_type_mismatch);

            EMU_TRUE_OR_RETURN_UN_EC(not t.has_strides(), errc::dlpack_strides_not_supported);

            auto ptr = reinterpret_cast<T*>(t.get());

            return *ptr;
        }

    };


    template<cpts::any_span SpanType>
        requires (not cpts::any_container<SpanType>)
    struct dlpack_adaptor<SpanType>
    {
        constexpr static auto dtype = dlpack::data_type<typename SpanType::value_type>;

        constexpr static bool keep_capsule = true;

        static dlpack::scoped_tensor export_(const SpanType& sp) {
            using location_p = location_type_of<SpanType>;

            auto *b_ptr = b_ptr_of(sp);

            auto extents = make_container<int64_t>(1); extents[0] = sp.size();

            return {
                b_ptr, location_p::device_of(b_ptr), dtype,
                /*byte_offset = */ 0, /*data_capsule = */ {},
                /* extents = */ std::move(extents), /* strides = */ {},
                /* flags = */ (is_const<typename SpanType::element_type> ? DLPACK_FLAG_BITMASK_READ_ONLY : 0)
            };
        }

        static result<SpanType> import_(const dlpack::scoped_tensor& t) {

            EMU_TRUE_OR_RETURN_UN_EC(is_const<typename SpanType::element_type> or not t.read_only(), errc::dlpack_read_only);

            EMU_TRUE_OR_RETURN_UN_EC(t.rank() == 1, errc::dlpack_rank_mismatch);
            EMU_TRUE_OR_RETURN_UN_EC(t.dtype() == dtype, errc::dlpack_type_mismatch);

            EMU_TRUE_OR_RETURN_UN_EC(not t.has_strides(), errc::dlpack_strides_not_supported);

            auto ptr = reinterpret_cast<typename SpanType::element_type*>(t.get());

            return SpanType{ptr, static_cast<size_t>(t.extents()[0])};
        }
    };

    template<cpts::any_container ContainerType>
    struct dlpack_adaptor<ContainerType>
    {
        using base_adaptor = dlpack_adaptor<typename ContainerType::span_type>;

        constexpr static bool keep_capsule = false;

        static dlpack::scoped_tensor export_(const ContainerType& c) {
            auto result = base_adaptor::export_(c);

            result.data_capsule_ = c.capsule();

            return result;
        }

        static dlpack::scoped_tensor export_(ContainerType&& c) {
            auto result = base_adaptor::export_(c);

            result.data_capsule_ = std::move(c).capsule();
            result.flags |= DLPACK_FLAG_BITMASK_IS_COPIED;

            return result;
        }

        static result<ContainerType> import_(const dlpack::scoped_tensor& t) {
            return base_adaptor::import_(t).map([&](auto&& span) {
                return ContainerType(span, t.data_capsule());
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

        constexpr static bool keep_capsule = true;

        static dlpack::scoped_tensor export_(const MdSpanType& m) {

            auto *b_ptr = reinterpret_cast<byte*>(m.data_handle());

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
                /* extents = */ std::move(extents), /* strides = */ std::move(strides),
                (is_const<typename MdSpanType::element_type> ? DLPACK_FLAG_BITMASK_READ_ONLY : 0)
            };
        }

        static result<mdpan_type> import_(const dlpack::scoped_tensor& t) {
            EMU_TRUE_OR_RETURN_UN_EC(is_const<typename MdSpanType::element_type> or not t.read_only(), errc::dlpack_read_only);

            EMU_TRUE_OR_RETURN_UN_EC(t.rank() == extents_type::rank(), errc::dlpack_rank_mismatch);
            EMU_TRUE_OR_RETURN_UN_EC(t.dtype() == dtype, errc::dlpack_type_mismatch);

            if constexpr (not mdpan_type::is_always_exhaustive()) {
                EMU_TRUE_OR_RETURN_UN_EC(t.has_strides(), errc::dlpack_strides_not_supported);
            }

            auto b_ptr = reinterpret_cast<typename MdSpanType::element_type*>(t.get());

            auto mappint = EMU_UNWRAP(dlpack::detail::create_mapping_from_dltensor<mapping_type>(t.tensor()));

            return mdpan_type(b_ptr, mappint);
        }
    };

    template<cpts::any_mdcontainer MdContainerType>
    struct dlpack_adaptor<MdContainerType>
    {
        using base_adaptor = dlpack_adaptor<typename MdContainerType::mdspan_type>;

        constexpr static bool keep_capsule = false;

        static dlpack::scoped_tensor export_(const MdContainerType& m) {
            auto result = base_adaptor::export_(m);

            result.data_capsule = m.capsule();

            return result;
        }

        static result<MdContainerType> import_(const dlpack::scoped_tensor& t) {
            return base_adaptor::import_(t).map([&](auto&& mdspan) {
                return MdContainerType{mdspan, t.data_capsule()};
            });
        }
    };

    template<cpts::string_view StringView>
    struct dlpack_adaptor<StringView>
    {
        using base_adaptor = dlpack_adaptor< std::span<const char> >;

        constexpr static bool keep_capsule = true;

        static dlpack::scoped_tensor export_(const StringView& sv) {
            return base_adaptor::export_(std::span{sv});
        }

        static result<StringView> import_(const dlpack::scoped_tensor& t) {
            return base_adaptor::import_(t).map([&](auto&& span) {
                return StringView(span.begin(), span.end());
            });
        }
    };

namespace detail_cpts
{
    template<typename T>
    concept container = requires (const T& t, std::span<typename T::value_type> s) {
        { std::span{t} } -> std::same_as<std::span<const typename T::value_type>>;
        { s | ranges::to<T>() } -> std::same_as<T>;
    };

} // namespace detail_cpts

    template<detail_cpts::container Container>
        // requires cpts::relocatable_owning_range<Container>
    struct dlpack_adaptor<Container>
    {
        using value_type = typename Container::value_type;
        using copy_adaptor = dlpack_adaptor< std::span<const value_type> >;
        using move_adaptor = dlpack_adaptor< std::span<value_type> >;

        constexpr static bool keep_capsule = false;

        static dlpack::scoped_tensor export_(const Container& container) {
            return copy_adaptor::export_(std::span{container});
        }

        static dlpack::scoped_tensor export_(Container&& container) {

            // move container into a capsule and keep the final destinal address.
            auto [cap, ptr] = make_capsule_and_keep_addr<Container>(std::move(container));

            // Construct from new location address.
            auto result = move_adaptor::export_(std::span{*ptr});

            result.data_capsule_ = std::move(cap);
            // IS_COPIED means that scoped_tensor owns the data.
            result.flags |= DLPACK_FLAG_BITMASK_IS_COPIED;

            return result;
        }

        static result<Container> import_(const dlpack::scoped_tensor& t) {
            return copy_adaptor::import_(t).map([&](auto&& span) {
                return span | ranges::to<Container>();
            });
        }

        static result<Container> import_(dlpack::scoped_tensor&& t) {
            return move_adaptor::import_(t).map([&](auto&& span) {

                // tranform span iterator into "mode_iterator".
                auto begin = std::make_move_iterator(span.begin()),
                    end = std::make_move_iterator(span.end());

                // borrow content of scoped_tensor.
                return std::ranges::subrange(begin, end) | ranges::to<Container>();
            });
        }
    };

} // namespace spe

namespace dlpack
{

    template<typename T>
    scoped_tensor export_to_scoped_tensor(T&& t) {
        return spe::dlpack_adaptor<rm_cvref<T>>::export_(EMU_FWD(t));
    }

    template<typename T>
    auto import_from_scoped_tensor(const scoped_tensor& t) {
        return spe::dlpack_adaptor<T>::import_(t);
    }

    template<typename T>
    auto import_from_scoped_tensor(scoped_tensor&& t) {
        return spe::dlpack_adaptor<T>::import_(std::move(t));
    }

    template<typename T>
    constexpr bool keep_capsule = emu::spe::dlpack_adaptor<T>::keep_capsule;

} // namespace dlpack

} // namespace emu
