#include <emu/cufft/error.h>

namespace emu
{

namespace cufft
{

    std::string describe(status_t error_code) {
        using namespace std::string_literals;

        switch (status(error_code))
        {
            case status::success:
                return "Success";
            case status::invalid_plan:
                return "Invalid plan";
            case status::alloc_failed:
                return "Alloc failed";
            case status::invalid_type:
                return "Invalid type";
            case status::invalid_value:
                return "Invalid value";
            case status::internal_error:
                return "Internal error";
            case status::exec_failed:
                return "Exec failed";
            case status::setup_failed:
                return "Setup failed";
            case status::invalid_size:
                return "Invalid size";
            case status::unaligned_data:
                return "Unaligned data";
            case status::incomplete_parameter_list:
                return "Incomplete parameter list";
            case status::invalid_device:
                return "Invalid device";
            case status::parse_error:
                return "Parse error";
            case status::no_workspace:
                return "No workspace";
            case status::not_implemented:
                return "Not implemented";
            case status::license_error:
                return "License error";
            case status::not_supported:
                return "Not supported";
            default:
                return "unknow error"s;
        }

    }

} // namespace cufft

} // namespace emu
