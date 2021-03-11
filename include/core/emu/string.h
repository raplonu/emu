#ifndef EMU_STRING_H
#define EMU_STRING_H

#include <emu/macro.h>
#include <emu/utility.h>

#include <absl/strings/string_view.h>
#include <absl/strings/str_split.h>
#include <absl/strings/str_join.h>

namespace emu
{

    using absl::string_view;

    template<typename... Args>
    constexpr auto split(Args&&... args) EMU_NOEXCEPT_EXPR(absl::StrSplit(FWD(args)...)) {
        return absl::StrSplit(FWD(args)...);
    }

    template<typename... Args>
    constexpr auto join(Args&&... args) EMU_NOEXCEPT_EXPR(absl::StrJoin(FWD(args)...)) {
        return absl::StrJoin(FWD(args)...);
    }

} // namespace emu

#endif //EMU_STRING_H