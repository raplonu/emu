#ifndef EMU_OPTIONAL_H
#define EMU_OPTIONAL_H

#include <emu/macro.h>
#include <emu/type_traits.h>
#include <emu/utility.h>

#include <tl/optional.hpp>

namespace emu
{
    //TODO: Addsdocumentation.
    template<typename T>
    using optional_t = tl::optional<T>;

    using tl::monostate;

    using tl::nullopt_t;
    using tl::nullopt;

    using tl::in_place_t;
    using tl::in_place;

    namespace optional
    {

        // /// Create an optional of type T by taking ownership of value.
        // template<typename T>
        // auto create(T&& value) {
        //     return optional_t<RemoveCVRef<T>>(EMU_FWD(value));
        // }

        // /// Create an optional of type T by taking ownership of value.
        // template<typename T>
        // auto wrap(T&& value) {
        //     return optional_t<T>(EMU_FWD(value));
        // }

        /**
         * @brief Returns the opt content if exists or will emplace it from the additional arguments provided.
         *
         * @param opt - An optional value (e.g. std::optional, tl::optional).
         * @param args - variadic list of argument that will be forwarded to the constructor.
         */
        template<typename Opt, typename... Args>
        constexpr auto value_or_emplace(Opt && opt, Args &&... args)
            EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : EMU_FWD(opt).emplace(EMU_FWD(args)...) )
        {
            return (opt) ? EMU_FWD(opt).value() : EMU_FWD(opt).emplace(EMU_FWD(args)...);
        }

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
            EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : ValueType<Opt>(EMU_FWD(args)...) )
        {
            return (opt) ? EMU_FWD(opt).value() : ValueType<Opt>(EMU_FWD(args)...);
        }

        /**
         * @brief Returns the opt content if exists or will get it from the invocation of the provided callable.
         *
         * This function is an alternative to Opt::value_or (e.g. std::optional::value_or, tl::optional::value_or)
         * when the user doesn't want to invoke the function if not used.
         *
         * @param opt - An optional value (e.g. std::optional, tl::optional).
         * @param fn - A callable value that will return (e.g. function, function object).
         * @param args - variadic list of argument that will be forwarded to the function.
         */
        template<typename Opt, typename Fn, typename... Args>
        constexpr auto value_or_invoke(Opt && opt, Fn && fn, Args &&... args)
            EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : EMU_FWD(fn)(EMU_FWD(args)...) )
        {
            return (opt) ? EMU_FWD(opt).value() : EMU_FWD(fn)(EMU_FWD(args)...);
        }

        /**
         * @brief Returns the opt content if exists or will emplace it from the invocation of the provided callable.
         *
         * This function is an alternative to value_or_emplace
         * when the user doesn't want to invoke the function if not used.
         *
         * @param opt - An optional value (e.g. std::optional, tl::optional).
         * @param fn - A callable value that will return (e.g. function, function object).
         * @param args - variadic list of argument that will be forwarded to the function.
         */
        template<typename Opt, typename Fn, typename... Args>
        constexpr auto value_or_invoke_emplace(Opt && opt, Fn && fn, Args &&... args)
            EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : EMU_FWD(fn)(EMU_FWD(args)...) )
        {
            return (opt) ? EMU_FWD(opt).value() : EMU_FWD(opt).emplace(EMU_FWD(fn)(EMU_FWD(args)...));
        }

        /**
         * @brief Cast underlying optional type into the specified type.
         *
         * @tparam To
         * @tparam Opt - An optional value (e.g. tl::optional).
         * @param opt
         * @return constexpr auto
         */
        template<typename To, typename Opt>
        constexpr auto as(Opt && opt)
            EMU_NOEXCEPT_EXPR( EMU_FWD(opt).map(emu::as<To>) )
        {
            return EMU_FWD(opt).map(emu::as<To>);
        }

    } // namespace optional


}

#endif //EMU_OPTIONAL_H