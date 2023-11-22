#ifndef EMU_CUDA_GRAPH_H
#define EMU_CUDA_GRAPH_H

#include <emu/cuda.h>
#include <emu/scoped.h>

namespace emu
{

namespace cuda
{

namespace graph
{

namespace handle
{

    using id_t = cudaGraph_t;

namespace detail
{
    inline id_t create() {
        id_t handle_id;
        ::cuda::throw_if_error(cudaGraphCreate(&handle_id, 0));
        return handle_id;
    }

    inline void destroy(id_t handle_id) {
        ::cuda::throw_if_error(cudaGraphDestroy(handle_id));
    }

    struct Destroyer{
        void operator()(id_t handle_id) const { destroy(handle_id); }
    };

    inline void begin_capture(::cuda::stream::id_t stream_id, cudaStreamCaptureMode mode) {
        ::cuda::throw_if_error(cudaStreamBeginCapture(stream_id, mode));
    }

    inline id_t end_capture(::cuda::stream::id_t stream_id) {
        id_t handle_id;
        ::cuda::throw_if_error(cudaStreamEndCapture(stream_id, &handle_id));
        return handle_id;
    }

} // namespace detail

    using ScopeHandle = emu::scoped_t<id_t, detail::Destroyer>;

} // namespace handle


    struct handle_t
    {
        handle_t():
            handle_id(handle::detail::create(), true)
        {}

        handle_t(handle::id_t handle_id, bool owning):
            handle_id(handle_id, owning)
        {}

        handle_t(const ::cuda::stream_t & stream):
            handle_id(handle::detail::end_capture(stream.id()), true)
        {}

        constexpr handle_t(handle_t && o) = default;
        handle_t(const handle_t &) = delete;

        handle_t& operator=(handle_t &&) = default;
        handle_t& operator=(const handle_t &) = delete;

        handle::id_t id() const noexcept { return handle_id.value; }

        ~handle_t() = default;

    private:
        handle::ScopeHandle handle_id;
    };

namespace handle
{
    inline void begin_capture(const ::cuda::stream_t & stream) {
        detail::begin_capture(stream.id(), cudaStreamCaptureModeGlobal);
    }

    inline handle_t create() {
        return {};
    }

    inline handle_t wrap(id_t handle_id, bool take_ownership) {
        return { handle_id, take_ownership };
    }

    inline handle_t end_capture(const ::cuda::stream_t & stream) {
        return {stream};
    }

} // namespace handle

namespace exec
{
    using id_t =  cudaGraphExec_t;

namespace detail
{
    inline id_t create(handle::id_t handle_id) {
        id_t exec_id;
        ::cuda::throw_if_error(cudaGraphInstantiate(&exec_id, handle_id, nullptr, nullptr, 0));
        return exec_id;
    }

    inline void destroy(id_t exec_id) {
        ::cuda::throw_if_error(cudaGraphExecDestroy(exec_id));
    }

    struct Destroyer{
        void operator()(id_t exec_id) const { destroy(exec_id); }
    };

    inline void update(id_t exec_id, handle::id_t handle_id) {
        cudaGraphExecUpdateResult updateResult;
        cudaGraphNode_t errorNode;
        ::cuda::throw_if_error(cudaGraphExecUpdate(exec_id, handle_id, &errorNode, &updateResult));
        if (updateResult != cudaGraphExecUpdateSuccess)
            fmt::print("ERROR\n");
    }

    inline void launch(id_t exec_id, ::cuda::stream::id_t stream_id) {
        ::cuda::throw_if_error(cudaGraphLaunch(exec_id, stream_id));
    }

} // namespace detail

    using ScopeExec = emu::scoped_t<id_t, detail::Destroyer>;

} // namespace exec

    struct exec_t
    {
        exec_t(exec::id_t exec_id, bool owning):
            exec_id(exec_id, owning)
        {}

        exec_t(const handle_t & handle):
            exec_id(exec::detail::create(handle.id()), true)
        {}

        constexpr exec_t(exec_t && o) = default;
        exec_t(const exec_t &) = delete;

        exec_t& operator=(exec_t &&) = default;
        exec_t& operator=(const exec_t &) = delete;

        void update(const handle_t & handle) {
            exec::detail::update(id(), handle.id());
        }

        void launch(const ::cuda::stream_t & stream) {
            exec::detail::launch(id(), stream.id());
        }

        exec::id_t id() const noexcept { return exec_id.value; }

        ~exec_t() = default;

    private:
        exec::ScopeExec exec_id;
    };

namespace exec
{
    inline exec_t create(const handle_t & handle) {
        return {handle};
    }

    inline exec_t wrap(id_t exec_id, bool take_ownership) {
        return {exec_id, take_ownership};
    }

} // namespace exec

} // namespace graph

} // namespace cuda

} // namespace emu

#endif //EMU_CUDA_GRAPH_H