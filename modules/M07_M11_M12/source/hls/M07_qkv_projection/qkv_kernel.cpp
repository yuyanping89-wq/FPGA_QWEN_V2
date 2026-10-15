// Module 4: Q/K/V Linear projection — HLS kernel (synthesizable).
//
// y[t][o] = sum_h x[t][h] * W[o][h] + b[o]   for q/k/v.
//
// This file compiles two ways:
//   * plain g++  (pragmas ignored) -> fast C simulation against the reference
//   * Vitis HLS  (pragmas active)  -> synthesis / cosim
//
// Design notes for bring-up (correctness first, tune later):
//   * m_axi interfaces for the big buffers, s_axilite for scalars/control.
//   * One reusable inner-product engine `linear_layer`, instantiated for q/k/v.
//   * fp32 accumulator; the H-loop is the pipeline target (II=1 goal).
//   * Weight reuse across T is left to the burst/cache; a later revision can
//     block over T to raise weight reuse and add compute parallelism.

#include "qkv_kernel.hpp"
#include "../../include/fpga_crc32_v2.h"
#include "../../include/fpga_strict_host_v2.h"
#include "../../include/fpga_og32_linear_v2.h"

#define HLS_STRINGIFY_IMPL(x) #x
#define HLS_STRINGIFY(x) HLS_STRINGIFY_IMPL(x)
#define HLS_PRAGMA(x) _Pragma(HLS_STRINGIFY(x))

// Compute y[T,OUT] = x[T,H] @ W[OUT,H]^T + b[OUT].
// Four output channels share each input load. Per-channel fp32 accumulation
// order is unchanged; throughput rises because each mac pass advances OUT_PAR
// independent dot products (same strategy as module10).
static void linear_layer(
    const data_t* x,      // [T, H]
    const data_t* W,      // [OUT, H]
    const data_t* b,      // [OUT]
    data_t* y,            // [T, OUT]
    int T, int OUT) {
row_loop:
    for (int t = 0; t < T; ++t) {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = T_MAX
    out_tile_loop:
        for (int o = 0; o < OUT; o += OUT_PAR) {
#pragma HLS LOOP_TRIPCOUNT min = 1 max = Q_OUT / OUT_PAR
            acc_t acc[OUT_PAR];
#pragma HLS ARRAY_PARTITION variable=acc complete
            for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                acc[lane] = 0.0f;
            }
        mac_loop:
            for (int h = 0; h < H; ++h) {
#pragma HLS PIPELINE II = 1
                const acc_t value = bf16_to_float(x[t * H + h]);
                for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                    acc[lane] += value * bf16_to_float(W[(o + lane) * H + h]);
                }
            }
            for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                y[t * OUT + o + lane] =
                    float_to_bf16(acc[lane] + bf16_to_float(b[o + lane]));
            }
        }
    }
}

void qkv_proj(
    const data_t* x,
    const data_t* Wq, const data_t* bq,
    const data_t* Wk, const data_t* bk,
    const data_t* Wv, const data_t* bv,
    data_t* q, data_t* k, data_t* v,
    int T) {
    // ---- Interfaces (ignored by g++, honored by Vitis HLS) ----
    HLS_PRAGMA(HLS INTERFACE m_axi port = x  offset = slave bundle = gmem0 depth = QKV_X_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = Wq offset = slave bundle = gmem1 depth = QKV_WQ_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = Wk offset = slave bundle = gmem1 depth = QKV_WK_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = Wv offset = slave bundle = gmem1 depth = QKV_WV_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = bq offset = slave bundle = gmem2 depth = QKV_BQ_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = bk offset = slave bundle = gmem2 depth = QKV_BK_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = bv offset = slave bundle = gmem2 depth = QKV_BV_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = q  offset = slave bundle = gmem3 depth = QKV_Q_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = k  offset = slave bundle = gmem3 depth = QKV_K_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port = v  offset = slave bundle = gmem3 depth = QKV_V_DEPTH)
#pragma HLS INTERFACE s_axilite port = x  bundle = control
#pragma HLS INTERFACE s_axilite port = Wq bundle = control
#pragma HLS INTERFACE s_axilite port = bq bundle = control
#pragma HLS INTERFACE s_axilite port = Wk bundle = control
#pragma HLS INTERFACE s_axilite port = bk bundle = control
#pragma HLS INTERFACE s_axilite port = Wv bundle = control
#pragma HLS INTERFACE s_axilite port = bv bundle = control
#pragma HLS INTERFACE s_axilite port = q  bundle = control
#pragma HLS INTERFACE s_axilite port = k  bundle = control
#pragma HLS INTERFACE s_axilite port = v  bundle = control
#pragma HLS INTERFACE s_axilite port = T  bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    linear_layer(x, Wq, bq, q, T, Q_OUT);
    linear_layer(x, Wk, bk, k, T, K_OUT);
    linear_layer(x, Wv, bv, v, T, V_OUT);
}

static void qkv_packed_head_major(const data_t* x, const data_t* W, const data_t* b,
                                  data_t* q, data_t* k, data_t* v, int rows) {
    const int total_out = Q_OUT + K_OUT + V_OUT;
    for (int out0 = 0; out0 < total_out; out0 += OUT_PAR) {
        for (int row = 0; row < rows; ++row) {
                acc_t acc[OUT_PAR];
#pragma HLS ARRAY_PARTITION variable=acc complete
                for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                    acc[lane] = 0.0f;
                }
                for (int h = 0; h < H; ++h) {
#pragma HLS PIPELINE II=1
                    const acc_t value = bf16_to_float(x[row * H + h]);
                    for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                        const int global_out = out0 + lane;
                        const int weight_index = ((global_out / 32) * H + h) * 32 + (global_out % 32);
                        acc[lane] += value * bf16_to_float(W[weight_index]);
                    }
                }
                for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                    const int global_out = out0 + lane;
                    const data_t value = float_to_bf16(acc[lane] + bf16_to_float(b[global_out]));
                    if (global_out < Q_OUT) {
                        const int head = global_out / D, d = global_out % D;
                        q[(head * rows + row) * D + d] = value;
                    } else if (global_out < Q_OUT + K_OUT) {
                        const int local = global_out - Q_OUT;
                        k[((local / D) * rows + row) * D + local % D] = value;
                    } else {
                        const int local = global_out - Q_OUT - K_OUT;
                        v[((local / D) * rows + row) * D + local % D] = value;
                    }
                }
            }
    }
}

void qkv_projection_v2(
    const data_t* x, const data_t* w_qkv, const data_t* bias_qkv,
    data_t* q, data_t* k, data_t* v,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t layout_id,
    uint32_t abi_version, uint32_t job_id, uint32_t flags,
    common_result_v2* result) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=x offset=slave bundle=gmem0 depth=QKV_X_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=w_qkv offset=slave bundle=gmem1 depth=(QKV_WQ_DEPTH + QKV_WK_DEPTH + QKV_WV_DEPTH))
    HLS_PRAGMA(HLS INTERFACE m_axi port=bias_qkv offset=slave bundle=gmem2 depth=(QKV_BQ_DEPTH + QKV_BK_DEPTH + QKV_BV_DEPTH))
    HLS_PRAGMA(HLS INTERFACE m_axi port=q offset=slave bundle=gmem3 depth=QKV_Q_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=k offset=slave bundle=gmem3 depth=QKV_K_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=v offset=slave bundle=gmem3 depth=QKV_V_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=result offset=slave bundle=gmem4 depth=1)
#pragma HLS INTERFACE s_axilite port=x bundle=control
#pragma HLS INTERFACE s_axilite port=w_qkv bundle=control
#pragma HLS INTERFACE s_axilite port=bias_qkv bundle=control
#pragma HLS INTERFACE s_axilite port=q bundle=control
#pragma HLS INTERFACE s_axilite port=k bundle=control
#pragma HLS INTERFACE s_axilite port=v bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=hidden_size bundle=control
#pragma HLS INTERFACE s_axilite port=layout_id bundle=control
#pragma HLS INTERFACE s_axilite port=abi_version bundle=control
#pragma HLS INTERFACE s_axilite port=job_id bundle=control
#pragma HLS INTERFACE s_axilite port=flags bundle=control
#pragma HLS INTERFACE s_axilite port=result bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
    (void)job_id;
    common_job_v2 job = {abi_version, job_id, run_mode, valid_rows, flags};
    uint32_t status = fpga_validate_job_v2(&job);
    if (status == FPGA_STATUS_V2_OK && hidden_size != (uint32_t)H) {
        status = FPGA_STATUS_V2_BAD_DIMENSION;
    }
    if (status == FPGA_STATUS_V2_OK && layout_id != FPGA_LAYOUT_W_OG32_I_L32_V2) {
        status = FPGA_STATUS_V2_BAD_LAYOUT;
    }
    /* Buffer address/range validation belongs to the PS/M02 descriptor path.
       Keep the defensive null check for host CSim, but do not synthesize
       comparisons against pointers that may be local ABI objects. */
#ifndef __SYNTHESIS__
    if (status == FPGA_STATUS_V2_OK &&
        (x == 0 || w_qkv == 0 || bias_qkv == 0 || q == 0 || k == 0 ||
         v == 0 || result == 0)) {
        status = FPGA_STATUS_V2_BAD_ADDRESS_RANGE;
    }
#endif
    /* Pointer aliasing is checked by the PS descriptor planner. Host CSim
       keeps the defensive check; HLS does not support pointer comparisons. */
#ifndef __SYNTHESIS__
    if (status == FPGA_STATUS_V2_OK && (q == k || q == v || k == v || q == x || k == x || v == x)) {
        status = FPGA_STATUS_V2_BAD_ALIAS;
    }
#endif
    fpga_result_init_v2(result, status, valid_rows, 0x02070401U);
    if (status != FPGA_STATUS_V2_OK) return;

    qkv_packed_head_major(x, w_qkv, bias_qkv, q, k, v, (int)valid_rows);
    if (flags & FPGA_FLAG_ENABLE_OUTPUT_CRC) {
        uint32_t crc = 0xffffffffU;
        crc = fpga_crc32_bf16_range_v2(crc, q, valid_rows * Q_OUT);
        crc = fpga_crc32_bf16_range_v2(crc, k, valid_rows * K_OUT);
        crc = fpga_crc32_bf16_range_v2(crc, v, valid_rows * V_OUT);
        result->output_crc = ~crc;
    }
}

static void qkv_pack_result(const common_result_v2& result, uint64_t* words) {
    words[0] = ((uint64_t)result.processed_rows << 32) | result.status_code;
    words[1] = result.perf_cycles_total;
    words[2] = result.perf_cycles_compute;
    words[3] = result.perf_read_beats;
    words[4] = result.perf_write_beats;
    words[5] = ((uint64_t)result.build_config << 32) | result.output_crc;
    words[6] = 0U;
    words[7] = 0U;
}

/* Burst implementation used by the hardware top.  Same validation and
 * bit-exact results as qkv_projection_v2(); see fpga_og32_linear_v2.h. */
#define QKV_BUILD_CONFIG_V2 (0x40008001U | (OG32_ROW_PAR << 4)) /* rev4, clk100, H_TILE=32, rows=4, 8 lanes */
constexpr int QKV_TOTAL = Q_OUT + K_OUT + V_OUT;
static_assert(QKV_TOTAL % 32 == 0 && D % OG32_LANES == 0, "OG32 core shape");

/* Head-major write of one row block: dst[(head*rows + row)*D + d]. */
static void qkv_write_heads(fpga_axi128_t* dst, const float obuf[OG32_ROW_PAR][QKV_TOTAL],
                            uint32_t o_base, uint32_t heads, uint32_t rows,
                            uint32_t row0, uint32_t nr, const float *bias) {
#pragma HLS INLINE
    const uint32_t beats_per_row = (uint32_t)(D / OG32_LANES);
write_heads:
    for (uint32_t h = 0; h < heads; ++h) {
#pragma HLS LOOP_TRIPCOUNT min=2 max=14
        const uint32_t nbeats = nr * beats_per_row;
        const uint32_t dst_base = (h * rows + row0) * beats_per_row;
        uint32_t rr = 0, b = 0;
    write_head_rows:
        for (uint32_t t = 0; t < nbeats; ++t) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=8 max=32
            uint16_t val[OG32_LANES];
            for (int lane = 0; lane < OG32_LANES; ++lane) {
#pragma HLS UNROLL
                const uint32_t o = o_base + h * (uint32_t)D + b * OG32_LANES + (uint32_t)lane;
                val[lane] = og32_f2b(obuf[rr][o] + bias[o]);
            }
            dst[dst_base + t] = og32_pack_beat(val);
            if (++b == beats_per_row) { b = 0; ++rr; }
        }
    }
}

static void qkv_burst(const fpga_axi128_t* x, const fpga_axi128_t* w_qkv,
                      const fpga_axi128_t* bias_qkv, fpga_axi128_t* q,
                      fpga_axi128_t* k, fpga_axi128_t* v, uint32_t rows) {
    fpga_axi128_t xbuf[OG32_ROW_PAR][OG32_RB_MAX][H / OG32_LANES];
#pragma HLS ARRAY_PARTITION variable=xbuf complete dim=1
    float obuf[OG32_ROW_PAR][QKV_TOTAL];
#pragma HLS ARRAY_PARTITION variable=obuf complete dim=1
#pragma HLS ARRAY_PARTITION variable=obuf cyclic factor=8 dim=2
#pragma HLS BIND_STORAGE variable=obuf type=ram_s2p impl=bram
    float bias[QKV_TOTAL];
#pragma HLS ARRAY_PARTITION variable=bias cyclic factor=8 dim=1
load_bias:
    for (uint32_t t = 0; t < (uint32_t)(QKV_TOTAL / OG32_LANES); ++t) {
#pragma HLS PIPELINE II=1
        const fpga_axi128_t beat = bias_qkv[t];
        for (int lane = 0; lane < OG32_LANES; ++lane) {
#pragma HLS UNROLL
            bias[t * OG32_LANES + lane] = og32_b2f(fpga_axi128_get_bf16(beat, (uint32_t)lane));
        }
    }
    og32_load_x<H>(x, xbuf, rows);
    const uint32_t nblocks = (rows + OG32_ROW_PAR - 1U) / OG32_ROW_PAR;
row_blocks:
    for (uint32_t rb = 0; rb < nblocks; ++rb) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=16
        og32_compute_block<H, QKV_TOTAL / 32, true>(w_qkv, xbuf, rb, bias, obuf);
        const uint32_t row0 = rb * OG32_ROW_PAR;
        const uint32_t nr = rows - row0 < OG32_ROW_PAR ? rows - row0 : OG32_ROW_PAR;
        qkv_write_heads(q, obuf, 0U, (uint32_t)A, rows, row0, nr, bias);
        qkv_write_heads(k, obuf, (uint32_t)Q_OUT, (uint32_t)KVH, rows, row0, nr, bias);
        qkv_write_heads(v, obuf, (uint32_t)(Q_OUT + K_OUT), (uint32_t)KVH, rows, row0, nr, bias);
    }
}

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
    uint32_t& result_build_config) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=x offset=slave bundle=gmem0 depth=(QKV_X_DEPTH / 8) max_read_burst_length=64 num_read_outstanding=8 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=w_qkv offset=slave bundle=gmem1 depth=((QKV_WQ_DEPTH + QKV_WK_DEPTH + QKV_WV_DEPTH) / 8) max_read_burst_length=64 num_read_outstanding=16 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=bias_qkv offset=slave bundle=gmem2 depth=((QKV_BQ_DEPTH + QKV_BK_DEPTH + QKV_BV_DEPTH) / 8) max_read_burst_length=64 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=q offset=slave bundle=gmem3 depth=(QKV_Q_DEPTH / 8) max_write_burst_length=64 num_write_outstanding=8 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=k offset=slave bundle=gmem4 depth=(QKV_K_DEPTH / 8) max_write_burst_length=64 num_write_outstanding=8 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=v offset=slave bundle=gmem5 depth=(QKV_V_DEPTH / 8) max_write_burst_length=64 num_write_outstanding=8 latency=64)
#pragma HLS INTERFACE s_axilite port=x bundle=control
#pragma HLS INTERFACE s_axilite port=w_qkv bundle=control
#pragma HLS INTERFACE s_axilite port=bias_qkv bundle=control
#pragma HLS INTERFACE s_axilite port=q bundle=control
#pragma HLS INTERFACE s_axilite port=k bundle=control
#pragma HLS INTERFACE s_axilite port=v bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=hidden_size bundle=control
#pragma HLS INTERFACE s_axilite port=layout_id bundle=control
#pragma HLS INTERFACE s_axilite port=abi_version bundle=control
#pragma HLS INTERFACE s_axilite port=job_id bundle=control
#pragma HLS INTERFACE s_axilite port=flags bundle=control
#pragma HLS INTERFACE s_axilite port=result_status_code bundle=control
#pragma HLS INTERFACE s_axilite port=result_processed_rows bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_cycles_total bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_cycles_compute bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_read_beats bundle=control
#pragma HLS INTERFACE s_axilite port=result_perf_write_beats bundle=control
#pragma HLS INTERFACE s_axilite port=result_output_crc bundle=control
#pragma HLS INTERFACE s_axilite port=result_build_config bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
    common_job_v2 job = {abi_version, job_id, run_mode, valid_rows, flags};
    uint32_t status = fpga_validate_job_v2(&job);
    if (status == FPGA_STATUS_V2_OK && hidden_size != (uint32_t)H) {
        status = FPGA_STATUS_V2_BAD_DIMENSION;
    }
    if (status == FPGA_STATUS_V2_OK && layout_id != FPGA_LAYOUT_W_OG32_I_L32_V2) {
        status = FPGA_STATUS_V2_BAD_LAYOUT;
    }
    common_result_v2 result;
#ifndef __SYNTHESIS__
    if (status == FPGA_STATUS_V2_OK && (flags & FPGA_FLAG_STRICT_CHECK)) {
        const fpga_host_range_v2 inputs[] = {{x,(uint64_t)valid_rows*H*2U},{w_qkv,(uint64_t)QKV_TOTAL*H*2U},{bias_qkv,(uint64_t)QKV_TOTAL*2U}};
        const fpga_host_range_v2 outputs[] = {{q,(uint64_t)valid_rows*Q_OUT*2U},{k,(uint64_t)valid_rows*K_OUT*2U},{v,(uint64_t)valid_rows*V_OUT*2U}};
        status = fpga_strict_host_ranges_v2(inputs,3,outputs,3);
    }
#endif
    fpga_result_init_v2(&result, status, valid_rows, QKV_BUILD_CONFIG_V2);
    if (status == FPGA_STATUS_V2_OK) {
        qkv_burst(x, w_qkv, bias_qkv, q, k, v, valid_rows);
        if (flags & FPGA_FLAG_ENABLE_OUTPUT_CRC) {
            uint32_t crc = 0xffffffffU;
            crc = fpga_crc32_axi128_range_v2(crc, q, valid_rows * Q_OUT / OG32_LANES);
            crc = fpga_crc32_axi128_range_v2(crc, k, valid_rows * K_OUT / OG32_LANES);
            crc = fpga_crc32_axi128_range_v2(crc, v, valid_rows * V_OUT / OG32_LANES);
            result.output_crc = ~crc;
        }
        /* Actual cycles and AXI handshake beats are supplied by the RTL
         * measurement wrapper. HLS alone reports unavailable as zero. */
    }
    result_status_code = result.status_code;
    result_processed_rows = result.processed_rows;
    result_perf_cycles_total = result.perf_cycles_total;
    result_perf_cycles_compute = result.perf_cycles_compute;
    result_perf_read_beats = result.perf_read_beats;
    result_perf_write_beats = result.perf_write_beats;
    result_output_crc = result.output_crc;
    result_build_config = result.build_config;
}
