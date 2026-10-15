#ifndef FPGA_OG32_LINEAR_V2_H_
#define FPGA_OG32_LINEAR_V2_H_

/* Burst-friendly BF16 linear core for W_OG32_I_L32_V2 packed weights, shared
 * by M07 (QKV) and M11 (AttnOut).
 *
 * Packed weight beat order is [g][i][c] with 8 BF16 lanes per 128-bit beat,
 * so one sequential pass over the weight buffer streams every group exactly
 * once.  Each beat holds outputs o = g*32 + c*8 + lane for input i.
 *
 *   - Weights: one linear burst read per row block (II=1, 8 lanes/beat).
 *   - Rows: OG32_ROW_PAR rows share every weight beat, so Prefill re-reads
 *     the weights once per OG32_ROW_PAR rows instead of once per row.
 *   - Accumulation: per output, FP32 multiply then add in ascending input
 *     order starting from 0.0f, identical to the scalar reference, so results
 *     stay bit-exact.  The four accumulators of one lane (c = 0..3) form a
 *     4-deep rotating register; consecutive updates of the same output are
 *     4 iterations apart, which lets the FP32 add pipeline at II=1.
 */
#include <stdint.h>
#include "fpga_axi128.h"

#ifndef OG32_ROW_PAR
#define OG32_ROW_PAR 4
#endif
#define OG32_RB_MAX (64 / OG32_ROW_PAR)
#define OG32_LANES 8  /* BF16 per 128-bit beat */

static inline float og32_b2f(uint16_t v) {
    union { uint32_t u; float f; } bits;
    bits.u = ((uint32_t)v) << 16;
    return bits.f;
}

static inline uint16_t og32_f2b(float f) {
    union { float f; uint32_t u; } bits;
    bits.f = f;
    const uint32_t lsb = (bits.u >> 16) & 1U;
    return (uint16_t)((bits.u + 0x7FFFU + lsb) >> 16);
}

/* x[T][I] (beats) -> xbuf[row % ROW_PAR][row / ROW_PAR][i / 8]. */
template <int I>
static void og32_load_x(const fpga_axi128_t *x,
                        fpga_axi128_t xbuf[OG32_ROW_PAR][OG32_RB_MAX][I / OG32_LANES],
                        uint32_t rows) {
    const uint32_t n = rows * (uint32_t)(I / OG32_LANES);
    uint32_t r = 0, ib = 0;
load_x:
    for (uint32_t t = 0; t < n; ++t) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=112 max=7168
        xbuf[r % OG32_ROW_PAR][r / OG32_ROW_PAR][ib] = x[t];
        if (++ib == (uint32_t)(I / OG32_LANES)) { ib = 0; ++r; }
    }
}

/* One row block: FP32 sums in obuf; output writers add bias and round BF16. */
template <int I, int G, bool HAS_BIAS>
static void og32_compute_block(const fpga_axi128_t *w,
                               const fpga_axi128_t xbuf[OG32_ROW_PAR][OG32_RB_MAX][I / OG32_LANES],
                               uint32_t rb, const float *bias,
                               float obuf[OG32_ROW_PAR][G * 32]) {
    float acc[OG32_ROW_PAR][OG32_LANES][4];
#pragma HLS ARRAY_PARTITION variable=acc complete dim=0
    const uint32_t NB = (uint32_t)G * (uint32_t)I * 4U;
    uint32_t g = 0, i = 0;
weight_stream:
    for (uint32_t wb = 0; wb < NB; ++wb) {
#pragma HLS PIPELINE II=1
        const fpga_axi128_t wbeat = w[wb];
        const uint32_t c = wb & 3U;
        const bool first = (i == 0U);
        const bool last = (i == (uint32_t)I - 1U);
        for (int rr = 0; rr < OG32_ROW_PAR; ++rr) {
#pragma HLS UNROLL
            const float xv = og32_b2f(fpga_axi128_get_bf16(xbuf[rr][rb][i / OG32_LANES], i % OG32_LANES));
            for (int lane = 0; lane < OG32_LANES; ++lane) {
#pragma HLS UNROLL
                /* acc[..][0] always holds the accumulator for the current c. */
                const float base = first ? 0.0f : acc[rr][lane][0];
                const float sum = base + xv * og32_b2f(fpga_axi128_get_bf16(wbeat, (uint32_t)lane));
                acc[rr][lane][0] = acc[rr][lane][1];
                acc[rr][lane][1] = acc[rr][lane][2];
                acc[rr][lane][2] = acc[rr][lane][3];
                acc[rr][lane][3] = sum;
                if (last) {
                    const uint32_t o = g * 32U + c * 8U + (uint32_t)lane;
                    obuf[rr][o] = sum;
                }
            }
        }
        if (c == 3U) {
            if (last) { i = 0; ++g; } else { ++i; }
        }
    }
}

static inline fpga_axi128_t og32_pack_beat(const uint16_t v[OG32_LANES]) {
    fpga_axi128_t beat;
#if !defined(__SYNTHESIS__) && !defined(FPGA_USE_AP_INT)
    beat.lo = 0; beat.hi = 0;
#endif
    for (int lane = 0; lane < OG32_LANES; ++lane) {
#pragma HLS UNROLL
        fpga_axi128_set_bf16(beat, (uint32_t)lane, v[lane]);
    }
    return beat;
}

#endif
