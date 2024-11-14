#include <emu/cuda/error.hpp>
#include <emu/cuda.hpp>

namespace emu
{

namespace cuda
{

    const char * error_category::name() const noexcept {
        return "cuda";
    }

    std::string error_category::message( int ev ) const {
        return cu::describe(static_cast<cu::status_t>(ev));
    }

    std::error_category const& error_category::instance() {
        static const error_category instance;
        return instance;
    }

} // namespace cuda

    std::error_code make_error_code( CUresult e ) {
        return { static_cast<int>(e), cuda::error_category::instance() };
    }

    std::error_code make_error_code( cudaError_t e ) {
        return { static_cast<int>(e), cuda::error_category::instance() };
    }

    std::error_code make_error_code( ::cuda::status::named_t e ) {
        return { static_cast<int>(e), cuda::error_category::instance() };
    }

    unexpected<std::error_code> make_unexpected( CUresult e ) {
        return unexpected<std::error_code>{ make_error_code(e) };
    }

    unexpected<std::error_code> make_unexpected( cudaError_t e ) {
        return unexpected<std::error_code>{ make_error_code(e) };
    }

    unexpected<std::error_code> make_unexpected( ::cuda::status::named_t e ) {
        return unexpected<std::error_code>{ make_error_code(e) };
    }

    void throw_error( CUresult e ) {
        throw_error( make_error_code( e) );
    }
    void throw_error( cudaError_t e ) {
        throw_error( make_error_code( e) );
    }
    void throw_error( ::cuda::status::named_t e ) {
        throw_error( make_error_code( e) );
    }

} // namespace emu
