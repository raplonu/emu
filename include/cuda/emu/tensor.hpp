#pragma once

#include <emu/info.hpp>
#include <emu/cuda/mdspan.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <matx.h>

namespace emu
{

namespace cpts
{
    template<typename T>
    concept tensor = matx::is_tensor_view_v<T>;

} // namespace cpts

namespace detail
{
    template<cpts::tensor T>
    auto c_contigous(const T& tensor) {
        auto size = 1;
        for (int i = T::Rank() - 1; i >= 0; --i) {
            if (tensor.Descriptor().Stride(i) != size) {
                return false;
            }
            size *= tensor.Size(i);
        }
        return true;
        // or return tensor.IsContiguous();
    }

    template<cpts::tensor T>
    auto f_contigous(const T& tensor) {
        auto size = 1;
        for (int i = 0; i < T::Rank(); ++i) {
            if (tensor.Descriptor().Stride(i) != size) {
                return false;
            }
            size *= tensor.Size(i);
        }
        return true;
    }

} // namespace detail

    template<cpts::tensor Tensor>
    auto as_span(Tensor& tensor) {
        EMU_ASSERT_MSG(tensor.IsContiguous(), "Tensor must be contiguous to create a span");
        return cuda::span{tensor.Data(), static_cast<std::size_t>(tensor.TotalSize())};
    }

    template<cpts::span Span>
    auto as_tensor(Span span) {
        return matx::make_tensor(span.data(), {static_cast<int>(span.size())}, /*owning = */false);
    }

    template<
        typename LayoutPolicy = layout_c,
        cpts::tensor Tensor
    >
    auto as_mdspan(Tensor& tensor) {
        using element_type = typename Tensor::value_type;
        constexpr std::size_t rank = Tensor::Rank();

        using extents = _nd<rank>;

        if constexpr (std::is_same_v<LayoutPolicy, layout_c>) {
            EMU_ASSERT_MSG(is_c_contigous(tensor), "Tensor must be order c contiguous");

            return stdex::mdspan<element_type, extents, layout_c>{
                tensor.Data(), tensor.Shape()
            };
        } else if constexpr (std::is_same_v<LayoutPolicy, layout_f>) {
            EMU_ASSERT_MSG(is_f_contigous(tensor), "Tensor must be order f contiguous");

            return stdex::mdspan<element_type, extents, layout_f>{
                tensor.Data(), tensor.Shape()
            };
        } else {
            using mapping_type = typename LayoutPolicy::template mapping<extents>;
            return stdex::mdspan<element_type, extents, LayoutPolicy>{
                tensor.Data(), mapping_type{tensor.Shape(), tensor.Descriptor().Strides()}
            };
        }
    }

    template<cpts::mdspan Mdspan>
    auto as_tensor(Mdspan mdspan) {
        constexpr std::size_t rank = Mdspan::rank();

        return [&]<std::size_t... Is>(std::index_sequence<Is...>){
            return matx::make_tensor(
                mdspan.data_handle(),
                {static_cast<int>(mdspan.extent(Is))...},
                {static_cast<int>(mdspan.stride(Is))...},
                /*owning = */false);
        }(std::make_index_sequence<rank>{});
    }

namespace spe
{

    // template<cpts::tensor Tensor>
    template <typename T, int RANK, typename Storage, typename Desc>
    struct info_t< matx::tensor_t<T, RANK, Storage, Desc> > : default_info_t< matx::tensor_t<T, RANK, Storage, Desc> >
    {
        using type = matx::tensor_t<T, RANK, Storage, Desc>;

        // constexpr auto format_type(fmt::format_context::iterator it) const {
        //     return it;
        // }
        constexpr auto format_value(const type &t, fmt::format_context::iterator it) const {
            auto [layout, is_stride] = [&] {
                if (detail::c_contigous(t))
                    return std::make_pair("C/right", false);

                if (detail::f_contigous(t))
                    return std::make_pair("F/left", false);

                //TODO: check if stride is correct.
                return std::make_pair("stride", true);
            }();

            it = fmt::format_to(it, "@{}[{}:{}]", fmt::ptr(t.Data()), t.Shape(), layout);
            if (is_stride) {
                it = fmt::format_to(it, "[{}]", t.Stride());
            }
            return it;
        }
    };

} // namespace spe

    template <typename T, int RANK>
    constexpr auto rank(const matx::tensor_t<T, RANK>&) {
        return RANK;
    }

} // namespace emu

namespace std
{

    template<emu::cpts::tensor Tensor>
    constexpr auto data(Tensor& tensor) {
        return tensor.Data();
    }

    template<emu::cpts::tensor Tensor>
    constexpr auto data(const Tensor& tensor) {
        return tensor.Data();
    }

    template<emu::cpts::tensor Tensor>
    constexpr auto size(const Tensor& tensor) {
        return tensor.TotalSize();
    }

} // namespace std

namespace fmt
{
    // Disable range support for tensor in fmt.
    template <emu::cpts::tensor Tensor, typename Char>
    struct is_range<Tensor, Char>: std::false_type{};

} // namespace fmt
