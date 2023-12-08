#pragma once

#include <emu/utility.hpp>

#include <concepts>
#include <type_traits>
#include <memory>

namespace emu
{

    struct capsule {

        struct interface {
            virtual ~interface() = default;
        };

        template<typename DataHolder>
        struct impl : interface {
            impl(auto&& d) : data_holder(EMU_FWD(d)) {}

            DataHolder data_holder;
        };

        capsule() = default;

        template<typename DataHolder>
            requires (not is_lref<DataHolder>) and (not std::same_as<capsule, std::decay_t<DataHolder>>)
        capsule(DataHolder&& d) : holder(std::make_shared<impl<DataHolder>>(EMU_FWD(d))) {}

        std::shared_ptr<interface> holder;

        /// shared_ptr interface

        long use_count() const { return holder.use_count(); }
        void reset() { holder.reset(); }
        operator bool() const { return static_cast<bool>(holder); }
    };

} // namespace emu
