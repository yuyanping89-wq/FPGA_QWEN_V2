#ifndef COMMON_NUMERIC_V2_H
#define COMMON_NUMERIC_V2_H

#include <stdint.h>

// ABI V2 floating-point boundary policy:
//   * finite FP32 -> BF16 uses round-to-nearest, ties-to-even;
//   * finite overflow caused by rounding becomes signed infinity;
//   * underflow is gradual: values round to a BF16 subnormal or signed zero;
//   * infinities preserve their sign;
//   * every NaN is canonicalized to a quiet NaN while preserving its sign.
// All HLS modules and their software references must use these functions.
namespace common_numeric_v2 {

static inline uint32_t fp32_to_bits(float value) {
#pragma HLS INLINE
    union {
        float f;
        uint32_t u;
    } cvt;
    cvt.f = value;
    return cvt.u;
}

static inline float fp32_from_bits(uint32_t bits) {
#pragma HLS INLINE
    union {
        float f;
        uint32_t u;
    } cvt;
    cvt.u = bits;
    return cvt.f;
}

static inline bool fp32_is_nan(float value) {
#pragma HLS INLINE
    const uint32_t bits = fp32_to_bits(value);
    return (bits & 0x7f800000u) == 0x7f800000u &&
           (bits & 0x007fffffu) != 0u;
}

static inline bool fp32_is_inf(float value) {
#pragma HLS INLINE
    return (fp32_to_bits(value) & 0x7fffffffu) == 0x7f800000u;
}

static inline bool fp32_signbit(float value) {
#pragma HLS INLINE
    return (fp32_to_bits(value) & 0x80000000u) != 0u;
}

static inline float canonical_nan_fp32(bool negative = false) {
#pragma HLS INLINE
    return fp32_from_bits((negative ? 0x80000000u : 0u) | 0x7fc00000u);
}

static inline float signed_zero_fp32(bool negative) {
#pragma HLS INLINE
    return fp32_from_bits(negative ? 0x80000000u : 0u);
}

static inline float bf16_to_fp32(uint16_t value) {
#pragma HLS INLINE
    if ((value & 0x7f80u) == 0x7f80u && (value & 0x007fu) != 0u) {
        return canonical_nan_fp32((value & 0x8000u) != 0u);
    }
    return fp32_from_bits((uint32_t)value << 16);
}

static inline uint16_t fp32_to_bf16_rne(float value) {
#pragma HLS INLINE
    const uint32_t bits = fp32_to_bits(value);
    const uint32_t sign = bits & 0x80000000u;
    const uint32_t exponent = bits & 0x7f800000u;
    const uint32_t mantissa = bits & 0x007fffffu;

    if (exponent == 0x7f800000u) {
        if (mantissa == 0u) {
            return (uint16_t)(bits >> 16);
        }
        return (uint16_t)((sign >> 16) | 0x7fc0u);
    }

    const uint32_t rounded = bits + 0x7fffu + ((bits >> 16) & 1u);
    return (uint16_t)(rounded >> 16);
}

}  // namespace common_numeric_v2

#endif  // COMMON_NUMERIC_V2_H
