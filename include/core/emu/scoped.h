#ifndef EMU_RESOURCE_H
#define EMU_RESOURCE_H

#include <emu/utility.h>
#include <emu/type_traits.h>

#include <utility>

namespace emu
{
    template<typename Base, typename Derive>
    using EnableIfNotBase = EnableIf<not std::is_base_of<RemoveCVRef<Base>, RemoveCVRef<Derive>>::value>;

    template<typename T, typename F>
    struct scoped_t
    {
        using value_type = T;
        using function_type = F;

        constexpr scoped_t() = default;

        template<typename T1, bool = EnableIfNotBase<scoped_t, T1>{}>
        constexpr scoped_t(T1 value, bool owning = true):
            value(FWD(value)), function(), owning_(owning)
        {}

        template<typename T1, typename F1, bool = EnableIf<not Equivalent<F1, bool>::value>{}>
        constexpr scoped_t(T1 value, F1 function, bool owning = true):
            value(FWD(value)), function(FWD(function)), owning_(owning)
        {}

        scoped_t(const scoped_t & oc) = delete;

        constexpr scoped_t(scoped_t && oc):
            value(mv(oc.value)), function(mv(oc.function)),
            owning_(std::exchange(oc.owning_, false))
        {}

        scoped_t& operator=(const scoped_t & oc) = delete;

        scoped_t& operator=(scoped_t && oc) {
            invoke();

            value    = mv(oc.value);
            function = mv(oc.function);
            owning_ = std::exchange(oc.owning_, false);

            return *this;
        };

        ~scoped_t() {
            invoke();
        }

        constexpr bool owning() const noexcept {
            return owning_;
        }

    private:
        void invoke() {
            if (owning_) function(value);
        }

    public:
        value_type value;
        function_type function;

    private:
        bool owning_ = true;
    };

    template<typename F>
    struct scoped_t<void, F>
    {
        using function_type = F;

        constexpr scoped_t() = default;

        template<typename F1, bool = EnableIfNotBase<scoped_t, F1>{}>
        constexpr scoped_t(F1 function, bool owning = true):
            function(FWD(function)), owning_(owning)
        {}

        scoped_t(const scoped_t & oc) = delete;

        constexpr scoped_t(scoped_t && oc):
            function(mv(oc.function)),
            owning_(std::exchange(oc.owning_, false))
        {}

        scoped_t& operator=(const scoped_t & oc) = delete;

        scoped_t& operator=(scoped_t && oc) {
            invoke();

            function = mv(oc.function);
            owning_ = std::exchange(oc.owning_, false);

            return *this;
        };

        ~scoped_t() {
            invoke();
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

namespace scoped
{
    /// Returns a scoped_t for the given value and function.
    /// Useful to do type deduction.
    ///
    /// Regular T, FunctionObject<U (T)> F where U is not constrained
    template<typename T, typename F>
    constexpr scoped_t<std::decay_t<T>, std::decay_t<F>> create(T&& value, F&& f) {
        return scoped_t<std::decay_t<T>, std::decay_t<F>>{FWD(value), FWD(f)};
    }

    /// Returns a scoped_t for the given function.
    /// Useful to do type deduction.
    ///
    /// FunctionObject<U (T)> F where U is not constrained
    template<typename F>
    constexpr scoped_t<void, std::decay_t<F>> create(F&& f) {
        return scoped_t<void, std::decay_t<F>>{FWD(f)};
    }

#define EMU_INVOKE_AT_SCOPE_EXIT(F)                                         \
    const auto EMU_UNIQUE_NAME(invoke_at_scope_exit) = ::emu::scoped::create(F)

    template<typename T, typename F>
    constexpr scoped_t<std::decay_t<T>, std::decay_t<F>> wrap(T&& value, F&& f) {
        return scoped_t<std::decay_t<T>, std::decay_t<F>>{FWD(value), FWD(f), false};
    }

    template<typename F>
    constexpr scoped_t<void, std::decay_t<F>> wrap(F&& f) {
        return scoped_t<void, std::decay_t<F>>{FWD(f), false};
    }


    template<typename T, typename V>
    constexpr auto assign_for_current_scope(T & t, V && v)
        EMU_NOEXCEPT_EXPR(std::exchange(t, FWD(v)))
    {
        auto old = std::exchange(t, FWD(v));
        return create([&t, old = mv(old)]{ t = mv(old); });
    }

#define EMU_ASSIGN_FOR_CURRENT_SCOPE(T, V)                                                    \
    const auto EMU_UNIQUE_NAME(assign_value_for_scope) = ::emu::scoped::assign_for_current_scope(T, V)

} // namespace scope

} // namespace emu

#endif //EMU_RESOURCE_H