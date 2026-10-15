#ifndef SWIGLU_V2_H
#define SWIGLU_V2_H

#include <stdint.h>

#include "ap_int_compat.h"
#include "common_abi_v2.h"

typedef ap_uint<128> swiglu_axi128_t;

enum SwigluV2RunMode {
    SWIGLU_V2_PREFILL = COMMON_RUN_MODE_PREFILL,
    SWIGLU_V2_DECODE = COMMON_RUN_MODE_DECODE
};

enum SwigluV2Status {
    SWIGLU_V2_OK = COMMON_STATUS_OK,
    SWIGLU_V2_BAD_ABI = COMMON_STATUS_BAD_ABI,
    SWIGLU_V2_BAD_DIMENSION = COMMON_STATUS_BAD_DIMENSION,
    SWIGLU_V2_BAD_ALIGNMENT = COMMON_STATUS_BAD_ALIGNMENT,
    SWIGLU_V2_BAD_ADDRESS_RANGE = COMMON_STATUS_BAD_ADDRESS_RANGE,
    SWIGLU_V2_BAD_LAYOUT = COMMON_STATUS_BAD_LAYOUT,
    SWIGLU_V2_BAD_LAYER = COMMON_STATUS_BAD_LAYER,
    SWIGLU_V2_BAD_TOKEN = COMMON_STATUS_BAD_TOKEN,
    SWIGLU_V2_BAD_ALIAS = COMMON_STATUS_BAD_ALIAS,
    SWIGLU_V2_NUMERIC_ERROR = COMMON_STATUS_NUMERIC_ERROR,
    SWIGLU_V2_DRIVER_TIMEOUT = COMMON_STATUS_DRIVER_TIMEOUT,
    SWIGLU_V2_BAD_FLAGS = COMMON_STATUS_BAD_FLAGS,
    SWIGLU_V2_INTERNAL_ERROR = COMMON_STATUS_INTERNAL_ERROR
};

enum SwigluV2Constants {
    SWIGLU_V2_ABI_VERSION = COMMON_ABI_VERSION_V2,
    SWIGLU_V2_LAYOUT_W_OG32_I_L32 = 6,
    SWIGLU_V2_HIDDEN_SIZE = 896,
    SWIGLU_V2_INTERMEDIATE_SIZE = 4864,
    SWIGLU_V2_MAX_ROWS = 64,
    // Eight token rows share each weight beat. Decode still issues one beat
    // per cycle; intermediate storage retains only the current 64-wide tile.
    SWIGLU_V2_ROW_TILE = 8,
    SWIGLU_V2_MID_TILE = 64,
    SWIGLU_V2_GATE_UP_PAR = 8,
    SWIGLU_V2_DOWN_OUT_PAR = 8,
    SWIGLU_V2_DOWN_REDUCE_PAR = 8,
    SWIGLU_V2_ACC_LANES = 5,
    SWIGLU_V2_BF16_PER_BEAT = 8,
    SWIGLU_V2_MODULE_REVISION = 4
};

// Logical M13 ABI V2 interface.  The result_* references are synthesized as
// AXI-Lite output registers; no result structure is written to DDR.
void swiglu_v2(
    const swiglu_axi128_t *x,
    const swiglu_axi128_t *w_gate,
    const swiglu_axi128_t *w_up,
    const swiglu_axi128_t *w_down,
    swiglu_axi128_t *y,
    uint32_t run_mode,
    uint32_t valid_rows,
    uint32_t hidden_size,
    uint32_t intermediate_size,
    uint32_t layout_id,
    uint32_t abi_version,
    uint32_t job_id,
    uint32_t flags,
    uint32_t &result_status_code,
    uint32_t &result_processed_rows,
    uint64_t &result_perf_cycles_total,
    uint64_t &result_perf_cycles_compute,
    uint64_t &result_perf_read_beats,
    uint64_t &result_perf_write_beats,
    uint32_t &result_output_crc,
    uint32_t &result_build_config,
    uint32_t &result_mid_tiles,
    uint64_t &result_gate_up_active_cycles,
    uint64_t &result_down_active_cycles,
    uint64_t &result_weight_wait_cycles,
    uint64_t &result_fifo_stall_cycles);

#endif  // SWIGLU_V2_H
