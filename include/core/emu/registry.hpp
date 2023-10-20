#pragma once

#include <emu/macro.h>

#include <unordered_map>

namespace emu
{
    template<typename T>
    struct registry {

        using factory_type = std::function<T()>;

        using map_type = std::unordered_map<std::string, factory>;

        factory_type factory;

        // should be implemented in .cpp
        static map_type& entries();
        // {
        //     static map_type instance;
        //     return instance;
        // }

        static void register_entry(std::string name, factory_type factory) {
            entries().emplace(name, factory);
        }

        static factory_type at(std::string name) {
            return entries().at(name);
        }

        static std::vector<std::string> entry_names() {
            std::vector<std::string> names;
            for (auto& entry : entries())
                names.push_back(entry.first);
            return names;
        }
    };

} // namespace emu



#define EMU_REGISTRY_DEFINITION(REGISTRY_NAME, TYPE) \
using REGISTRY_NAME = ::emu::registry<TYPE>;

#define EMU_REGISTRY_IMPL(TYPE) \
template <> \
::emu::registry<TYPE>::map_type& ::emu::registry<TYPE>::entries() { \
    static ::emu::registry<TYPE>::map_type instance; \
    return instance; \
}

#define EMU_REGISTER_IMPL(NAME, TYPE) \
namespace emu{ namespace{ \
    struct EMU_CONCAT_3(module_register, TYPE, NAME){ \
        EMU_CONCAT_3(module_register, TYPE, NAME)(){ \
            ::emu::registry<TYPE>::register_entry(BOOST_PP_STRINGIZE(NAME), [](){ return factory(); }); \
        } \
        static inline TYPE factory(); \
    } EMU_CONCAT_3(instance_module_register, TYPE, NAME); \
}} \
TYPE \
emu::EMU_CONCAT_3(module_register, TYPE, NAME)::factory()
