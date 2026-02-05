#include <gtest/gtest.h>
#include <emu/cuda/test/device_test.hpp>

#include <emu/pybind11/cast/cuda/event.hpp>

#include <pybind11/embed.h> // everything needed for embedding

#include <bit>

namespace py = pybind11;
namespace cu = emu::cuda;

namespace {

    EMU_CUDA_TEST(CastCudaEvent, PythonToCppRef)
    {
        {
            auto res = py::eval("None").cast<cu::event_ref>();
            EXPECT_FALSE(res.get());
        }
        {
            try {
                auto cp_stream = py::module_::import("cupy.cuda.stream");

                auto python_event = cp_stream.attr("Event")();

                auto cpp_event = python_event.cast<cu::event_ref>();
                EXPECT_TRUE(cpp_event.get());

                auto event_ptr = python_event.attr("ptr").cast<uintptr_t>();
                EXPECT_EQ(cpp_event.get(), std::bit_cast<cu::event_id>(event_ptr));

            } catch (const py::error_already_set &e) {
                GTEST_SKIP() << "Could not test cupy: " << e.what();
            }
        }
        {
            try {
                auto cu_runtime = py::module_::import("cuda.bindings.runtime");

                const ::pybind11::object python_event = cu_runtime.attr("cudaEventCreate")()[py::int_(1)];

                auto cpp_event = python_event.cast<cu::event_ref>();

                EXPECT_TRUE(cpp_event.get());

                auto event_ptr = python_event.cast<uintptr_t>();
                EXPECT_EQ(cpp_event.get(), std::bit_cast<cu::event_id>(event_ptr));

                cu_runtime.attr("cudaEventDestroy")(python_event);

            } catch (const py::error_already_set &e) {
                GTEST_SKIP() << "Could not test cuda: " << e.what();
            }
        }


    }

    EMU_CUDA_TEST(CastCudaEvent, CppRefToPython)
    {
        try {
            auto event = cu::event_handle(cu::current_device());

            {

                const cu::event_ref& ref = event;
                const pybind11::object py_event = py::cast(ref);

                EXPECT_FALSE(py_event.is_none());

                EXPECT_EQ(
                    std::bit_cast<cu::event_id>(py_event.cast<uintptr_t>()),
                    ref.get()
                );
            }

            {
                const pybind11::object py_event = py::cast(event);

                EXPECT_FALSE(py_event.is_none());

                EXPECT_EQ(
                    std::bit_cast<cu::event_id>(py_event.cast<uintptr_t>()),
                    event.get()
                );
            }

        } catch (const py::error_already_set &e) {
            GTEST_SKIP() << "Could not test cuda: " << e.what();
        }

    }

} // namespace
