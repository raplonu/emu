#ifndef EMU_SPAN_H
#define EMU_SPAN_H

#include <gsl/span>

namespace emu
{
    /**
     * @brief The class template span describes an object that can refer to a contiguous sequence of objects
     * with the first element of the sequence at position zero.
     *
     * A span can either have a static extent, in which case the number of elements in the sequence is known
     * and encoded in the type, or a dynamic extent.
     * Please refer to https://en.cppreference.com/w/cpp/container/span for more detail.
     *
     * @note Use span from gsl library since it is the closest implementation of c++20 std::span.
     */
    //TODO: replace gsl::span by std::span when c++20.
    using gsl::span;

}

#endif //EMU_SPAN_H