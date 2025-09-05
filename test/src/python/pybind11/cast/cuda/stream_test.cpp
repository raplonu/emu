#include <gtest/gtest.h>
#include <emu/cuda/test/device_test.hpp>

#include <emu/pybind11/cast/cuda/stream.hpp>

#include <pybind11/embed.h> // everything needed for embedding

namespace py = pybind11;
namespace cu = emu::cuda;

namespace {

    EMU_CUDA_TEST(CastCudaStream, PythonToCppRef)
    {
        {
            auto res = py::eval("None").cast<cu::stream_ref>();
            EXPECT_FALSE(res.get());
        }
        {
            try {
                auto cp_stream = py::module_::import("cupy.cuda.stream");

                auto python_stream = cp_stream.attr("Stream")();

                auto cpp_stream = python_stream.cast<cu::stream_ref>();
                EXPECT_TRUE(cpp_stream.get());

                auto stream_ptr = python_stream.attr("ptr").cast<uintptr_t>();
                EXPECT_EQ(cpp_stream.get(), reinterpret_cast<::cudaStream_t>(stream_ptr));

            } catch (const py::error_already_set &e) {
                GTEST_SKIP() << "Could not test cupy: " << e.what();
            }
        }
        {
            try {
                auto cu_runtime = py::module_::import("cuda.bindings.runtime");

                ::pybind11::object python_stream = cu_runtime.attr("cudaStreamCreate")()[py::int_(1)];

                auto cpp_stream = python_stream.cast<cu::stream_ref>();

                EXPECT_TRUE(cpp_stream.get());

                auto stream_ptr = python_stream.cast<uintptr_t>();
                EXPECT_EQ(cpp_stream.get(), reinterpret_cast<::cudaStream_t>(stream_ptr));

                cu_runtime.attr("cudaStreamDestroy")(python_stream);

            } catch (const py::error_already_set &e) {
                GTEST_SKIP() << "Could not test cuda: " << e.what();
            }
        }


    }

    EMU_CUDA_TEST(CastCudaStream, CppRefToPython)
    {
        try {
            auto stream = cu::stream::create();

            {

                cu::stream_ref ref = stream;
                pybind11::object py_stream = py::cast(ref);

                EXPECT_FALSE(py_stream.is_none());

                EXPECT_EQ(
                    reinterpret_cast<cu::stream::handle_t>(py_stream.cast<uintptr_t>()),
                    ref.get()
                );
            }

            {
                pybind11::object py_stream = py::cast(stream);

                EXPECT_FALSE(py_stream.is_none());

                EXPECT_EQ(
                    reinterpret_cast<cu::stream::handle_t>(py_stream.cast<uintptr_t>()),
                    stream.get()
                );
            }
        } catch (const py::error_already_set &e) {
            GTEST_SKIP() << "Could not test cuda: " << e.what();
        }

    }

} // namespace
