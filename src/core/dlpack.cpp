#include <emu/dlpack.hpp>

#include <gsl/pointers>

namespace emu::dlpack::detail
{

// NOLINTBEGIN(cppcoreguidelines-owning-memory)

    void managed_tensor_deleter(DLManagedTensor* dlmt) {
        auto* manager_ptr = static_cast<manager*>(dlmt->manager_ctx);
        // delete always check if manager_ptr is nullptr
        delete manager_ptr;
        delete dlmt;
    }

    void managed_tensor_versioned_deleter(DLManagedTensorVersioned* dlmtv) {
        auto* manager_ptr = static_cast<manager*>(dlmtv->manager_ctx);
        // delete always check if manager_ptr is nullptr
        delete manager_ptr;
        delete dlmtv;
    }

// NOLINTEND(cppcoreguidelines-owning-memory)

} // namespace emu::dlpack::detail
