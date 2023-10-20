#pragma once

#include <unordered_map>

namespace emu
{

    struct plugin
    {
        plugin() = default;

        ~plugin() = default;


        template<typename T>
        std::unordered_map<std::string, T> load() {
            return {};
        }

    };

} // namespace emu
