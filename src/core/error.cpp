#include <emu/error.hpp>
#include <system_error>

namespace emu
{

    std::string_view describe(errc e) {
        switch (e) {
            case errc::success:
                return "success";

            case errc::dlpack_rank_mismatch:
                return "dlpack rank mismatch";
            case errc::dlpack_type_mismatch:
                return "dlpack type mismatch";
            case errc::dlpack_strides_not_supported:
                return "dlpack strides not supported";
            case errc::dlpack_read_only:
                return "dlpack trying to access read only memory";
            case errc::dlpack_unkown_device_type:
                return "dlpack unknow device type";
            case errc::dlpack_unkown_data_type_code:
                return "dlpack unknow data type code";
            case errc::dlpack_bits_too_large:
                return "dlpack bits too large to handle";

            case errc::numeric_dl_data_type_not_supported:
                return "numeric dlpack data type not supported";

            case errc::pointer_desc_not_found:
                return "pointer's descriptor not found";
            case errc::pointer_device_not_found:
                return "pointer's device not found";
            case errc::pointer_maps_file_not_found:
                return "pointer's maps file not found";
            case errc::pointer_parsing_error:
                return "pointer's parsing error";

            case errc::cuda_pointer_unregistered:
                return "cuda pointer is unregistered";

            case errc::not_implemented:
                return "not implemented";
        }
        return "unknown";
    }

    unexpected<std::error_code> make_unexpected( std::error_code e ) {
        return unexpected<std::error_code>{ e };
    }

    unexpected<std::error_code> make_unexpected( std::errc e ) {
        return make_unexpected( std::make_error_code(e) );
    }

    void throw_error( std::error_code e ) {
        throw std::system_error( e );
    }

    void throw_error( std::error_code e, const std::string& what_arg ) {
        throw std::system_error( e, what_arg );
    }

    void throw_error( std::errc e ) {
        throw_error( std::make_error_code(e) );
    }

    void throw_error( std::errc e, const std::string& what_arg ) {
        throw_error( std::make_error_code(e), what_arg );
    }

} // namespace emu
