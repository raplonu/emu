#ifndef EMU_FUNCTIONAL_INVOKE_H
#define EMU_FUNCTIONAL_INVOKE_H

#include <emu/macro.h>
#include <emu/utility.h>

#include <type_traits>

namespace emu
{

namespace functional
{
    // C++14 workaround from https://stackoverflow.com/questions/38288042/c11-14-invoke-workaround
    // TODO : Delete when C++17 and cuda std provide it.
    template<typename Fn, typename... Args,
        std::enable_if_t<std::is_member_pointer<std::decay_t<Fn>>{}, int> = 0
    >
    EMU_HODE constexpr
    decltype(auto) invoke(Fn&& f, Args&&... args)
        EMU_NOEXCEPT_EXPR( std::mem_fn(f)(FWD(args)...) )
    {
        return std::mem_fn(f)(FWD(args)...);
    }

    template<typename Fn, typename... Args,
        std::enable_if_t<!std::is_member_pointer<std::decay_t<Fn>>{}, int> = 0
    >
    EMU_HODE constexpr
    decltype(auto) invoke(Fn&& f, Args&&... args)
        EMU_NOEXCEPT_EXPR( FWD(f)(FWD(args)...) )
    {
        return FWD(f)(FWD(args)...);
    }

} // namespace functional


} // namespace emu

#endif //EMU_FUNCTIONAL_INVOKE_H