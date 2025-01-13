#pragma once

#include <emu/scoped.hpp>

#include <Python.h>

namespace emu::pybind11
{

namespace detail {

    struct buffer_deleter {

        void operator()(Py_buffer buffer) const noexcept {
            PyBuffer_Release(&buffer);
        }
    };

    using scoped_buffer = scoped<Py_buffer, buffer_deleter>;
} // namespace detail


    struct buffer
    {
        buffer() = default;


        [[nodiscard]] Py_buffer&       view()       { return buffer_.value; }
        [[nodiscard]] const Py_buffer& view() const { return buffer_.value; }


    private:
        detail::scoped_buffer buffer_;
    };

} // namespace emu::pybind11
