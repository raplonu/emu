#ifndef EMU_BYTE_H
#define EMU_BYTE_H

#include <gsl/gsl_byte>

namespace emu
{
    /**
     * @brief emu::byte is a distinct type that implements the concept of byte as specified in the C++ language definition.
     *
     * Like byte and unsigned byte, it can be used to access raw memory occupied by other objects (object representation), but
     * unlike those types, it is not a byteacter type and is not an arithmetic type. A byte is only a collection of bits,
     * and the only operators defined for it are the bitwise ones.
     *
     * Please refer to https://en.cppreference.com/w/cpp/types/byte for more detail.
     */
    //TODO: replace gsl::byte by std::byte when c++17.
    using gsl::byte;

} // namespace emu

#endif //EMU_BYTE_H