#pragma once

#include <emu/macro.h>

#include <type_traits>
#include <iterator>

namespace emu
{

    struct use_default{};

    template<typename T, typename Other>
    using not_default_or = std::conditional_t<std::is_same_v<T, use_default>, Other, T>;

    template<typename T1, typename T2>
    constexpr auto equivalent = std::is_same_v<std::decay_t<T1>, std::decay_t<T2>>;

    template<class, template<class...> class>
    constexpr bool is_specialization = false;

    template<template<class...> class T, class... Args>
    constexpr bool is_specialization<T<Args...>, T> = true;

//     template<typename T>
//     using value_type      = typename std::decay_t<T>::value_type;
//     template<typename T>
//     using key_type        = typename std::decay_t<T>::key_type;
//     template<typename T>
//     using mapped_type     = typename std::decay_t<T>::mapped_type;
//     template<typename T>
//     using difference_type = typename std::decay_t<T>::difference_type;
//     template<typename T>
//     using iterator_category        = typename std::decay_t<T>::iterator_category;

// namespace iterator
// {
//     template<typename It>
//     using value_type      = emu::value_type<std::iterator_traits<std::decay_t<It>>>;
//     template<typename It>
//     using difference_type = emu::difference_type<std::iterator_traits<std::decay_t<It>>>;
//     template<typename It>
//     using iterator_category   = emu::iterator_category<std::iterator_traits<std::decay_t<It>>>;

// } // namespace iterator


// use a specific namespace name in order to avoid namespace polution with `using std::begin;`
// namespace detail_begin
// {

//     using std::begin;
//     template<typename Rg>
//     using RangeIteratorTypeImpl = decltype(begin(std::declval<Rg>()));

// } // namespace detail_begin

//     template<typename Rg>
//     using RangeIteratorType = detail_begin::RangeIteratorTypeImpl<Rg>;

//     template<typename Rg>
//     using RangeValue      = IteratorValue<RangeIteratorType<Rg>>;
//     template<typename Rg>
//     using RangeDifference = IteratorDifference<RangeIteratorType<Rg>>;
//     template<typename Rg>
//     using RangeCategory   = IteratorCategory<RangeIteratorType<Rg>>;

// namespace detail
// {

//     template<typename Container>
//     using ContainerIteratorTypeImpl = decltype(std::declval<Container>().data());

// } // namespace detail

//     template<typename Rg>
//     using ContainerIteratorType = detail::ContainerIteratorTypeImpl<Rg>;

//     template<typename Rg>
//     using ContainerValue      = IteratorValue<ContainerIteratorType<Rg>>;
//     template<typename Rg>
//     using ContainerDifference = IteratorDifference<ContainerIteratorType<Rg>>;
//     template<typename Rg>
//     using ContainerCategory   = IteratorCategory<ContainerIteratorType<Rg>>;

    // EMU_GENERATE_TRAITS_HAS(HasData, T, std::declval<T&>().data());
    // EMU_GENERATE_TRAITS_HAS(HasSize, T, std::declval<T&>().size());

    // template<typename Container>
    // constexpr auto IsContainer = HasData<Container> and HasSize<Container> and std::is_same_v<ContainerCategory<Container>, std::random_access_iterator_tag>;

namespace spe
{

    template<typename Location>
    struct is_location : std::false_type {};

} // namespace spe

    template<typename Location>
    constexpr auto is_location = spe::is_location<Location>::value;

} // namespace emu
