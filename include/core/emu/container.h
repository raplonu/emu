#pragma once

#include <emu/utility.h>

#include <emu/misc/location.h>

// Include for accessor types.
#include <emu/mdspan.h>
#include <emu/scoped.h>
#include <emu/type_traits.h>

#include <memory>

namespace emu
{

    template<typename ViewType>
    struct container
    {

        using view_type = ViewType;

        struct interface
        {
            virtual ~interface() = default;
        };

        template<typename Capsule>
        struct implementation : interface
        {
            Capsule capsule_; // maintain a captured object alive if moved into.

            template<typename C>
            implementation(view_type view, C&& capsule)
                : view(view), capsule_(EMU_FWD(capsule))
            {}

            view_type view() override
            {
                return view;
            }
        };

        std::shared_ptr<interface> impl_;
        view_type view;

        template<typename C>
        container(view_type view, C&& capsule)
            : impl_(std::make_shared<implementation<Capsule>>(view, EMU_FWD(capsule)))
        {}

        template<typename C, typename... ViewArgs>
        container(C&& con, ViewArgs&&... view_args)
            : container(view_type(con, EMU_FWD(view_args)...), EMU_FWD(con))
        {}

    };

} // namespace emu