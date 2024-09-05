#include <emu/error.hpp>

namespace emu
{

    const char * error_category::name() const noexcept {
        return "emu";
    }

    std::string error_category::message( int ev ) const {
        switch (static_cast<error>(ev)) {
            case error::success: return "success";

            case error::dlpack_rank_mismatch: return "dlpack rank mismatch";
            case error::dlpack_type_mismatch: return "dlpack type mismatch";
            case error::dlpack_strides_not_supported: return "dlpack strides not supported";

            case error::pointer_device_not_found: return "pointer's device not found";
            case error::pointer_maps_file_not_found: return "pointer's maps file not found";

        }
        return "unknown";
    }

    const std::error_category& error_category::instance() {
        static const error_category instance;
        return instance;
    }

    std::error_code make_error_code( error e ) {
        return { static_cast<int>(e), error_category::instance() };
    }

    unexpected<std::error_code> make_unexpected( error e ) {
        return unexpected<std::error_code>{ make_error_code(e) };
    }

    unexpected<std::error_code> make_unexpected( std::errc e ) {
        return unexpected<std::error_code>{ std::make_error_code(e) };
    }

    void throw_error( error e ) {
        throw std::system_error( make_error_code(e) );
    }

    void throw_error( std::errc e ) {
        throw std::system_error( std::make_error_code(e) );
    }

} // namespace emu
