#ifndef EMU_TYPE_TRAITS_H
#define EMU_TYPE_TRAITS_H

#include <type_traits>
namespace emu
{
    template<typename It>
    using IteratorValue = typename std::iterator_traits<It>::value_type;

}

#endif //EMU_TYPE_TRAITS_H