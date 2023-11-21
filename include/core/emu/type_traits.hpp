#pragma once

#include <emu/macro.h>

#include <type_traits>
#include <iterator>

namespace emu
{

    template<typename T> using rm_cv = std::remove_cv_t<T>;
    template<typename T> using rm_const = std::remove_const_t<T>;
    template<typename T> using rm_volatile = std::remove_volatile_t<T>;
    template<typename T> using rm_ref = std::remove_reference_t<T>;
    template<typename T> using rm_cvref = rm_cv<rm_ref<T>>;
    template<typename T> using rm_ptr = std::remove_pointer_t<T>;
    template<typename T> using rm_extent = std::remove_extent_t<T>;
    template<typename T> using rm_all_extents = std::remove_all_extents_t<T>;
    template<typename T> using decay = std::decay_t<T>;

    template<typename T> using add_cv = std::add_cv_t<T>;
    template<typename T> using add_const = std::add_const_t<T>;
    template<typename T> using add_volatile = std::add_volatile_t<T>;
    template<typename T> using add_ptr = std::add_pointer_t<T>;
    template<typename T> using add_lref = std::add_lvalue_reference_t<T>;
    template<typename T> using add_rref = std::add_rvalue_reference_t<T>;

    struct use_default{};

    template<typename T, typename Other>
    using not_default_or = std::conditional_t<std::is_same_v<T, use_default>, Other, T>;

    template<typename T1, typename T2>
    constexpr auto equivalent = std::is_same_v<decay<T1>, decay<T2>>;

    template<class, template<class...> class>
    constexpr bool is_specialization = false;

    template<template<class...> class T, class... Args>
    constexpr bool is_specialization<T<Args...>, T> = true;

} // namespace emu
