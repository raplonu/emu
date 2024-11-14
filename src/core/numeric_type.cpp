#include <emu/numeric_type.hpp>

namespace emu
{

    // result<data_type_t> from_dl_data_type(dlpack::data_type_t dtype) {
    //     auto bits = dtype.bits;
    //     switch (dtype.code)
    //     {
    //         case dlpack::data_type_code_t::kDLBool:
    //             EMU_ASSERT_ASSUME_MSG(bits == 8, "Bool should be 8 bits"); // Not even an error, should never happen.
    //             return data_type_t::boolean;
    //         case dlpack::data_type_code_t::kDLInt:
    //             switch (bits)
    //             {
    //                 case 8: return data_type_t::int8;
    //                 case 16: return data_type_t::int16;
    //                 case 32: return data_type_t::int32;
    //                 case 64: return data_type_t::int64;
    //                 default:
    //                     EMU_COLD_LOGGER("Unsupported bits for integer type: {}", bits);
    //                     return make_unexpected(errc::numeric_dl_data_type_not_supported);
    //             }
    //         case dlpack::data_type_code_t::kDLUInt:
    //             switch (bits)
    //             {
    //                 case 8: return data_type_t::uint8;
    //                 case 16: return data_type_t::uint16;
    //                 case 32: return data_type_t::uint32;
    //                 case 64: return data_type_t::uint64;
    //                 default:
    //                     EMU_COLD_LOGGER("Unsupported bits for unsigned integer type: {}", bits);
    //                     return make_unexpected(errc::numeric_dl_data_type_not_supported);
    //             }
    //         case dlpack::data_type_code_t::kDLFloat:
    //             switch (bits)
    //             {
    //                 case 16: return data_type_t::float16;
    //                 case 32: return data_type_t::float32;
    //                 case 64: return data_type_t::float64;
    //                 case 128: return data_type_t::float128;
    //                 default:
    //                     EMU_COLD_LOGGER("Unsupported bits for float type: {}", bits);
    //                     return make_unexpected(errc::numeric_dl_data_type_not_supported);
    //             }
    //         case dlpack::data_type_code_t::kDLComplex:
    //             switch (bits)
    //             {
    //                 case 32: return data_type_t::complex32;
    //                 case 64: return data_type_t::complex64;
    //                 case 128: return data_type_t::complex128;
    //                 default:
    //                     EMU_COLD_LOGGER("Unsupported bits for complex type: {}", bits);
    //                     return make_unexpected(errc::numeric_dl_data_type_not_supported);
    //             }
    //         default:
    //             EMU_COLD_LOGGER("Unsupported data type code: {}", static_cast<int>(dtype.code));
    //             return make_unexpected(errc::numeric_dl_data_type_not_supported);
    //     }
    // }

    // dlpack::data_type_t to_dl_data_type(data_type_t dtype) {
    //     switch (dtype)
    //     {
    //         case data_type_t::boolean: return { .code = dlpack::data_type_code_t::kDLBool, .bits = 8, .lanes = 1 };
    //         case data_type_t::int8: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 8, .lanes = 1 };
    //         case data_type_t::int16: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 16, .lanes = 1 };
    //         case data_type_t::int32: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 32, .lanes = 1 };
    //         case data_type_t::int64: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 64, .lanes = 1 };
    //         case data_type_t::uint8: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 8, .lanes = 1 };
    //         case data_type_t::uint16: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 16, .lanes = 1 };
    //         case data_type_t::uint32: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 32, .lanes = 1 };
    //         case data_type_t::uint64: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 64, .lanes = 1 };
    //         case data_type_t::float16: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 16, .lanes = 1 };
    //         case data_type_t::float32: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 32, .lanes = 1 };
    //         case data_type_t::float64: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 64, .lanes = 1 };
    //         case data_type_t::float128: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 128, .lanes = 1 };
    //         case data_type_t::complex32: return { .code = dlpack::data_type_code_t::kDLComplex, .bits = 32, .lanes = 1 };
    //         case data_type_t::complex64: return { .code = dlpack::data_type_code_t::kDLComplex, .bits = 64, .lanes = 1 };
    //         case data_type_t::complex128: return { .code = dlpack::data_type_code_t::kDLComplex, .bits = 128, .lanes = 1 };
    //         case data_type_t::complex256: throw_error(errc::dlpack_bits_too_large); // We decided to throw because of the rarity of this case.
    //         case data_type_t::bfloat16: return { .code = dlpack::data_type_code_t::kDLBfloat, .bits = 16, .lanes = 1 };
    //         default:
    //             EMU_UNREACHABLE;
    //     }
    // }

    // dlpack::data_type_ext_t to_dl_data_type_ext(data_type_t dtype) {
    //     switch (dtype)
    //     {
    //         case data_type_t::boolean: return { .code = dlpack::data_type_code_t::kDLBool, .bits = 8, .lanes = 1 };
    //         case data_type_t::int8: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 8, .lanes = 1 };
    //         case data_type_t::int16: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 16, .lanes = 1 };
    //         case data_type_t::int32: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 32, .lanes = 1 };
    //         case data_type_t::int64: return { .code = dlpack::data_type_code_t::kDLInt, .bits = 64, .lanes = 1 };
    //         case data_type_t::uint8: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 8, .lanes = 1 };
    //         case data_type_t::uint16: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 16, .lanes = 1 };
    //         case data_type_t::uint32: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 32, .lanes = 1 };
    //         case data_type_t::uint64: return { .code = dlpack::data_type_code_t::kDLUInt, .bits = 64, .lanes = 1 };
    //         case data_type_t::float16: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 16, .lanes = 1 };
    //         case data_type_t::float32: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 32, .lanes = 1 };
    //         case data_type_t::float64: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 64, .lanes = 1 };
    //         case data_type_t::float128: return { .code = dlpack::data_type_code_t::kDLFloat, .bits = 128, .lanes = 1 };
    //         case data_type_t::complex32: return { .code = dlpack::data_type_code_t::kDLComplex, .bits = 32, .lanes = 1 };
    //         case data_type_t::complex64: return { .code = dlpack::data_type_code_t::kDLComplex, .bits = 64, .lanes = 1 };
    //         case data_type_t::complex128: return { .code = dlpack::data_type_code_t::kDLComplex, .bits = 128, .lanes = 1 };
    //         case data_type_t::complex256: return { .code = dlpack::data_type_code_t::kDLComplex, .bits = 256, .lanes = 1 };
    //         case data_type_t::bfloat16: return { .code = dlpack::data_type_code_t::kDLBfloat, .bits = 16, .lanes = 1 };
    //         default:
    //             EMU_UNREACHABLE;
    //     }
    // }

} // namespace emu
