#pragma once

#include "type_traits.hpp"
#include <emu/expected.hpp>

#include <stdexcept>
#include <system_error>

namespace emu
{

    struct error_category: public std::error_category
    {
        const char * name() const noexcept;
        std::string message( int ev ) const;

        static const std::error_category& instance();
    };

    enum class error
    {
        success = 0,

        dlpack_rank_mismatch,
        dlpack_type_mismatch,
        dlpack_strides_not_supported,

        pointer_device_not_found,
        pointer_maps_file_not_found,
    };

    /**
     * @brief Return a std::error_code from a emu::error
     *
     * @param e The emu::error
     * @return The std::error_code
     */
    std::error_code make_error_code( error e );

    /**
     * @brief Return a std::error_code from a emu::error
     *
     * @param e The emu::error
     * @return The std::error_code
     */
    unexpected<std::error_code> make_unexpected( error e );
    unexpected<std::error_code> make_unexpected( std::errc e );

    void throw_error( error e );
    void throw_error( std::errc e );


    /**
     * @brief This is a helper type that is used to bypass the optional/expected limitation of not being able to hold a reference.
     *
     * @tparam T
     */
    template<typename T>
    using return_type = std::conditional_t<is_ref<T>, std::reference_wrapper<rm_ref<T>>, T>;

    template<typename T>
    using result = expected<return_type<T>, std::error_code>;

} // namespace emu

#define EMU_TRUE_OR_RETURN_UN_EC(expr, error_code) \
    do { using ::emu::make_unexpected; if ( not (expr) ) return make_unexpected(error_code); } while (false)

#define EMU_TRUE_OR_THROW_ERROR(expr, error_code) \
    do { using ::emu::throw_error; if ( not (expr) ) throw_error(error_code); } while (false)

template <>
struct std::is_error_code_enum< emu::error > : std::true_type {};
