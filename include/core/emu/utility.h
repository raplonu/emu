#ifndef EMU_UTILITY_H
#define EMU_UTILITY_H

#include <emu/config.h>
#include <emu/macro.h>
#include <emu/type_traits.h>

#include <type_traits>
#include <algorithm>
#include <functional>
#include <utility>

#define EMU_FWD(v) emu::fwd<decltype(v)>(v)

namespace emu
{

namespace detail
{

    template<typename To>
    struct as_t {

        template<typename From>
        constexpr EMU_HODE To operator()(From && from) const
            EMU_NOEXCEPT_EXPR( static_cast<To>(std::forward<From>(from)) )
        {
            return static_cast<To>(std::forward<From>(from));
        }
    };

} // namespace detail

    /**
     * @brief Functional equivalent of static_cast.
     *
     * @tparam To The destination type.
     */
    template<typename To>
    EMU_HODE_CONSTEXPR auto as = detail::as_t<To>{};

    template<typename T>
    EMU_HODE constexpr T&& fwd(std::remove_reference_t<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T>
    EMU_HODE constexpr T&& fwd(std::remove_reference_t<T>&& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T> constexpr
    EMU_HODE auto mv(T&& t) noexcept -> std::remove_reference_t<T>&& {
        return static_cast<std::remove_reference_t<T>&&>(t);
    }

  //###################### SIZE ########################

#ifdef EMU_NO_CXX17

    template <class C>
    constexpr auto size(const C& c) -> decltype(c.size())
    {
        return c.size();
    }

    template <class C>
    constexpr auto ssize(const C& c)
        -> std::common_type_t<std::ptrdiff_t,
                            std::make_signed_t<decltype(c.size())>>
    {
        using R = std::common_type_t<std::ptrdiff_t,
                                    std::make_signed_t<decltype(c.size())>>;
        return static_cast<R>(c.size());
    }

    template <class T, std::size_t N>
    constexpr std::size_t size(const T (&array)[N]) noexcept
    {
        return N;
    }

    template <class T, std::ptrdiff_t N>
    constexpr std::ptrdiff_t ssize(const T (&array)[N]) noexcept
    {
        return N;
    }

#else

    using std::size;

#endif

    /**
    * Calculate the ceil result of a / b
    * @param  a numerator
    * @param  b denominator
    * @return   ceil division of a / b
    */
    template <typename T>
    EMU_HODE constexpr
    T ceil(T a, T b) noexcept { return (a + b - 1) / b; }

    template<typename T>
    EMU_HODE constexpr
    T next_mul(T a, T b) { return ( (a - 1) / b + 1) * b; }

    template <typename T>
    EMU_HODE constexpr
    T align(T size) noexcept { return next_mul(size, 32); }

    template <typename T>
    EMU_HODE constexpr
    const T & min(const T & a, const T & b) noexcept {
        return (a < b) ? a : b;
    }

    template<typename T, class Compare>
    EMU_HODE constexpr
    const T& min(const T & a, const T& b, Compare comp)
        EMU_NOEXCEPT_EXPR((comp(a, b)) ? a : b)
    {
        return (comp(a, b)) ? a : b;
    }

    template<typename T>
    EMU_HODE constexpr
    T min(std::initializer_list<T> ilist)
        EMU_NOEXCEPT_EXPR(*std::min_element(ilist.begin(), ilist.end()))
    {
        return *std::min_element(ilist.begin(), ilist.end());
    }

    template<class T, class Compare>
    EMU_HODE constexpr
    T min(std::initializer_list<T> ilist, Compare comp)
        EMU_NOEXCEPT_EXPR(*std::min_element(ilist.begin(), ilist.end(), comp))
    {
        return *std::min_element(ilist.begin(), ilist.end(), comp);
    }

    template<std::size_t key_id, typename Tuple>
    EMU_HODE constexpr
    const Tuple & min(const Tuple & t1, const Tuple & t2) noexcept {
        using std::get;
        return get<key_id>(t1) < get<key_id>(t2) ? t1 : t2;
    }

    template <typename T>
    EMU_HODE constexpr
    const T & max(const T & a, const T & b)
        EMU_NOEXCEPT_EXPR((a < b) ? b : a)
    {
        return (a < b) ? b : a;
    }

    template<typename T, class Compare>
    EMU_HODE constexpr
    const T& max(const T & a, const T& b, Compare comp)
        EMU_NOEXCEPT_EXPR((comp(a, b)) ? b : a)
    {
        return (comp(a, b)) ? b : a;
    }

    template<typename T>
    EMU_HODE constexpr
    T max(std::initializer_list<T> ilist)
        EMU_NOEXCEPT_EXPR(*std::max_element(ilist.begin(), ilist.end()))
    {
        return *std::max_element(ilist.begin(), ilist.end());
    }

    template<class T, class Compare>
    EMU_HODE constexpr
    T max(std::initializer_list<T> ilist, Compare comp)
        EMU_NOEXCEPT_EXPR(*std::max_element(ilist.begin(), ilist.end(), comp))
    {
        return *std::max_element(ilist.begin(), ilist.end(), comp);
    }

    template<std::size_t key_id, typename Tuple>
    EMU_HODE constexpr
    const Tuple & max(const Tuple & t1, const Tuple & t2) noexcept {
        using std::get;
        return get<key_id>(t1) < get<key_id>(t2) ? t2 : t1;
    }

    /**
     * @brief Return the generalized number of item per group. This number is the same for every group.
     * The sum of the result for every block may be equal or superior of item_nb
     * Esamples :
     * item_per_group_local_nb(100, 10) -> 10
     * item_per_group_local_nb(89, 10) -> 9
     * @param item_nb
     * @param group_size
     * @return int item_per_group_nb
     */
    EMU_HODE inline
    int item_per_group_nb(int item_nb, int group_size) { return ceil(item_nb, group_size); }

    /**
     * @brief Share a resource between a group. Guaranty to minimize the number of resource
     * for each group element and to handle exactly the right number of available item
     * Examples :
     * Item are perfectly shared between all groups
     * item_per_group_local_nb(100, [0, .., 9], 10) -> [10, .., 10]
     * The last group handle only 1 element to unsure item is not handled two times
     * item_per_group_local_nb(7, [0, 1, 2, 3], 4) -> [2, 2, 2, 1]
     * The last group remain idle because the is not element at all to handle
     * item_per_group_local_nb(5, [0, 1, 2, 3], 4) -> [2, 2, 1, 0]
     *
     * @param item_nb Total number of item to handle
     * @param group_id The id of the current group
     * @param group_size The number of element in the group
     * @return int item_per_group_local_nb
     */
    EMU_HODE inline
    int item_per_group_local_nb(int item_nb, int group_id, int group_size) {
        int item_pg = item_per_group_nb(item_nb, group_size);

        return max(0, min(item_nb, (group_id + 1) * item_pg) - group_id * item_pg);
    }
}

#endif //EMU_UTILITY_H