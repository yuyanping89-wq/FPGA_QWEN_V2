#ifndef QWEN_BOARD_CONFIG_H
#define QWEN_BOARD_CONFIG_H

#include <stdint.h>

/* AXU9EG / current vivado/proj/qwen Block Design. */
#define QWEN_ABI_VERSION                 2U
#ifndef QWEN_ENABLE_UART_LOG
#define QWEN_ENABLE_UART_LOG             0
#endif
#define QWEN_NUM_LAYERS                  24U
#define QWEN_HIDDEN_SIZE                 896U
#define QWEN_Q_HEADS                     14U
#define QWEN_KV_HEADS                    2U
#define QWEN_HEAD_DIM                    64U
#define QWEN_INTERMEDIATE_SIZE           4864U
#define QWEN_VOCAB_SIZE                  151936U
#define QWEN_MAX_SEQUENCE                2048U
#define QWEN_MAX_PREFILL_ROWS            64U
#define QWEN_CACHE_LINE_BYTES            64U

#define QWEN_CDMA_BASE                   0xB0000000U
#define QWEN_M06_BASE                    0xB0010000U
#define QWEN_M09_BASE                    0xB0020000U
#define QWEN_M13_BASE                    0xB0030000U
#define QWEN_M05_BASE                    0xB0040000U
#define QWEN_M07_BASE                    0xB0050000U
#define QWEN_M11_BASE                    0xB0060000U
#define QWEN_M12_BASE                    0xB0070000U
#define QWEN_M14_BASE                    0xB0080000U
#define QWEN_M08_BASE                    0xB0090000U
#define QWEN_M10_BASE                    0xB00A0000U

#define QWEN_PS_DDR_BASE                 0x00000000U
#define QWEN_PS_DDR_BYTES                0x80000000U
#define QWEN_PL_DDR_BASE                 0x80000000U
#define QWEN_PL_DDR_BYTES                0x40000000U
#define QWEN_PL_DDR_END_EXCLUSIVE        0xC0000000U

#define QWEN_BF16_BYTES                  2U
#define QWEN_ROW_BYTES                   (QWEN_HIDDEN_SIZE * QWEN_BF16_BYTES)
#define QWEN_MAX_ACTIVATION_BYTES        (QWEN_MAX_PREFILL_ROWS * QWEN_ROW_BYTES)
#define QWEN_KV_HEAD_ROW_BYTES           (QWEN_HEAD_DIM * QWEN_BF16_BYTES)
#define QWEN_KV_CACHE_BYTES_PER_TENSOR   \
    (QWEN_NUM_LAYERS * QWEN_KV_HEADS * QWEN_MAX_SEQUENCE * QWEN_KV_HEAD_ROW_BYTES)
#define QWEN_ROPE_TABLE_BYTES            \
    (QWEN_MAX_SEQUENCE * QWEN_HEAD_DIM * QWEN_BF16_BYTES)

/* One tied BF16 [vocab, hidden] tensor is used by both M05 and M14. */
#define QWEN_SHARED_VOCAB_WEIGHT_BYTES   \
    ((uint64_t)QWEN_VOCAB_SIZE * QWEN_HIDDEN_SIZE * QWEN_BF16_BYTES)

/* Packed V2 W_OG32_I_L32 assets, all byte sizes are 64-byte aligned. */
#define QWEN_M07_WQKV_BYTES              2064384U
#define QWEN_M07_BQKV_BYTES              2304U
#define QWEN_M11_WO_BYTES                1605632U
#define QWEN_M06_GAMMA_BYTES             (QWEN_HIDDEN_SIZE * QWEN_BF16_BYTES)
#define QWEN_M13_WEIGHT_BYTES            \
    (QWEN_HIDDEN_SIZE * QWEN_INTERMEDIATE_SIZE * QWEN_BF16_BYTES)
#define QWEN_LAYER_WEIGHT_STRIDE         0x01C71700U
#define QWEN_LAYER_OFF_M07_WQKV          0x00000000U
#define QWEN_LAYER_OFF_M07_BQKV          0x001F8000U
#define QWEN_LAYER_OFF_M11_WO            0x001F8900U
#define QWEN_LAYER_OFF_M06_ATTN_GAMMA    0x00380900U
#define QWEN_LAYER_OFF_M06_FFN_GAMMA     0x00381000U
#define QWEN_LAYER_OFF_M13_W_GATE        0x00381700U
#define QWEN_LAYER_OFF_M13_W_UP          0x00BD1700U
#define QWEN_LAYER_OFF_M13_W_DOWN        0x01421700U

/* Model weights occupy the first 0x3AE4A800 bytes of PL DDR. */
#define QWEN_PL_SHARED_WEIGHT_ADDR       QWEN_PL_DDR_BASE
#define QWEN_PL_LAYER_WEIGHT_BASE        \
    (QWEN_PL_DDR_BASE + (uint32_t)QWEN_SHARED_VOCAB_WEIGHT_BYTES)
#define QWEN_PL_LAYER_WEIGHTS_BYTES      (QWEN_NUM_LAYERS * QWEN_LAYER_WEIGHT_STRIDE)
#define QWEN_PL_K_CACHE_BASE             \
    (QWEN_PL_LAYER_WEIGHT_BASE + QWEN_PL_LAYER_WEIGHTS_BYTES)
#define QWEN_PL_V_CACHE_BASE             \
    (QWEN_PL_K_CACHE_BASE + (uint32_t)QWEN_KV_CACHE_BYTES_PER_TENSOR)

/* M08 ROPE_SD_V2 tables, populated by the model loader through CDMA. */
#define QWEN_PL_ROPE_COS_ADDR            0xBC700000U
#define QWEN_PL_ROPE_SIN_ADDR            0xBC740000U

/* CDMA is 32-bit, simple mode, no DRE; reserve 48 MiB for activations and probe. */
#define QWEN_PL_WORK_BASE                0xBD000000U
#define QWEN_PL_WORK_BYTES               0x03000000U
#define QWEN_PL_PROBE_ADDR               QWEN_PL_WORK_BASE

/* Fixed non-aliasing work buffers. Every buffer has at least 0x20000 bytes. */
#define QWEN_PL_BUF_TOKEN_IDS            (QWEN_PL_WORK_BASE + 0x00010000U)
#define QWEN_PL_BUF_HIDDEN_A             (QWEN_PL_WORK_BASE + 0x00030000U)
#define QWEN_PL_BUF_HIDDEN_B             (QWEN_PL_WORK_BASE + 0x00050000U)
#define QWEN_PL_BUF_NORM                 (QWEN_PL_WORK_BASE + 0x00070000U)
#define QWEN_PL_BUF_Q                    (QWEN_PL_WORK_BASE + 0x00090000U)
#define QWEN_PL_BUF_K                    (QWEN_PL_WORK_BASE + 0x000B0000U)
#define QWEN_PL_BUF_V                    (QWEN_PL_WORK_BASE + 0x000D0000U)
#define QWEN_PL_BUF_Q_ROT                (QWEN_PL_WORK_BASE + 0x000F0000U)
#define QWEN_PL_BUF_K_ROT                (QWEN_PL_WORK_BASE + 0x00110000U)
#define QWEN_PL_BUF_CONTEXT              (QWEN_PL_WORK_BASE + 0x00130000U)
#define QWEN_PL_BUF_ATTN_OUT             (QWEN_PL_WORK_BASE + 0x00150000U)
#define QWEN_PL_BUF_RESIDUAL              (QWEN_PL_WORK_BASE + 0x00170000U)
#define QWEN_PL_BUF_FFN_OUT               (QWEN_PL_WORK_BASE + 0x00190000U)
#define QWEN_PL_BUF_FINAL_NORM            (QWEN_PL_WORK_BASE + 0x001B0000U)
#define QWEN_PL_BUF_HISTORY               (QWEN_PL_WORK_BASE + 0x001D0000U)
#define QWEN_PL_BUF_FINAL_GAMMA           (QWEN_PL_WORK_BASE + 0x00200000U)
#define QWEN_HISTORY_BITMAP_BYTES         ((QWEN_VOCAB_SIZE + 7U) / 8U)

/* Current packages report these build identities after a completed job. */
#define QWEN_M05_BUILD_CONFIG             0x30010011U
#define QWEN_M06_BUILD_CONFIG             0x10002011U
#define QWEN_M07_BUILD_CONFIG             0x40008041U
#define QWEN_M08_BUILD_CONFIG             0x30010011U
#define QWEN_M09_BUILD_CONFIG             0x10010010U
#define QWEN_M10_BUILD_CONFIG             0x30008011U
#define QWEN_M11_BUILD_CONFIG             0x40008041U
#define QWEN_M12_BUILD_CONFIG             0x30010001U
#define QWEN_M13_BUILD_CONFIG             0x40450041U
#define QWEN_M14_BUILD_CONFIG             0x10008011U

#define QWEN_IP_FLAG_CRC                  0x1U
#define QWEN_IP_FLAG_PERF                 0x2U
#define QWEN_IP_FLAG_STRICT               0x8U
#define QWEN_IP_FLAGS                     QWEN_IP_FLAG_PERF
#define QWEN_M13_FLAGS                    QWEN_IP_FLAGS
#define QWEN_LAYOUT_W_OG32_I_L32          6U
#define QWEN_M06_EPSILON                  1.0e-6f

#define QWEN_CDMA_MAX_BTT                 0x007FFFFFU
#define QWEN_CDMA_CHUNK_BYTES             0x00100000U
#define QWEN_POLL_TIMEOUT_MS              300000U
#define QWEN_CDMA_TIMEOUT_MS              30000U

/* Original, non-congestion-tuned bitstream: M13 ROW_TILE=4, 100 MHz. */
#define QWEN_ACCELERATOR_CLOCK_HZ         100000000U
#define QWEN_PL_DEBUG_DMA_ADDR           (QWEN_PL_WORK_BASE + 0x01000000U)
#define QWEN_PROFILE_MAX_STATS           1024U
#define QWEN_PROFILE_MAX_BOOT_STATS      512U
#define QWEN_PROFILE_MAX_TRACE           2048U
#define QWEN_PROFILE_HEADER_BYTES        160U
#define QWEN_PROFILE_RECORD_BYTES        112U
#define QWEN_PROFILE_MAX_WIRE_BYTES      (QWEN_PROFILE_HEADER_BYTES + \
    (QWEN_PROFILE_MAX_STATS + QWEN_PROFILE_MAX_BOOT_STATS + QWEN_PROFILE_MAX_TRACE) * \
    QWEN_PROFILE_RECORD_BYTES + QWEN_MAX_SEQUENCE * 8U)

#endif
