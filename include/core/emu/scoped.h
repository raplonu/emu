#ifndef EMU_SCOPED_H
#define EMU_SCOPED_H

#include <emu/type_traits.h>
#include <emu/utility.h>

#include <utility>

namespace emu
{

    template<typename Base, typename Derive>
    using EnableIfNotBase = EnableIf<not std::is_base_of_v<RemoveCVRef<Base>, RemoveCVRef<Derive>>>;

    /**
     * @brief A scoped object that calls a destructor when it goes out of scope.
     *
     * @tparam T The type of the object to be destroyed.
     * @tparam F The type of the destructor function.
     */
    template<typename T, typename F>
    struct scoped_t
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
        constexpr scoped_t() = default;

        template<typename T1, EnableIfNotBase<scoped_t, T1> = true>
        constexpr scoped_t(T1 && value, bool owning = true)
            EMU_NOEXCEPT_EXPR(value_type(EMU_FWD(value)), function_type())
        :
            value(EMU_FWD(value)), function(), owning_(owning)
        {}

        template<typename T1, typename F1, EnableIf<not Equivalent<F1, bool>> = true>
        constexpr scoped_t(T1 && value, F1 && function, bool owning = true)
            EMU_NOEXCEPT_EXPR(value_type(EMU_FWD(value)), function_type(EMU_FWD(function)))
        :
            value(EMU_FWD(value)), function(EMU_FWD(function)), owning_(owning)
        {}

        scoped_t(const scoped_t & oc) = delete;

        constexpr scoped_t(scoped_t && oc)
            EMU_NOEXCEPT_EXPR(value_type(mv(oc.value)), function_type(mv(oc.function))):
            value(mv(oc.value)), function(mv(oc.function)),
            owning_(std::exchange(oc.owning_, false))
        {}

        scoped_t& operator=(const scoped_t & oc) = delete;

        scoped_t& operator=(scoped_t && oc)
            noexcept(noexcept_invoke and noexcept(std::declval<value_type&>() = mv(oc.value), std::declval<function_type&>() = mv(oc.function)))
        {
            invoke();

            value    = mv(oc.value);
            function = mv(oc.function);
            owning_ = std::exchange(oc.owning_, false);

            return *this;
        };

        ~scoped_t() noexcept(noexcept_invoke) {
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
    struct scoped_t<void, F>
    {
        using function_type = F;

        static constexpr bool noexcept_invoke = EMU_NOEXCEPT_EXPR(std::declval<function_type&>()());

        constexpr scoped_t() = default;

        template<typename F1, bool = EnableIfNotBase<scoped_t, F1>{}>
        constexpr scoped_t(F1 function, bool owning = true)
            EMU_NOEXCEPT_EXPR(function_type(EMU_FWD(function))):
            function(EMU_FWD(function)), owning_(owning)
        {}

        scoped_t(const scoped_t & oc) = delete;

        constexpr scoped_t(scoped_t && oc)
            EMU_NOEXCEPT_EXPR(function_type(mv(oc.function)))
        :
            function(mv(oc.function)),
            owning_(std::exchange(oc.owning_, false))
        {}

        scoped_t& operator=(const scoped_t & oc) = delete;

        scoped_t& operator=(scoped_t && oc)
            noexcept(noexcept_invoke and noexcept(std::declval<function_type&>() = mv(oc.function)))
        {
            invoke();

            function = mv(oc.function);
            owning_ = std::exchange(oc.owning_, false);

            return *this;
        };

        ~scoped_t() noexcept(noexcept_invoke) {
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

namespace detail
{

    struct deleter_t {
        template<typename T>
        void operator()(T* ptr) {
            delete ptr;
        }
    };

} // namespace detail

    template<typename T, typename Deleter = detail::deleter_t>
    using scoped_ptr_t = scoped_t<T*, Deleter>;

namespace scoped
{

    /**
     * @brief Returns a scoped_t for the given value and function.
     *
     * Useful to do type deduction.
     *
     * @tparam T Regular
     * @tparam F FunctionObject<U (T)>
     * @param value The contained value.
     * @param f The function to call when the scoped_t is destroyed.
     * @return scoped struct.
     */
    template<typename T, typename F>
    constexpr auto create(T&& value, F&& f) {
        return scoped_t<std::decay_t<T>, std::decay_t<F>>{EMU_FWD(value), EMU_FWD(f)};
    }

    /// Returns a scoped_t for the given function.
    /// Useful to do type deduction.
    ///
    /// FunctionObject<U (T)> F where U is not constrained
    template<typename F>
    constexpr auto create(F&& f) {
        return scoped_t<void, std::decay_t<F>>{EMU_FWD(f)};
    }

    /// Returns a scoped_t for the given value and function.
    /// The function won't be called if the scoped_t is destroyed.
    template<typename T, typename F>
    constexpr auto wrap(T&& value, F&& f) {
        return scoped_t<std::decay_t<T>, std::decay_t<F>>{EMU_FWD(value), EMU_FWD(f), false};
    }

    template<typename F>
    constexpr auto wrap(F&& f) {
        return scoped_t<void, std::decay_t<F>>{EMU_FWD(f), false};
    }

    template<typename T, typename V>
    constexpr auto assign_for_current_scope(T & t, V && v)
        EMU_NOEXCEPT_EXPR(std::exchange(t, EMU_FWD(v)))
    {
        auto old = std::exchange(t, EMU_FWD(v));
        return create([&t, old = mv(old)]{ t = mv(old); });
    }

#define EMU_INVOKE_AT_SCOPE_EXIT(F)                                         \
    const auto EMU_UNIQUE_NAME(invoke_at_scope_exit) = ::emu::scoped::create(F)

#define EMU_ASSIGN_FOR_CURRENT_SCOPE(T, V)                                                    \
    const auto EMU_UNIQUE_NAME(assign_value_for_scope) = ::emu::scoped::assign_for_current_scope(T, V)

} // namespace scope

} // namespace emu

#endif //EMU_SCOPED_H