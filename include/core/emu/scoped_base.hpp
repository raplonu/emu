#pragma once

#include <emu/macro.hpp>
#include <emu/concepts.hpp>

#include <type_traits>
#include <utility>

namespace emu
{

    template<typename Base, typename Function>
    struct scoped_base : Base
    {
        using value_type = Base;
        using function_type = Function;

        static constexpr bool noexcept_invoke = std::is_nothrow_invocable_v<function_type, value_type>;

        template<typename BaseType, typename FunctionType>
        scoped_base(BaseType&& base, FunctionType&& function, bool owning = true)
            EMU_NOEXCEPT_EXPR(Base(EMU_FWD(base)), function_type(EMU_FWD(function)))
            : Base(EMU_FWD(base))
            , function(EMU_FWD(function))
            , owning_(owning)
        {}

        template<typename BaseType>
        scoped_base(BaseType&& base, bool owning = true)
            EMU_NOEXCEPT_EXPR(Base(EMU_FWD(base)), function_type())
            : Base(EMU_FWD(base))
            , function()
            , owning_(owning)
        {}

        template<typename FunctionType, typename... Args>
        constexpr scoped_base(FunctionType&& function, bool owning, Args&&... args)
            EMU_NOEXCEPT_EXPR(Base(EMU_FWD(args)...), function_type(EMU_FWD(function)))
            : Base(EMU_FWD(args)...)
            , function(EMU_FWD(function))
            , owning_(owning)
        {}

        template<typename... Args>
        constexpr scoped_base(bool owning, Args&&... args)
            EMU_NOEXCEPT_EXPR(Base(EMU_FWD(args)...), function_type())
            : Base(EMU_FWD(args)...)
            , function()
            , owning_(owning)
        {}


        scoped_base(const scoped_base& oc) = delete;

        constexpr scoped_base(scoped_base&& oc)
            EMU_NOEXCEPT_EXPR(Base(std::move(oc)), function_type(std::move(oc.function)))
            : Base(std::move(oc))
            , function(std::move(oc.function))
            , owning_(std::exchange(oc.owning_, false))
        {}

        scoped_base& operator=(const scoped_base& oc) = delete;

        scoped_base& operator=(scoped_base&& oc)
            noexcept(noexcept_invoke and noexcept(Base(std::move(oc))) and noexcept(std::declval<function_type&>() = std::move(oc.function)))
        {
            if (this != &oc) {
                invoke();
                Base::operator=(std::move(oc));
                function = std::move(oc.function);
                owning_ = std::exchange(oc.owning_, false);
            }
            return *this;
        }

        ~scoped_base() noexcept(noexcept_invoke) {
            invoke();
        }

        Base release() noexcept {
            owning_ = false;

            return std::move(*this);
        }

        template<typename NewBase>
        void reset(NewBase&& new_base, bool owning = true)
            noexcept(noexcept_invoke)
        {
            invoke();
            Base::operator=(EMU_FWD(new_base));
            owning_ = owning;
        }

        value_type& value() & noexcept { return static_cast<Base&>(*this); }
        const value_type& value() const& noexcept { return static_cast<const Base&>(*this); }
        value_type value() && noexcept { return static_cast<Base&&>(*this); }

        constexpr bool owning() const noexcept {
            return owning_;
        }

    private:
        void invoke() noexcept(noexcept_invoke){
            if (owning_) function(value());
        }

        [[no_unique_address]] function_type function;
        bool owning_ = true;

    };

    template<typename F>
    struct scoped_base<void, F>
    {
        using function_type = F;

        static constexpr bool noexcept_invoke = std::is_nothrow_invocable_v<function_type>;

        template<typename FunctionType>
        scoped_base(FunctionType&& function, bool owning = true)
            EMU_NOEXCEPT_EXPR(function_type(EMU_FWD(function)))
            : function(EMU_FWD(function))
            , owning_(owning)
        {}

        scoped_base(bool owning = true)
            EMU_NOEXCEPT_EXPR(function_type())
            : function()
            , owning_(owning)
        {}

        scoped_base(const scoped_base& oc) = delete;

        constexpr scoped_base(scoped_base&& oc)
            EMU_NOEXCEPT_EXPR(function_type(std::move(oc.function)))
            : function(std::move(oc.function))
            , owning_(std::exchange(oc.owning_, false))
        {}

        scoped_base& operator=(const scoped_base& oc) = delete;

        scoped_base& operator=(scoped_base&& oc)
            noexcept(noexcept_invoke and noexcept(std::declval<function_type&>() = std::move(oc.function)))
        {
            if (this != &oc) {
                invoke();
                function = std::move(oc.function);
                owning_ = std::exchange(oc.owning_, false);
            }
            return *this;
        }

        ~scoped_base() noexcept(noexcept_invoke) {
            invoke();
        }

        void release() noexcept {
            owning_ = false;
        }

    private:
        void invoke() noexcept(noexcept_invoke) {
            if (owning_) function();
        }

        [[no_unique_address]] function_type function;
        bool owning_ = true;
    };

} // namespace emu
