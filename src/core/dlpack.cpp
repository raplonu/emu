#include <emu/dlpack.hpp>

namespace emu::dlpack::detail
{

    void managed_tensor_deleter(DLManagedTensor* dlmt) {
        auto* manager_ptr = static_cast<manager*>(dlmt->manager_ctx);
        // always check if manager_ptr is nullptr
        if (manager_ptr) delete manager_ptr;
        delete dlmt;
    }

    void managed_tensor_versioned_deleter(DLManagedTensorVersioned* dlmtv) {
        auto* manager_ptr = static_cast<manager*>(dlmtv->manager_ctx);
        // always check if manager_ptr is nullptr
        if (manager_ptr) delete manager_ptr;
        delete dlmtv;
    }

} // namespace emu::dlpack::detail
