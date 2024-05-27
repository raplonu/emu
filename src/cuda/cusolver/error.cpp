
#include <emu/cusolver/error.hpp>

namespace emu::cusolver
{

std::string describe(status_t error_code)
{
    using namespace std::string_literals;

    switch (status(error_code))
    {
        case status::success:
            return "success"s;

        case status::not_initialized:
            return "library not initialized"s;

        case status::alloc_failed:
            return "resource allocation failed"s;

        case status::invalid_value:
            return "unsupported value"s;

        case status::arch_mismatch:
            return "feature absent from the device architecture"s;

        case status::execution_failed:
            return "program failed to execute"s;

        case status::internal_error:
            return "internal operation failed"s;

        case status::not_supported:
            return "functionality requested is not supported"s;

        default:
            return "unknow error"s;
    }

}

} // namespace emu::cusolver
