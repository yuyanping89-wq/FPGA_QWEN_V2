#ifndef PSPL_V2_ABI_H
#define PSPL_V2_ABI_H

#include <stddef.h>
#include <stdint.h>

/*
 * Stable V2 data contract shared by the host runtime, software references and
 * HLS IP adapters. Keep enum values and structure field order synchronized
 * with the wire/register ABI; do not silently change an existing field's meaning.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PSPL_V2_ABI_VERSION 2U
#define PSPL_V2_PROTOCOL_VERSION 2U

#define PSPL_V2_NUM_LAYERS 24U
#define PSPL_V2_HIDDEN_SIZE 896U
#define PSPL_V2_NUM_Q_HEADS 14U
#define PSPL_V2_NUM_KV_HEADS 2U
#define PSPL_V2_GQA_GROUP_SIZE 7U
#define PSPL_V2_HEAD_DIM 64U
#define PSPL_V2_INTERMEDIATE_SIZE 4864U
#define PSPL_V2_VOCAB_SIZE 151936U
#define PSPL_V2_MAX_SEQUENCE 2048U
#define PSPL_V2_MAX_BLOCK_ROWS 64U
#define PSPL_V2_EXTERNAL_BEAT_BITS 128U
#define PSPL_V2_CACHE_LINE_BYTES 64U
#define PSPL_V2_BF16_BYTES 2U
#define PSPL_V2_U32_BYTES 4U
#define PSPL_V2_WEIGHT_GROUP 32U

#define PSPL_V2_MAX_PS_REGIONS 2U
#define PSPL_V2_MAX_MANIFEST_FILES 64U
#define PSPL_V2_MAX_SESSIONS 16U
#define PSPL_V2_MAX_JOBS 64U
#define PSPL_V2_MAX_IPS 32U
#define PSPL_V2_MAX_CACHE_EVENTS 1024U
#define PSPL_V2_MAX_NAME 128U
#define PSPL_V2_MAX_ERROR 256U

typedef enum pspl_v2_status_code {
    PSPL_V2_OK = 0,
    PSPL_V2_BAD_ABI = 1,
    PSPL_V2_BAD_DIMENSION = 2,
    PSPL_V2_BAD_ALIGNMENT = 3,
    PSPL_V2_BAD_ADDRESS_RANGE = 4,
    PSPL_V2_BAD_LAYOUT = 5,
    PSPL_V2_BAD_LAYER = 6,
    PSPL_V2_BAD_TOKEN = 7,
    PSPL_V2_BAD_ALIAS = 8,
    PSPL_V2_NUMERIC_ERROR = 9,
    PSPL_V2_DRIVER_TIMEOUT = 10,
    PSPL_V2_BAD_FLAGS = 11,
    PSPL_V2_INTERNAL_ERROR = 12
} pspl_v2_status_code_t;

typedef enum pspl_v2_run_mode {
    PSPL_V2_PREFILL = 0,
    PSPL_V2_DECODE = 1
} pspl_v2_run_mode_t;

typedef enum pspl_v2_dtype {
    PSPL_V2_DTYPE_INVALID = 0,
    PSPL_V2_DTYPE_BF16 = 1,
    PSPL_V2_DTYPE_FP32 = 2,
    PSPL_V2_DTYPE_U32 = 3,
    PSPL_V2_DTYPE_U8 = 4
} pspl_v2_dtype_t;

typedef enum pspl_v2_layout_id {
    /* Tensor shapes are documented by the producer/consumer module; IDs are ABI-stable. */
    PSPL_V2_LAYOUT_INVALID = 0,
    PSPL_V2_LAYOUT_ACT_TH = 1,
    PSPL_V2_LAYOUT_Q_HTD = 2,
    PSPL_V2_LAYOUT_KV_HTD = 3,
    PSPL_V2_LAYOUT_KVC_LHSD = 4,
    PSPL_V2_LAYOUT_ROPE_SD = 5,
    PSPL_V2_LAYOUT_W_OG32_I_L32 = 6,
    PSPL_V2_LAYOUT_VOCAB_VH = 7,
    PSPL_V2_LAYOUT_TOKEN_U32 = 8,
    PSPL_V2_LAYOUT_HISTORY_BITMAP = 9
} pspl_v2_layout_id_t;

typedef enum pspl_v2_owner {
    /* CPU_DIRTY requires clean-before-PL-read; PL_WRITTEN requires invalidate-before-CPU-read. */
    PSPL_V2_OWNER_CPU_CLEAN = 0,
    PSPL_V2_OWNER_CPU_DIRTY = 1,
    PSPL_V2_OWNER_PL_READABLE = 2,
    PSPL_V2_OWNER_PL_WRITTEN = 3,
    PSPL_V2_OWNER_IMMUTABLE = 4
} pspl_v2_owner_t;

typedef enum pspl_v2_memory_kind {
    PSPL_V2_MEMORY_PS_DDR = 0,
    PSPL_V2_MEMORY_PL_DDR = 1
} pspl_v2_memory_kind_t;

typedef enum pspl_v2_module_id {
    PSPL_V2_M00_HOST = 0,
    PSPL_V2_M01_SCHEDULER = 1,
    PSPL_V2_M02_DRIVER = 2,
    PSPL_V2_M03_BUFFER = 3,
    PSPL_V2_M04_WEIGHT_STORE = 4,
    PSPL_V2_M05_EMBEDDING = 5,
    PSPL_V2_M06_RMSNORM = 6,
    PSPL_V2_M07_QKV = 7,
    PSPL_V2_M08_ROPE = 8,
    PSPL_V2_M09_KV_CACHE = 9,
    PSPL_V2_M10_ATTENTION = 10,
    PSPL_V2_M11_ATTN_OUT = 11,
    PSPL_V2_M12_RESIDUAL = 12,
    PSPL_V2_M13_SWIGLU = 13,
    PSPL_V2_M14_LM_HEAD = 14,
    PSPL_V2_M15_DEBUG = 15
} pspl_v2_module_id_t;

typedef enum pspl_v2_capability {
    PSPL_V2_CAP_NONE = 0U,
    PSPL_V2_CAP_DYNAMIC_ROWS = 1U << 0,
    PSPL_V2_CAP_DEBUG_CRC = 1U << 1,
    PSPL_V2_CAP_DEBUG_SNAPSHOT = 1U << 2,
    PSPL_V2_CAP_IRQ_COMPLETE = 1U << 3,
    PSPL_V2_CAP_INPLACE_ALLOWED = 1U << 4,
    PSPL_V2_CAP_ONLINE_SOFTMAX = 1U << 5,
    PSPL_V2_CAP_PL_DDR_DIRECT = 1U << 6,
    PSPL_V2_CAP_WRITE_LOGITS_OPTIONAL = 1U << 7,
    PSPL_V2_CAP_PERF_COUNTERS = 1U << 8
} pspl_v2_capability_t;

typedef enum pspl_v2_common_flag {
    /* Flags shared by modules; unsupported bits must be rejected, not ignored. */
    PSPL_V2_FLAG_ENABLE_OUTPUT_CRC = 1U << 0,
    PSPL_V2_FLAG_ENABLE_PERF = 1U << 1,
    PSPL_V2_FLAG_DEBUG_SNAPSHOT = 1U << 2,
    PSPL_V2_FLAG_STRICT_CHECK = 1U << 3
} pspl_v2_common_flag_t;

typedef enum pspl_v2_profile_flag {
    PSPL_V2_PROFILE_PS_DDR_READY = 1U << 0,
    PSPL_V2_PROFILE_MIG_CALIBRATED = 1U << 1,
    PSPL_V2_PROFILE_RESET_RELEASED = 1U << 2,
    PSPL_V2_PROFILE_HP_WIDTH_MATCH = 1U << 3,
    PSPL_V2_PROFILE_MIG_AXI_WIDTH_MATCH = 1U << 4
} pspl_v2_profile_flag_t;

typedef struct pspl_v2_common_job {
    uint32_t abi_version; /* Must match the hardware/software module ABI. */
    uint32_t job_id;      /* Correlates module activity with M01/M15 records. */
    uint32_t run_mode;    /* Prefill accepts 1..64 rows; Decode requires one. */
    uint32_t valid_rows;  /* Number of meaningful rows; padded rows are forbidden. */
    uint32_t flags;       /* Common flags plus only the module's documented extensions. */
} pspl_v2_common_job_t;

typedef struct pspl_v2_common_result {
    uint32_t status_code;          /* pspl_v2_status_code_t value. */
    uint32_t processed_rows;        /* Number of successfully completed rows. */
    uint64_t perf_cycles_total;     /* Zero means measured hardware cycles unavailable. */
    uint64_t perf_cycles_compute;  /* Compute-active cycles, when measured by RTL. */
    uint64_t perf_read_beats;       /* Beat semantics are defined by each module. */
    uint64_t perf_write_beats;
    uint32_t output_crc;            /* Optional CRC-32 of module-defined output bytes. */
    uint32_t build_config;          /* Packed implementation identity for diagnostics. */
} pspl_v2_common_result_t;

/* Memory window exposed to PS or PL logic; PHY width is distinct from AXI width. */
typedef struct pspl_v2_hw_mem_region {
    uint64_t base;
    uint64_t bytes;
    uint16_t phy_bits;
    uint8_t memory_kind;
    uint8_t flags;
} pspl_v2_hw_mem_region_t;

/* Board facts used by buffer validation and the IP driver's startup self-check. */
typedef struct pspl_v2_hardware_profile {
    uint32_t profile_version;
    uint32_t board_part_id;
    uint32_t ps_ref_clock_hz;
    uint32_t pl_ddr_ref_clock_hz;
    uint32_t pl_ddr_ui_clock_hz;
    uint32_t pl_user_clock_hz;
    uint32_t ps_hp_bits;
    uint32_t ps_hp_addr_bits;
    uint32_t pl_ddr_axi_bits;
    uint32_t pl_ddr_axi_addr_bits;
    uint32_t ps_ddr_region_count;
    uint32_t flags;
    uint64_t ps_ddr_capacity_bytes;
    uint64_t pl_ddr_capacity_bytes;
    pspl_v2_hw_mem_region_t ps_ddr_regions[PSPL_V2_MAX_PS_REGIONS];
    pspl_v2_hw_mem_region_t pl_ddr_region;
    char board_part_name[32];
} pspl_v2_hardware_profile_t;

/* Physical allocation and logical tensor metadata for one registered buffer. */
typedef struct pspl_v2_buffer_desc {
    uint64_t phys_addr;       /* Device/AXI-visible byte address. */
    uint32_t capacity_bytes;  /* Allocated range length, not just populated length. */
    uint32_t valid_offset;    /* Start of the current logical tensor in this allocation. */
    uint32_t valid_bytes;     /* Current logical tensor length. */
    uint16_t layout_id;       /* pspl_v2_layout_id_t. */
    uint8_t dtype;            /* pspl_v2_dtype_t. */
    uint8_t owner;            /* pspl_v2_owner_t. */
    uint32_t generation;      /* Incremented whenever a producer publishes new contents. */
    uint32_t flags;           /* Reserved for descriptor extensions; currently zero. */
} pspl_v2_buffer_desc_t;

/* Compact diagnostic identity of the selected implementation parameters. */
typedef struct pspl_v2_build_config {
    uint32_t par_lanes_code;
    uint32_t row_tile;
    uint32_t main_tile;
    uint32_t numeric_impl_id;
    uint32_t clock_code;
    uint32_t module_revision;
} pspl_v2_build_config_t;

/** @brief Return the stable printable name for a status code.
 * @param status Status value to translate.
 * @return Static, NUL-terminated name; unknown values map to an invalid-name string.
 */
const char *pspl_v2_status_string(pspl_v2_status_code_t status);



/** @brief Return the storage width of a supported tensor data type.
 * @param dtype Data type to query.
 * @return Element width in bytes, or zero for an unsupported type.
 */
size_t pspl_v2_dtype_size(pspl_v2_dtype_t dtype);



/** @brief Validate common ABI fields and module-supported flags.
 * @param job Common job descriptor to validate.
 * @param allowed_extra_flags Module-specific flag bits accepted in addition to common flags.
 * @return PSPL_V2_OK on success, otherwise the first ABI, dimension, mode, or flag error.
 */
pspl_v2_status_code_t pspl_v2_common_job_validate(
    const pspl_v2_common_job_t *job,
    uint32_t allowed_extra_flags);


/** @brief Validate a buffer descriptor's address, subrange, alignment, and metadata.
 * @param desc Descriptor to check.
 * @param alignment_bytes Required physical-address alignment; zero disables this check.
 * @return PSPL_V2_OK when valid, otherwise a specific descriptor validation status.
 */
pspl_v2_status_code_t pspl_v2_buffer_desc_validate(
    const pspl_v2_buffer_desc_t *desc,
    uint32_t alignment_bytes);


/** @brief Pack implementation identity fields into the compact ABI word.
 * @param config Source implementation identity.
 * @param encoded Receives the packed value on success.
 * @return PSPL_V2_OK, or PSPL_V2_BAD_DIMENSION if a field exceeds its bit width.
 */
pspl_v2_status_code_t pspl_v2_build_config_encode(
    const pspl_v2_build_config_t *config,
    uint32_t *encoded);


/** @brief Decode a packed implementation identity word.
 * @param encoded Packed value previously produced by the matching ABI encoder.
 * @param config Receives the decoded fields; must not be NULL.
 * @return None.
 */
void pspl_v2_build_config_decode(uint32_t encoded, pspl_v2_build_config_t *config);



/** @brief Compute IEEE CRC-32 for one complete byte sequence.
 * @param data Byte sequence; may be NULL only when bytes is zero.
 * @param bytes Number of bytes to process.
 * @return Finalized reflected IEEE CRC-32 value.
 */
uint32_t pspl_v2_crc32(const void *data, size_t bytes);


/** @brief Continue IEEE CRC-32 calculation from a prior finalized value.
 * @param crc Finalized CRC from the preceding chunk, or zero for the first chunk.
 * @param data Next byte sequence; may be NULL only when bytes is zero.
 * @param bytes Number of bytes in this chunk.
 * @return Updated finalized CRC-32 value.
 */
uint32_t pspl_v2_crc32_update(uint32_t crc, const void *data, size_t bytes);



/** @brief Convert an IEEE FP32 value to BF16 using round-to-nearest-even.
 * @param value Input floating-point value.
 * @return The 16-bit BF16 representation.
 */
uint16_t pspl_v2_float_to_bf16(float value);


/** @brief Expand a BF16 bit pattern to an IEEE FP32 value.
 * @param bits Input BF16 representation.
 * @return Equivalent FP32 value.
 */
float pspl_v2_bf16_to_float(uint16_t bits);

#ifdef __cplusplus
}
#endif

#endif
