// Module 4: Q/K/V Linear projection — HLS kernel header.
//
// Compile-time dims for Qwen2.5-0.5B-Instruct layer QKV projection.
// Fixed loop bounds let HLS schedule/pipeline tightly. T (sequence length)
// is a runtime argument bounded by T_MAX so the same kernel serves both
// prefill (T=36) and decode (T=1).

#ifndef QKV_KERNEL_HPP
#define QKV_KERNEL_HPP

#include <stdint.h>
#include "../../include/fpga_abi_v2.h"
#include "../../include/fpga_axi128.h"

// ---- Qwen2.5-0.5B shape ----
// These macros let a small RTL-cosim-only build reuse the same kernel source.
#ifndef QKV_H
#define QKV_H 896
#endif
#ifndef QKV_A
#define QKV_A 14
#endif
#ifndef QKV_KVH
#define QKV_KVH 2
#endif
#ifndef QKV_D
#define QKV_D 64
#endif

constexpr int H     = QKV_H;    // hidden_size
constexpr int A     = QKV_A;    // num_attention_heads
constexpr int KVH   = QKV_KVH;  // num_key_value_heads
constexpr int D     = QKV_D;    // head_dim
constexpr int Q_OUT = A * D;    // default: 896
constexpr int K_OUT = KVH * D;  // default: 128
constexpr int V_OUT = KVH * D;  // default: 128

#ifndef QKV_OUT_PAR
#define QKV_OUT_PAR 8
#endif
constexpr int OUT_PAR = QKV_OUT_PAR;
static_assert(OUT_PAR == 4 || OUT_PAR == 8, "QKV_OUT_PAR must be 4 or 8");
static_assert(Q_OUT % OUT_PAR == 0, "Q_OUT must be divisible by OUT_PAR");
static_assert(K_OUT % OUT_PAR == 0, "K_OUT must be divisible by OUT_PAR");
static_assert(V_OUT % OUT_PAR == 0, "V_OUT must be divisible by OUT_PAR");

// Max sequence length this kernel instance supports (prefill prompt len).
// Bump if you feed longer prompts.
#ifndef QKV_T_MAX
#define QKV_T_MAX 64
#endif
constexpr int T_MAX = QKV_T_MAX;

// HLS interface depths. Defaults match the full-size Qwen2.5-0.5B kernel.
#ifndef QKV_X_DEPTH
#define QKV_X_DEPTH 57344
#endif
#ifndef QKV_WQ_DEPTH
#define QKV_WQ_DEPTH 802816
#endif
#ifndef QKV_WK_DEPTH
#define QKV_WK_DEPTH 114688
#endif
#ifndef QKV_WV_DEPTH
#define QKV_WV_DEPTH 114688
#endif
#ifndef QKV_BQ_DEPTH
#define QKV_BQ_DEPTH 896
#endif
#ifndef QKV_BK_DEPTH
#define QKV_BK_DEPTH 128
#endif
#ifndef QKV_BV_DEPTH
#define QKV_BV_DEPTH 128
#endif
#ifndef QKV_Q_DEPTH
#define QKV_Q_DEPTH 57344
#endif
#ifndef QKV_K_DEPTH
#define QKV_K_DEPTH 8192
#endif
#ifndef QKV_V_DEPTH
#define QKV_V_DEPTH 8192
#endif

// BF16 little-endian storage at every AXI memory interface; fp32 accumulation.
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

// Top-level kernel. All buffers are row-major.
//   x  : [T, H]
//   Wq : [Q_OUT, H]  bq : [Q_OUT]   q : [T, Q_OUT]
//   Wk : [K_OUT, H]  bk : [K_OUT]   k : [T, K_OUT]
//   Wv : [V_OUT, H]  bv : [V_OUT]   v : [T, V_OUT]
void qkv_proj(
    const data_t* x,
    const data_t* Wq, const data_t* bq,
    const data_t* Wk, const data_t* bk,
    const data_t* Wv, const data_t* bv,
    data_t* q, data_t* k, data_t* v,
    int T);

/* ABI v2 entry point. Inputs remain [T,H], while q/k/v are head-major:
 * q=[QH,T,D], k/v=[KVH,T,D]. The legacy qkv_proj entry point is retained
 * for existing generated IP and test benches. */
void qkv_projection_v2(
    const data_t* x, const data_t* w_qkv, const data_t* bias_qkv,
    data_t* q, data_t* k, data_t* v,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t layout_id,
    uint32_t abi_version, uint32_t job_id, uint32_t flags,
    common_result_v2* result);

void qkv_projection_v2_hls(
    const fpga_axi128_t* x, const fpga_axi128_t* w_qkv,
    const fpga_axi128_t* bias_qkv, fpga_axi128_t* q,
    fpga_axi128_t* k, fpga_axi128_t* v,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t layout_id, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, uint32_t& result_status_code,
    uint32_t& result_processed_rows, uint64_t& result_perf_cycles_total,
    uint64_t& result_perf_cycles_compute, uint64_t& result_perf_read_beats,
    uint64_t& result_perf_write_beats, uint32_t& result_output_crc,
    uint32_t& result_build_config);

#endif  // QKV_KERNEL_HPP
