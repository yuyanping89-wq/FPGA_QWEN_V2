#include "attention_output_kernel.hpp"
#include "../../include/fpga_crc32_v2.h"
#include "../../include/fpga_strict_host_v2.h"
#include "../../include/fpga_og32_linear_v2.h"

#define HLS_STRINGIFY_IMPL(x) #x
#define HLS_STRINGIFY(x) HLS_STRINGIFY_IMPL(x)
#define HLS_PRAGMA(x) _Pragma(HLS_STRINGIFY(x))

void attention_output_projection(
    const data_t* x,
    const data_t* weight,
    data_t* y,
    int T) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=x offset=slave bundle=gmem0 depth=ATTN_OUT_X_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=weight offset=slave bundle=gmem1 depth=ATTN_OUT_W_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=y offset=slave bundle=gmem2 depth=ATTN_OUT_Y_DEPTH)
#pragma HLS INTERFACE s_axilite port=x bundle=control
#pragma HLS INTERFACE s_axilite port=weight bundle=control
#pragma HLS INTERFACE s_axilite port=y bundle=control
#pragma HLS INTERFACE s_axilite port=T bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

row_loop:
    for (int row = 0; row < T; ++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=T_MAX
    output_tile_loop:
        for (int out = 0; out < H; out += OUT_PAR) {
            // Compute four output channels together. The fp32 accumulator
            // recurrence remains II=4, but each input value and loop pass
            // advances four independent dot products.
            acc_t acc[OUT_PAR];
#pragma HLS ARRAY_PARTITION variable=acc complete
            for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                acc[lane] = 0.0f;
            }
        input_loop:
            for (int in = 0; in < H; ++in) {
#pragma HLS PIPELINE II=1
                const acc_t value = bf16_to_float(x[row * H + in]);
                for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                    acc[lane] += value * bf16_to_float(weight[(out + lane) * H + in]);
                }
            }
            for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                y[row * H + out + lane] = float_to_bf16(acc[lane]);
            }
        }
    }
}

void attention_out_v2(
    const data_t* x, const data_t* weight, data_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t layout_id, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, common_result_v2* result) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=x offset=slave bundle=gmem0 depth=ATTN_OUT_X_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=weight offset=slave bundle=gmem1 depth=ATTN_OUT_W_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=y offset=slave bundle=gmem2 depth=ATTN_OUT_Y_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=result offset=slave bundle=gmem3 depth=1)
#pragma HLS INTERFACE s_axilite port=x bundle=control
#pragma HLS INTERFACE s_axilite port=weight bundle=control
#pragma HLS INTERFACE s_axilite port=y bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=hidden_size bundle=control
#pragma HLS INTERFACE s_axilite port=layout_id bundle=control
#pragma HLS INTERFACE s_axilite port=abi_version bundle=control
#pragma HLS INTERFACE s_axilite port=job_id bundle=control
#pragma HLS INTERFACE s_axilite port=flags bundle=control
#pragma HLS INTERFACE s_axilite port=result bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
    common_job_v2 job = {abi_version, job_id, run_mode, valid_rows, flags};
    uint32_t status = fpga_validate_job_v2(&job);
    if (status == FPGA_STATUS_V2_OK && hidden_size != (uint32_t)H) {
        status = FPGA_STATUS_V2_BAD_DIMENSION;
    }
    if (status == FPGA_STATUS_V2_OK && layout_id != FPGA_LAYOUT_W_OG32_I_L32_V2) {
        status = FPGA_STATUS_V2_BAD_LAYOUT;
    }
#ifndef __SYNTHESIS__
    /* Buffer address/range validation belongs to the PS/M02 descriptor path.
       Comparing C pointers in synthesized RTL is unsafe here because result is
       a local ABI object owned by the top-level wrapper. */
    if (status == FPGA_STATUS_V2_OK && (x == 0 || weight == 0 || y == 0 || result == 0)) {
        status = FPGA_STATUS_V2_BAD_ADDRESS_RANGE;
    }
#endif
/* Alias policy is enforced by the PS buffer planner; pointer comparison is
   intentionally host-simulation-only because Vitis HLS cannot synthesize it. */
#ifndef __SYNTHESIS__
    if (status == FPGA_STATUS_V2_OK && y == x) {
        status = FPGA_STATUS_V2_BAD_ALIAS;
    }
#endif
    fpga_result_init_v2(result, status, valid_rows, 0x020B0401U);
    if (status != FPGA_STATUS_V2_OK) return;

    /* Packed Wo layout: [ceil(O/32)][I][32], lane is the output index. */
    for (uint32_t row = 0; row < valid_rows; ++row) {
        for (uint32_t out0 = 0; out0 < (uint32_t)H; out0 += OUT_PAR) {
            acc_t acc[OUT_PAR];
#pragma HLS ARRAY_PARTITION variable=acc complete
            for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                acc[lane] = 0.0f;
            }
            for (uint32_t in = 0; in < (uint32_t)H; ++in) {
#pragma HLS PIPELINE II=1
                const acc_t value = bf16_to_float(x[row * H + in]);
                for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                    const uint32_t global_out = out0 + (uint32_t)lane;
                    const uint32_t weight_index = (global_out / 32U * H + in) * 32U + (global_out % 32U);
                    acc[lane] += value * bf16_to_float(weight[weight_index]);
                }
            }
            for (int lane = 0; lane < OUT_PAR; ++lane) {
#pragma HLS UNROLL
                y[row * H + out0 + lane] = float_to_bf16(acc[lane]);
            }
        }
    }
    if (flags & FPGA_FLAG_ENABLE_OUTPUT_CRC) {
        result->output_crc = ~fpga_crc32_bf16_range_v2(0xffffffffU, y, valid_rows * H);
    }
}

static void attention_pack_result(const common_result_v2& result, uint64_t* words) {
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
 * bit-exact results as attention_out_v2(); see fpga_og32_linear_v2.h. */
#define ATTN_OUT_BUILD_CONFIG_V2 (0x40008001U | (OG32_ROW_PAR << 4)) /* rev4, clk100, H_TILE=32, rows=4, 8 lanes */
static void attention_out_burst(const fpga_axi128_t* x, const fpga_axi128_t* weight,
                                fpga_axi128_t* y, uint32_t rows) {
    static_assert(H % 32 == 0, "OG32 core needs H % 32 == 0");
    fpga_axi128_t xbuf[OG32_ROW_PAR][OG32_RB_MAX][H / OG32_LANES];
#pragma HLS ARRAY_PARTITION variable=xbuf complete dim=1
    float obuf[OG32_ROW_PAR][H];
#pragma HLS ARRAY_PARTITION variable=obuf complete dim=1
#pragma HLS ARRAY_PARTITION variable=obuf cyclic factor=8 dim=2
#pragma HLS BIND_STORAGE variable=obuf type=ram_s2p impl=bram
    og32_load_x<H>(x, xbuf, rows);
    const uint32_t nblocks = (rows + OG32_ROW_PAR - 1U) / OG32_ROW_PAR;
row_blocks:
    for (uint32_t rb = 0; rb < nblocks; ++rb) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=16
        og32_compute_block<H, H / 32, false>(weight, xbuf, rb, 0, obuf);
        const uint32_t nr = rows - rb * OG32_ROW_PAR < OG32_ROW_PAR ? rows - rb * OG32_ROW_PAR : OG32_ROW_PAR;
        const uint32_t nbeats = nr * (uint32_t)(H / OG32_LANES);
        uint32_t rr = 0, ob = 0;
    write_y:
        for (uint32_t t = 0; t < nbeats; ++t) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=112 max=448
            uint16_t v[OG32_LANES];
            for (int lane = 0; lane < OG32_LANES; ++lane) {
#pragma HLS UNROLL
                v[lane] = og32_f2b(obuf[rr][ob * OG32_LANES + lane]);
            }
            y[rb * OG32_ROW_PAR * (H / OG32_LANES) + t] = og32_pack_beat(v);
            if (++ob == (uint32_t)(H / OG32_LANES)) { ob = 0; ++rr; }
        }
    }
}

void attention_out_v2_hls(
    const fpga_axi128_t* x, const fpga_axi128_t* weight, fpga_axi128_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t layout_id, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, uint32_t& result_status_code,
    uint32_t& result_processed_rows, uint64_t& result_perf_cycles_total,
    uint64_t& result_perf_cycles_compute, uint64_t& result_perf_read_beats,
    uint64_t& result_perf_write_beats, uint32_t& result_output_crc,
    uint32_t& result_build_config) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=x offset=slave bundle=gmem0 depth=(ATTN_OUT_X_DEPTH / 8) max_read_burst_length=64 num_read_outstanding=8 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=weight offset=slave bundle=gmem1 depth=(ATTN_OUT_W_DEPTH / 8) max_read_burst_length=64 num_read_outstanding=16 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=y offset=slave bundle=gmem2 depth=(ATTN_OUT_Y_DEPTH / 8) max_write_burst_length=64 num_write_outstanding=8 latency=64)
#pragma HLS INTERFACE s_axilite port=x bundle=control
#pragma HLS INTERFACE s_axilite port=weight bundle=control
#pragma HLS INTERFACE s_axilite port=y bundle=control
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
        const fpga_host_range_v2 inputs[] = {{x,(uint64_t)valid_rows*H*2U},{weight,(uint64_t)H*H*2U}};
        const fpga_host_range_v2 outputs[] = {{y,(uint64_t)valid_rows*H*2U}};
        status = fpga_strict_host_ranges_v2(inputs,2,outputs,1);
    }
#endif
    fpga_result_init_v2(&result, status, valid_rows, ATTN_OUT_BUILD_CONFIG_V2);
    if (status == FPGA_STATUS_V2_OK) {
        attention_out_burst(x, weight, y, valid_rows);
        if (flags & FPGA_FLAG_ENABLE_OUTPUT_CRC) {
            result.output_crc = ~fpga_crc32_axi128_range_v2(0xffffffffU, y, valid_rows * H / OG32_LANES);
        }
        /* Actual measurements are injected by the RTL wrapper. */
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
