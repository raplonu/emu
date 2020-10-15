#ifndef EMU_CUSOLVER_HANDLE_H
#define EMU_CUSOLVER_HANDLE_H

#include <emu/cusolver/error.h>

#include <cuda/api_wrappers.hpp>

#include <emu/scoped.h>
#include <emu/device.h>

namespace emu
{

namespace cusolver
{

class handle_t;

namespace handle
{
    using id_t = cusolverDnHandle_t;

    namespace detail
    {
        id_t create();

        void destroy(id_t id);

        struct Destroyer{
            void operator()(id_t id) const { destroy(id); }
        };
    }

    using ScopedHandle = emu::scoped_t<const id_t, detail::Destroyer>;
}

struct handle_t
{
    handle_t();

    handle_t(handle::id_t id, cuda::device::id_t device_id, bool owning);

    handle_t(cuda::device::id_t device_id);

    constexpr handle_t(handle_t && o) = default;
    handle_t(const handle_t &) = delete;

    handle_t& operator=(handle_t &&) = default;
    handle_t& operator=(const handle_t &) = delete;

    handle::id_t id() const noexcept { return id_.value; }

    ~handle_t() = default;

private:
    handle::ScopedHandle id_;
    cuda::device::id_t device_id_;
};


namespace handle
{
    /**
     * Create cusolver handle_t on current device.
     */
    handle_t create();

    /**
     * Create cusolver handle_t on current device.
     */
    handle_t create(cuda::device_t device);

    handle_t wrap(id_t id, cuda::device_t device, bool take_ownership);
}

}

}

#endif //EMU_CUSOLVER_HANDLE_H