#include <emu/type_name.hpp>

#include <boost/core/demangle.hpp>


namespace emu::detail
{

    std::string demangle(const char* type_name) {
        return boost::core::demangle(type_name);
    }

    std::string demangle(const std::type_info& type_info) {
        return demangle(type_info.name());
    }

} // namespace emu::detail
