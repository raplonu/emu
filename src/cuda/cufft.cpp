#include <emu/cufft.h>
#include <emu/cufft/error.h>

namespace emu
{

namespace cufft
{

    template<typename In, typename Out> struct CuFFT;

    template<>
    struct CuFFT<float, cuFloatComplex> {
        constexpr static auto exec = cufftExecR2C;
        constexpr static auto type = CUFFT_R2C;
    };

    template<>
    struct CuFFT<double, cufftDoubleComplex> {
        constexpr static auto exec = cufftExecD2Z;
        constexpr static auto type = CUFFT_D2Z;
    };

    template<>
    struct CuFFT<cuFloatComplex, float> {
        constexpr static auto exec = cufftExecC2R;
        constexpr static auto type = CUFFT_C2R;
    };

    template<>
    struct CuFFT<cufftDoubleComplex, double> {
        constexpr static auto exec = cufftExecZ2D;
        constexpr static auto type = CUFFT_Z2D;
    };

    template<>
    struct CuFFT<cuFloatComplex, cuFloatComplex> {
        constexpr static auto exec = cufftExecC2C;
        constexpr static auto type = CUFFT_C2C;
    };

    template<>
    struct CuFFT<cufftDoubleComplex, cufftDoubleComplex> {
        constexpr static auto exec = cufftExecZ2Z;
        constexpr static auto type = CUFFT_Z2Z;
    };

    template<typename In, typename Out>
    cufftType_t type() {
        return CuFFT<In, Out>::type;
    }

    template Type type<float,              cuFloatComplex    >();
    template Type type<double,             cufftDoubleComplex>();
    template Type type<cuFloatComplex,     float             >();
    template Type type<cufftDoubleComplex, double            >();
    template Type type<cuFloatComplex,     cuFloatComplex    >();
    template Type type<cufftDoubleComplex, cufftDoubleComplex>();

    template<typename In, typename Out>
    void exec(const handle_t & handle, const In * in, Out * out, Direction dir) {
        throw_if_error(CuFFT<In, Out>::exec(handle.enable().id(), in, out, convert(dir)));
    }

    template void exec<float,              cuFloatComplex    >(const handle_t & handle, const float              * in, cuFloatComplex     * out, Direction dir);
    template void exec<double,             cufftDoubleComplex>(const handle_t & handle, const double             * in, cufftDoubleComplex * out, Direction dir);
    template void exec<cuFloatComplex,     float             >(const handle_t & handle, const cuFloatComplex     * in, float              * out, Direction dir);
    template void exec<cufftDoubleComplex, double            >(const handle_t & handle, const cufftDoubleComplex * in, double             * out, Direction dir);
    template void exec<cuFloatComplex,     cuFloatComplex    >(const handle_t & handle, const cuFloatComplex     * in, cuFloatComplex     * out, Direction dir);
    template void exec<cufftDoubleComplex, cufftDoubleComplex>(const handle_t & handle, const cufftDoubleComplex * in, cufftDoubleComplex * out, Direction dir);

} // namespace cufft

} // namespace emu
