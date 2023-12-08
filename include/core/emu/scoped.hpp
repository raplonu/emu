#pragma once

#include <emu/macro.hpp>
#include <emu/concepts.hpp>
#include <emu/utility.hpp>

#include <utility>

namespace emu
{
    /**
     * @brief A scoped object that calls a destructor when it goes out of scope.
     *
     * @tparam T The type of the object to be destroyed.
     * @tparam F The type of the destructor function.
     */
    template<typename T, typename F>
    struct scoped
    {
        /// The type of the object to be destroyed.
        using value_type = T;
        /// The type of the destructor function.
        using function_type = F;

        /// The object to be destroyed.
        static constexpr bool noexcept_invoke = EMU_NOEXCEPT_EXPR(std::declval<function_type&>()(std::declval<value_type&>()));

        /**
         * @brief Default constructor.
         *
         */
        constexpr scoped() = default;

        template<cpts::not_derived_from<scoped> T1>
        constexpr scoped(T1 && value, bool owning = true)
            EMU_NOEXCEPT_EXPR(value_type(EMU_FWD(value)), function_type())
        :
            value(EMU_FWD(value)), function(), owning_(owning)
        {}

        template<typename T1, cpts::not_equivalent<bool> F1>
        constexpr scoped(T1 && value, F1 && function, bool owning = true)
            EMU_NOEXCEPT_EXPR(value_type(EMU_FWD(value)), function_type(EMU_FWD(function)))
        :
            value(EMU_FWD(value)), function(EMU_FWD(function)), owning_(owning)
        {}

        scoped(const scoped & oc) = delete;

        constexpr scoped(scoped && oc)
            EMU_NOEXCEPT_EXPR(value_type(mv(oc.value)), function_type(mv(oc.function))):
            value(mv(oc.value)), function(mv(oc.function)),
            owning_(std::exchange(oc.owning_, false))
        {}

        scoped& operator=(const scoped & oc) = delete;

        scoped& operator=(scoped && oc)
            noexcept(noexcept_invoke and noexcept(std::declval<value_type&>() = mv(oc.value), std::declval<function_type&>() = mv(oc.function)))
        {
            invoke();

            value    = mv(oc.value);
            function = mv(oc.function);
            owning_ = std::exchange(oc.owning_, false);

            return *this;
        };

        ~scoped() noexcept(noexcept_invoke) {
            invoke();
        }

        constexpr T& operator*() noexcept {
            return value;
        }

        constexpr const T& operator*() const noexcept {
            return value;
        }

        // TODO: Adds value methods.
        // TODO: Adds support for rvalue this (&&) methods.

        constexpr T release()
            EMU_NOEXCEPT_EXPR(mv(std::declval<value_type&>()))
        {
            owning_ = false;
            return mv(value);
        }

        template<typename T1>
        constexpr void reset(T1 && new_value, bool owning = true)
            noexcept(noexcept_invoke and noexcept(std::declval<value_type&>() = EMU_FWD(new_value)))
        {
            invoke();
            value = EMU_FWD(new_value);
            owning_ = owning;
        }

        template<typename T1, typename F1>
        constexpr void reset(T1 && new_value, F1 && new_function, bool owning = true)
            noexcept(noexcept_invoke
                and noexcept(std::declval<value_type&>() = EMU_FWD(new_value))
                and noexcept(std::declval<function_type&>() = EMU_FWD(new_function)))
        {
            invoke();
            value = EMU_FWD(new_value);
            function = EMU_FWD(new_function);
            owning_ = owning;
        }

        constexpr bool owning() const noexcept {
            return owning_;
        }

    private:
        void invoke() noexcept(noexcept_invoke) {
            if (owning_) function(value);
        }

    public:
        value_type value;
        function_type function;

    private:
        bool owning_ = false;
    };

    template<typename F>
    struct scoped<void, F>
    {
        using value_type = void;
        using function_type = F;

        static constexpr bool noexcept_invoke = EMU_NOEXCEPT_EXPR(std::declval<function_type&>()());

        constexpr scoped() = default;

        template<cpts::not_derived_from<scoped> F1>
        constexpr scoped(F1 function, bool owning = true)
            EMU_NOEXCEPT_EXPR(function_type(EMU_FWD(function))):
            function(EMU_FWD(function)), owning_(owning)
        {}

        scoped(const scoped & oc) = delete;

        constexpr scoped(scoped && oc)
            EMU_NOEXCEPT_EXPR(function_type(mv(oc.function)))
        :
            function(mv(oc.function)),
            owning_(std::exchange(oc.owning_, false))
        {}

        scoped& operator=(const scoped & oc) = delete;

        scoped& operator=(scoped && oc)
            noexcept(noexcept_invoke and noexcept(std::declval<function_type&>() = mv(oc.function)))
        {
            invoke();

            function = mv(oc.function);
            owning_ = std::exchange(oc.owning_, false);

            return *this;
        };

        ~scoped() noexcept(noexcept_invoke) {
            invoke();
        }

        constexpr void release() noexcept {
            owning_ = false;
        }

        constexpr void reset(bool owning = true)
            noexcept(noexcept_invoke)
        {
            invoke();
            owning_ = owning;
        }

        constexpr bool owning() const noexcept {
            return owning_;
        }

    private:
        void invoke() {
            if (owning_) function();
        }

    public:
        function_type function;

    private:
        bool owning_ = true;
    };

    template<typename T, typename F>
    scoped(T&&, F&&) -> scoped<std::decay_t<T>, std::decay_t<F>>;

    template<typename T, typename F>
    scoped(T&&, F&&, bool) -> scoped<std::decay_t<T>, std::decay_t<F>>;

    template<typename T, typename F>
    scoped(scoped<T, F>&&) -> scoped<T, F>;

    template<typename F>
    scoped(F&&) -> scoped<void, std::decay_t<F>>;

    template<typename F>
    scoped(F&&, bool) -> scoped<void, std::decay_t<F>>;

namespace detail
{

    struct deleter {
        template<typename T>
        void operator()(T* ptr) {
            delete ptr;
        }
    };

} // namespace detail

    template<typename T, typename Deleter = detail::deleter>
    using scoped_ptr = scoped<T*, Deleter>;

    template<typename T, typename V>
    constexpr auto assign_for_current_scope(T & t, V && v)
        EMU_NOEXCEPT_EXPR(std::exchange(t, EMU_FWD(v)))
    {
        auto old = std::exchange(t, EMU_FWD(v));
        return scoped([&t, old = mv(old)]{ t = mv(old); });
    }

} // namespace emu

#define EMU_INVOKE_AT_SCOPE_EXIT(F)                              \
    const ::emu::scoped EMU_UNIQUE_NAME(invoke_at_scope_exit)(F)

#define EMU_ASSIGN_FOR_CURRENT_SCOPE(T, V)                                                     \
    const auto EMU_UNIQUE_NAME(assign_value_for_scope) = ::emu::assign_for_current_scope(T, V)
