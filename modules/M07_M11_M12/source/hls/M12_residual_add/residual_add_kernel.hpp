#ifndef RESIDUAL_ADD_KERNEL_HPP
#define RESIDUAL_ADD_KERNEL_HPP

#include <stdint.h>
#include "../../include/fpga_abi_v2.h"
#include "../../include/fpga_axi128.h"

#ifndef RESIDUAL_H
#define RESIDUAL_H 896
#endif
#ifndef RESIDUAL_T_MAX
#define RESIDUAL_T_MAX 64
#endif
#ifndef RESIDUAL_DEPTH
#define RESIDUAL_DEPTH (RESIDUAL_T_MAX * RESIDUAL_H)
#endif

constexpr int H = RESIDUAL_H;
constexpr int T_MAX = RESIDUAL_T_MAX;
constexpr int DEPTH = RESIDUAL_DEPTH;
typedef uint16_t data_t;

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

void attention_residual_add(
    const data_t* residual,
    const data_t* attention,
    data_t* output,
    int T);

void residual_add_v2(
    const data_t* a, const data_t* b, data_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t residual_role, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, common_result_v2* result);

/* HLS exposes the common result as equivalent AXI-Lite output fields. */
void residual_add_v2_hls(
    const fpga_axi128_t* a, const fpga_axi128_t* b, fpga_axi128_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t residual_role, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, uint32_t& result_status_code,
    uint32_t& result_processed_rows, uint64_t& result_perf_cycles_total,
    uint64_t& result_perf_cycles_compute, uint64_t& result_perf_read_beats,
    uint64_t& result_perf_write_beats, uint32_t& result_output_crc,
    uint32_t& result_build_config);

#endif
