#ifndef EMU_STRING_H
#define EMU_STRING_H

#include <emu/macro.h>
#include <emu/utility.h>

#include <string>

namespace emu
{
    using string_ref  =       std::string &;
    using string_cref = const std::string &;
} // namespace emu


#if EMU_STRING_UTIL

#include <absl/strings/string_view.h>
#include <absl/strings/str_split.h>
#include <absl/strings/str_join.h>

namespace emu
{

    using absl::string_view;

    template<typename... Args>
    constexpr auto split(Args&&... args) EMU_NOEXCEPT_EXPR(absl::StrSplit(EMU_FWD(args)...)) {
        return absl::StrSplit(EMU_FWD(args)...);
    }

    template<typename... Args>
    constexpr auto join(Args&&... args) EMU_NOEXCEPT_EXPR(absl::StrJoin(EMU_FWD(args)...)) {
        return absl::StrJoin(EMU_FWD(args)...);
    }


} // namespace emu

#endif //EMU_STRING_UTIL

#endif //EMU_STRING_H