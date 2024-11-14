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
        static constexpr bool noexcept_invoke = std::is_nothrow_invocable_v<function_type, value_type>;
        //();EMU_NOEXCEPT_EXPR(std::declval<function_type&>()(std::declval<value_type&>()));

        /**
         * @brief Default constructor.
         *
         */
        constexpr scoped() = default;

        /**
         * @brief Constructs a scoped object with the given value and destructor function.
         *
         * @tparam T1 The type of the value.
         * @param value The value to be stored.
         * @param owning Flag indicating ownership of the value.
         */
        template<cpts::not_derived_from<scoped> T1>
        constexpr scoped(T1 && value, bool owning = true)
            EMU_NOEXCEPT_EXPR(value_type(EMU_FWD(value)), function_type())
        :
            value(EMU_FWD(value)), function(), owning_(owning)
        {}

        /**
         * @brief Constructs a scoped object with the given value, destructor function, and ownership flag.
         *
         * @tparam T1 The type of the value.
         * @tparam F1 The type of the destructor function.
         * @param value The value to be stored.
         * @param function The destructor function.
         * @param owning Flag indicating ownership of the value.
         */
        template<typename T1, cpts::not_equivalent<bool> F1>
        constexpr scoped(T1 && value, F1 && function, bool owning = true)
            EMU_NOEXCEPT_EXPR(value_type(EMU_FWD(value)), function_type(EMU_FWD(function)))
        :
            value(EMU_FWD(value)), function(EMU_FWD(function)), owning_(owning)
        {}

        scoped(const scoped & oc) = delete;

        /**
         * @brief Move constructor.
         *
         * @param oc The scoped object to be moved.
         */
        constexpr scoped(scoped && oc)
            EMU_NOEXCEPT_EXPR(value_type(std::move(oc.value)), function_type(std::move(oc.function)))
            : value(std::move(oc.value)), function(std::move(oc.function))
            , owning_(std::exchange(oc.owning_, false))
        {}

        scoped& operator=(const scoped & oc) = delete;

        /**
         * @brief Move assignment operator.
         *
         * @param oc The scoped object to be moved.
         * @return scoped& The reference to the moved scoped object.
         */
        scoped& operator=(scoped && oc)
            noexcept(noexcept_invoke and noexcept(std::declval<value_type&>() = std::move(oc.value), std::declval<function_type&>() = std::move(oc.function)))
        {
            invoke();

            value    = std::move(oc.value);
            function = std::move(oc.function);
            owning_ = std::exchange(oc.owning_, false);

            return *this;
        };

        /**
         * @brief Destructor.
         *
         */
        ~scoped() noexcept(noexcept_invoke) {
            invoke();
        }

        /**
         * @brief Dereference operator.
         *
         * @return T& The reference to the stored value.
         */
        constexpr T& operator*() noexcept {
            return value;
        }

        /**
         * @brief Const dereference operator.
         *
         * @return const T& The const reference to the stored value.
         */
        constexpr const T& operator*() const noexcept {
            return value;
        }

        // TODO: Adds value methods.
        // TODO: Adds support for rvalue this (&&) methods.

        /**
         * @brief Releases ownership of the stored value.
         *
         * @return T The released value.
         */
        constexpr T release()
            EMU_NOEXCEPT_EXPR(std::move(std::declval<value_type&>()))
        {
            owning_ = false;
            return std::move(value);
        }

        /**
         * @brief Resets the stored value and ownership flag.
         *
         * @tparam T1 The type of the new value.
         * @param new_value The new value to be stored.
         * @param owning Flag indicating ownership of the new value.
         */
        template<typename T1>
        constexpr void reset(T1 && new_value, bool owning = true)
            noexcept(noexcept_invoke and noexcept(std::declval<value_type&>() = EMU_FWD(new_value)))
        {
            invoke();
            value = EMU_FWD(new_value);
            owning_ = owning;
        }

        /**
         * @brief Resets the stored value, destructor function, and ownership flag.
         *
         * @tparam T1 The type of the new value.
         * @tparam F1 The type of the new destructor function.
         * @param new_value The new value to be stored.
         * @param new_function The new destructor function.
         * @param owning Flag indicating ownership of the new value.
         */
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

        /**
         * @brief Checks if the scoped object owns the value.
         *
         * @return bool True if the scoped object owns the value, false otherwise.
         */
        constexpr bool owning() const noexcept {
            return owning_;
        }

    private:
        /**
         * @brief Invokes the destructor function if the scoped object owns the value.
         *
         */
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
            EMU_NOEXCEPT_EXPR(function_type(std::move(oc.function)))
        :
            function(std::move(oc.function)),
            owning_(std::exchange(oc.owning_, false))
        {}

        scoped& operator=(const scoped & oc) = delete;

        scoped& operator=(scoped && oc)
            noexcept(noexcept_invoke and noexcept(std::declval<function_type&>() = std::move(oc.function)))
        {
            invoke();

            function = std::move(oc.function);
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

    /**
     * @brief Alias for a scoped pointer.
     *
     * @tparam T The type of the pointer.
     * @tparam Deleter The type of the deleter function.
     */
    template<typename T, typename Deleter = detail::deleter>
    using scoped_ptr = scoped<T*, Deleter>;

    /**
     * @brief Assigns a new value to a variable for the current scope.
     *
     * @tparam T The type of the variable.
     * @tparam V The type of the new value.
     * @param t The variable to be assigned.
     * @param v The new value to be assigned.
     * @return scoped A scoped object that assigns the old value back to the variable when it goes out of scope.
     */
    template<typename T, typename V>
    constexpr auto assign_for_current_scope(T & t, V && v)
        EMU_NOEXCEPT_EXPR(std::exchange(t, EMU_FWD(v)))
    {
        auto old = std::exchange(t, EMU_FWD(v));
        return scoped([&t, old = std::move(old)]{ t = std::move(old); });
    }

    template<typename T, typename V>
    constexpr auto assign_at_scope_exit(T & t, V && v)
    {
        return scoped([&t, v = std::move(v)]{ t = std::move(v); });
    }


} // namespace emu

#define EMU_INVOKE_AT_SCOPE_EXIT(F) \
    const ::emu::scoped EMU_UNIQUE_NAME(invoke_at_scope_exit)(F)

#define EMU_ASSIGN_FOR_CURRENT_SCOPE(T, V) \
    const auto EMU_UNIQUE_NAME(assign_value_for_scope) = ::emu::assign_for_current_scope(T, V)

#define EMU_ASSIGN_AT_SCOPE_EXIT(T, V) \
    const auto EMU_UNIQUE_NAME(assign_value_at_scope_exit) = ::emu::assign_at_scope_exit(T, V)
