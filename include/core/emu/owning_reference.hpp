// #pragma once

// #include <emu/capsule.hpp>

// namespace emu
// {

//     template<typename T>
//     struct owning_reference : std::reference_wrapper<T>, emu::capsule
//     {
//         using base_t = std::reference_wrapper<T>;
//         using capsule_t = emu::capsule;

//         using type = typename base_t::type;

//         template< class U >
//             requires (not same_as<T, owning_reference>)
//                 and std::constructible_from<owning_reference, T>
//         constexpr owning_reference( U&& x ) noexcept(/*see below*/) ;

//         constexpr owning_reference( const owning_reference& ) noexcept = default;
//         constexpr owning_reference( owning_reference&& ) noexcept = default;

//         friend constexpr bool operator== ( const owning_reference& lhs, const owning_reference& rhs );
//         friend constexpr bool operator== ( const owning_reference& lhs, const owning_reference<const T>& rhs );
//         friend constexpr bool operator== ( const owning_reference& lhs, const T& ref );
//         friend constexpr auto operator<=>( const owning_reference& lhs, const owning_reference& rhs );
//         friend constexpr auto operator<=>( const owning_reference& lhs, const owning_reference<const T>& rhs );
//         friend constexpr auto operator<=>( const owning_reference& lhs, const T& ref );
//     };

// } // namespace emu
