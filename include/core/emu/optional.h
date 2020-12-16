#ifndef EMU_OPTIONAL_H
#define EMU_OPTIONAL_H

#include <tl/optional.hpp>

#include <emu/macro.h>

namespace emu
{
    //TODO: Addsdocumentation.
    using tl::optional;


    /**
     * @brief Returns the opt content if exists or will create it from the additional arguments provided.
     *
     * This function is an alternative to Opt::value_or (e.g. std::optional::value_or, tl::optional::value_or)
     * when the user doesn't want to create a default_value if not used.
     *
     * @param opt - An optional value (e.g. std::optional, tl::optional).
     * @param args - variadic list of argument that will be forwarded to the constructor.
     */
    template<typename Opt, typename... Args>
    constexpr auto value_or_create(Opt && opt, Args &&... args)
        EMU_NOEXCEPT_EXPR( (opt) ? FWD(opt).value() : ValueType<Opt>(FWD(args)...) )
    {
        return (opt) ? FWD(opt).value() : ValueType<Opt>(FWD(args)...);
    }

    /**
     * @brief Returns the opt content if exists or will get it from the invocation of the provided callable.
     *
     * This function is an alternative to Opt::value_or (e.g. std::optional::value_or, tl::optional::value_or)
     * when the user doesn't want to invoke the function if not used.
     *
     * @param opt - An optional value (e.g. std::optional, tl::optional).
     * @param fn - A callable value that will return (e.g. function, function object).
     */
    template<typename Opt, typename Fn>
    constexpr auto value_or_invoke(Opt && opt, Fn && fn)
        EMU_NOEXCEPT_EXPR( (opt) ? FWD(opt).value() : FWD(fn)() )
    {
        return (opt) ? FWD(opt).value() : FWD(fn)();
    }
}

#endif //EMU_OPTIONAL_H