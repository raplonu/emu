#pragma once

#include <emu/type_traits.hpp>
#include <emu/blas/blas_type.hpp>
#include <emu/mdalgo.hpp>

namespace emu::linealg::blas
{

    template<typename Ctx, typename Alpha, typename A, typename X, typename Beta, typename Y>
    void gemv(
        const Ctx &ctx,
        transpose_mode t_mode,
        Alpha&& alpha,
        const A& A_view,
        const X& x_view,
        Beta&& beta,
        Y& y_view
    ) {

        v_or_p alpha_v = alpha;
        v_or_p beta_v = beta;

        auto A_mdspan = as_md(A_view); using A_mdspan_type = decltype(A_mdspan);
        auto x_mdspan = as_md(x_view); using X_mdspan_type = decltype(x_mdspan);
        auto y_mdspan = as_md(y_view); using Y_mdspan_type = decltype(y_mdspan);

        using data_type = rm_const<typename A_mdspan_type::element_type>;

        static_assert(
            A_mdspan.rank() == 2 and x_mdspan.rank() == 1 and y_mdspan.rank() == 1,
            "gemv requires A to be a 2D view, x and y to be 1D views"
        );

        static_assert(
            // std::same_as<data_type, typename A_mdspan_type::element_type> and
            std::same_as<data_type, rm_const<typename X_mdspan_type::element_type>> and
            std::same_as<data_type, rm_const<typename Y_mdspan_type::element_type>>,
            "gemv requires A, x and y to be the same type"
        );

        auto layout = get_layout_mode(A_mdspan.mapping());

        //TODO: Check if it depends on the transpose mode.
        auto m = A_mdspan.extent(0);
        auto n = A_mdspan.extent(1);

        constexpr auto f_gemv = Ctx::template blas_type<data_type>::gemv;

        ctx.execute(f_gemv,
            layout,
            Ctx::transpose(t_mode),
            m,
            n,
            ctx.get_scalar(alpha_v),
            A_mdspan.data_handle(),
            A_mdspan.extent(1),
            x_mdspan.data_handle(),
            x_mdspan.stride(0),
            ctx.get_scalar(beta_v),
            y_mdspan.data_handle(),
            y_mdspan.stride(0)
        );
    }

} // namespace emu::linealg::blas
