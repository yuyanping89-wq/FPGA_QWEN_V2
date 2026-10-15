#ifndef ATTENTION_OUTPUT_KERNEL_HPP
#define ATTENTION_OUTPUT_KERNEL_HPP

#include <stdint.h>
#include "../../include/fpga_abi_v2.h"
#include "../../include/fpga_axi128.h"

#ifndef ATTN_OUT_H
#define ATTN_OUT_H 896
#endif
#ifndef ATTN_OUT_T_MAX
#define ATTN_OUT_T_MAX 64
#endif
#ifndef ATTN_OUT_X_DEPTH
#define ATTN_OUT_X_DEPTH (ATTN_OUT_T_MAX * ATTN_OUT_H)
#endif
#ifndef ATTN_OUT_W_DEPTH
#define ATTN_OUT_W_DEPTH (ATTN_OUT_H * ATTN_OUT_H)
#endif
#ifndef ATTN_OUT_Y_DEPTH
#define ATTN_OUT_Y_DEPTH (ATTN_OUT_T_MAX * ATTN_OUT_H)
#endif
#ifndef ATTN_OUT_PAR
#define ATTN_OUT_PAR 8
#endif

constexpr int H = ATTN_OUT_H;
constexpr int T_MAX = ATTN_OUT_T_MAX;
constexpr int OUT_PAR = ATTN_OUT_PAR;
static_assert(OUT_PAR == 4 || OUT_PAR == 8, "ATTN_OUT_PAR must be 4 or 8");
static_assert(H % OUT_PAR == 0, "H must be divisible by OUT_PAR");
typedef uint16_t data_t;
typedef float acc_t;

static inline float bf16_to_float(data_t value) {
    union {
        uint32_t u;
        float f;
    } bits;
    bits.u = ((uint32_t)value) << 16;
    return bits.f;
}

static inline data_t float_to_bf16(float value) {
    union {
        float f;
        uint32_t u;
    } bits;
    bits.f = value;
    const uint32_t lsb = (bits.u >> 16) & 1U;
    const uint32_t rounded = bits.u + 0x7FFFU + lsb;
    return (data_t)(rounded >> 16);
}

void attention_output_projection(
    const data_t* x,
    const data_t* weight,
    data_t* y,
    int T);

void attention_out_v2(
    const data_t* x, const data_t* weight, data_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t layout_id, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, common_result_v2* result);

void attention_out_v2_hls(
    const fpga_axi128_t* x, const fpga_axi128_t* weight, fpga_axi128_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t layout_id, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, uint32_t& result_status_code,
    uint32_t& result_processed_rows, uint64_t& result_perf_cycles_total,
    uint64_t& result_perf_cycles_compute, uint64_t& result_perf_read_beats,
    uint64_t& result_perf_write_beats, uint32_t& result_output_crc,
    uint32_t& result_build_config);

#endif
