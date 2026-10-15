#ifndef FPGA_ABI_V2_H_
#define FPGA_ABI_V2_H_

/* Shared PS/PL contract for all M05..M14 compute operators. */
#include <stdint.h>

#define FPGA_ABI_VERSION_V2 2U
#define FPGA_MAX_BLOCK_ROWS 64U
#define FPGA_HIDDEN_SIZE_V2 896U
#define FPGA_NUM_Q_HEADS_V2 14U
#define FPGA_NUM_KV_HEADS_V2 2U
#define FPGA_HEAD_DIM_V2 64U
#define FPGA_VOCAB_SIZE_V2 151936U
#define FPGA_MAX_SEQUENCE_V2 2048U

enum fpga_run_mode_v2 {
    FPGA_RUN_PREFILL = 0U,
    FPGA_RUN_DECODE = 1U
};

enum fpga_status_code_v2 {
    FPGA_STATUS_V2_OK = 0U,
    FPGA_STATUS_V2_BAD_ABI = 1U,
    FPGA_STATUS_V2_BAD_DIMENSION = 2U,
    FPGA_STATUS_V2_BAD_ALIGNMENT = 3U,
    FPGA_STATUS_V2_BAD_ADDRESS_RANGE = 4U,
    FPGA_STATUS_V2_BAD_LAYOUT = 5U,
    FPGA_STATUS_V2_BAD_LAYER = 6U,
    FPGA_STATUS_V2_BAD_TOKEN = 7U,
    FPGA_STATUS_V2_BAD_ALIAS = 8U,
    FPGA_STATUS_V2_NUMERIC_ERROR = 9U,
    FPGA_STATUS_V2_DRIVER_TIMEOUT = 10U,
    FPGA_STATUS_V2_BAD_FLAGS = 11U,
    FPGA_STATUS_V2_INTERNAL_ERROR = 12U
};

enum fpga_layout_id_v2 {
    FPGA_LAYOUT_INVALID = 0U,
    FPGA_LAYOUT_ACT_TH_V2 = 1U,
    FPGA_LAYOUT_Q_HTD_V2 = 2U,
    FPGA_LAYOUT_KV_HTD_V2 = 3U,
    FPGA_LAYOUT_KVC_LHSD_V2 = 4U,
    FPGA_LAYOUT_ROPE_SD_V2 = 5U,
    FPGA_LAYOUT_W_OG32_I_L32_V2 = 6U,
    FPGA_LAYOUT_VOCAB_VH_V2 = 7U,
    FPGA_LAYOUT_TOKEN_U32_V2 = 8U,
    FPGA_LAYOUT_HISTORY_BITMAP_V2 = 9U
};

enum fpga_dtype_id_v2 {
    FPGA_DTYPE_INVALID = 0U,
    FPGA_DTYPE_BF16 = 1U,
    FPGA_DTYPE_FP32 = 2U,
    FPGA_DTYPE_U32 = 3U,
    FPGA_DTYPE_U8 = 4U
};

#define FPGA_FLAG_ENABLE_OUTPUT_CRC (1U << 0)
#define FPGA_FLAG_ENABLE_PERF        (1U << 1)
#define FPGA_FLAG_DEBUG_SNAPSHOT     (1U << 2)
#define FPGA_FLAG_STRICT_CHECK       (1U << 3)
#define FPGA_COMMON_FLAGS_MASK       0x0000000FU
#define FPGA_SUPPORTED_FLAGS_MASK    (FPGA_FLAG_ENABLE_OUTPUT_CRC | FPGA_FLAG_ENABLE_PERF | FPGA_FLAG_STRICT_CHECK)

typedef struct {
    uint32_t abi_version;
    uint32_t job_id;
    uint32_t run_mode;
    uint32_t valid_rows;
    uint32_t flags;
} common_job_v2;

typedef struct {
    uint32_t status_code;
    uint32_t processed_rows;
    uint64_t perf_cycles_total;
    uint64_t perf_cycles_compute;
    uint64_t perf_read_beats;
    uint64_t perf_write_beats;
    uint32_t output_crc;
    uint32_t build_config;
} common_result_v2;

static inline uint32_t fpga_validate_job_v2(const common_job_v2 *job) {
    if (job == 0 || job->abi_version != FPGA_ABI_VERSION_V2) {
        return FPGA_STATUS_V2_BAD_ABI;
    }
    if (job->run_mode > FPGA_RUN_DECODE || job->valid_rows == 0U ||
        job->valid_rows > FPGA_MAX_BLOCK_ROWS ||
        (job->run_mode == FPGA_RUN_DECODE && job->valid_rows != 1U)) {
        return FPGA_STATUS_V2_BAD_DIMENSION;
    }
    /* Snapshot is not implemented by these three operators. Strict physical
     * address/window/alias validation is supplied by the PS M02/M03 adapter;
     * the HLS control path still validates ABI, dimensions and layouts. */
    if ((job->flags & ~FPGA_SUPPORTED_FLAGS_MASK) != 0U) {
        return FPGA_STATUS_V2_BAD_FLAGS;
    }
    return FPGA_STATUS_V2_OK;
}

static inline void fpga_result_init_v2(common_result_v2 *result,
                                       uint32_t status,
                                       uint32_t rows,
                                       uint32_t build_config) {
    if (result == 0) return;
    result->status_code = status;
    result->processed_rows = status == FPGA_STATUS_V2_OK ? rows : 0U;
    result->perf_cycles_total = 0U;
    result->perf_cycles_compute = 0U;
    result->perf_read_beats = 0U;
    result->perf_write_beats = 0U;
    result->output_crc = 0U;
    result->build_config = build_config;
}

#if defined(__cplusplus)
static_assert(sizeof(common_job_v2) == 20, "common_job_v2 ABI size");
static_assert(sizeof(common_result_v2) == 48, "common_result_v2 ABI size");
#endif

#endif
