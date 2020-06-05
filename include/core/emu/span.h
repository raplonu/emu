#ifndef EMU_SPAN_H
#define EMU_SPAN_H

#include <gsl/span>
#include <emu/byte.h>

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

    /**
     * @brief std::dynamic_extent is a constant of type std::size_t that is used to differentiate std::span of static and dynamic extent.
     *
     * Please refer to https://en.cppreference.com/w/cpp/container/span/dynamic_extent for more detail.
     */
    //TODO: replace gsl::dynamic_extent by std::dynamic_extent when c++20.
    using gsl::dynamic_extent;

    //TODO: replace gsl::as_bytes by std::as_bytes when c++20.
    using gsl::as_bytes;

    //TODO: replace gsl::as_writable_bytes by std::as_writable_bytes when c++20.
    using gsl::as_writable_bytes;
}

#endif //EMU_SPAN_H