#include "residual_add_kernel.hpp"

#define HLS_STRINGIFY_IMPL(x) #x
#define HLS_STRINGIFY(x) HLS_STRINGIFY_IMPL(x)
#define HLS_PRAGMA(x) _Pragma(HLS_STRINGIFY(x))

void attention_residual_add(
    const data_t* residual,
    const data_t* attention,
    data_t* output,
    int T) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=residual offset=slave bundle=gmem0 depth=RESIDUAL_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=attention offset=slave bundle=gmem1 depth=RESIDUAL_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=output offset=slave bundle=gmem2 depth=RESIDUAL_DEPTH)
#pragma HLS INTERFACE s_axilite port=residual bundle=control
#pragma HLS INTERFACE s_axilite port=attention bundle=control
#pragma HLS INTERFACE s_axilite port=output bundle=control
#pragma HLS INTERFACE s_axilite port=T bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    const int count = T * H;
add_loop:
    for (int index = 0; index < count; ++index) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=1 max=DEPTH
        const float residual_f32 = bf16_to_float(residual[index]);
        const float attention_f32 = bf16_to_float(attention[index]);
        output[index] = float_to_bf16(residual_f32 + attention_f32);
    }
}

void residual_add_v2(
    const data_t* a, const data_t* b, data_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t residual_role, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, common_result_v2* result) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=a offset=slave bundle=gmem0 depth=RESIDUAL_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=b offset=slave bundle=gmem1 depth=RESIDUAL_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=y offset=slave bundle=gmem2 depth=RESIDUAL_DEPTH)
    HLS_PRAGMA(HLS INTERFACE m_axi port=result offset=slave bundle=gmem3 depth=1)
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=b bundle=control
#pragma HLS INTERFACE s_axilite port=y bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=hidden_size bundle=control
#pragma HLS INTERFACE s_axilite port=residual_role bundle=control
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
    if (status == FPGA_STATUS_V2_OK && residual_role > 1U) {
        status = FPGA_STATUS_V2_BAD_DIMENSION;
    }
    /* Buffer address/range validation belongs to the PS/M02 descriptor path.
       Keep the defensive null check for host CSim, but do not synthesize
       comparisons against pointers that may be local ABI objects. */
#ifndef __SYNTHESIS__
    if (status == FPGA_STATUS_V2_OK && (a == 0 || b == 0 || y == 0 || result == 0)) {
        status = FPGA_STATUS_V2_BAD_ADDRESS_RANGE;
    }
#endif
/* Alias policy is enforced by the PS buffer planner; pointer comparison is
   intentionally host-simulation-only because Vitis HLS cannot synthesize it. */
#ifndef __SYNTHESIS__
    if (status == FPGA_STATUS_V2_OK && (y == a || y == b)) {
        status = FPGA_STATUS_V2_BAD_ALIAS;
    }
#endif
    fpga_result_init_v2(result, status, valid_rows, 0x020C0101U);
    if (status != FPGA_STATUS_V2_OK) return;

    const uint32_t count = valid_rows * (uint32_t)H;
    for (uint32_t index = 0; index < count; ++index) {
#pragma HLS PIPELINE II=1
        y[index] = float_to_bf16(bf16_to_float(a[index]) + bf16_to_float(b[index]));
    }
    result->perf_cycles_compute = count;
    result->perf_cycles_total = count;
    result->perf_read_beats = ((uint64_t)count * 2U + 7U) / 8U;
    result->perf_write_beats = ((uint64_t)count + 7U) / 8U;
}

static void pack_result_words(const common_result_v2& result,
                              uint64_t* result_words) {
    result_words[0] = ((uint64_t)result.processed_rows << 32) |
                      (uint64_t)result.status_code;
    result_words[1] = result.perf_cycles_total;
    result_words[2] = result.perf_cycles_compute;
    result_words[3] = result.perf_read_beats;
    result_words[4] = result.perf_write_beats;
    result_words[5] = ((uint64_t)result.build_config << 32) |
                      (uint64_t)result.output_crc;
    result_words[6] = 0U;
    result_words[7] = 0U;
}

/* Burst implementation used by the hardware top: one 128-bit beat (8 BF16)
 * from a and b per cycle, 8 FP32 adds, one beat to y.  Same validation and
 * bit-exact results as residual_add_v2(). */
#define RESIDUAL_BUILD_CONFIG_V2 0x20010001U /* rev2, clk100, burst=64 beats, 8 lanes */
static void residual_add_burst(const fpga_axi128_t* a, const fpga_axi128_t* b,
                               fpga_axi128_t* y, uint32_t rows) {
    const uint32_t nbeats = rows * (uint32_t)(H / 8);
add_beats:
    for (uint32_t t = 0; t < nbeats; ++t) {
#pragma HLS PIPELINE II=1
#pragma HLS LOOP_TRIPCOUNT min=112 max=7168
        const fpga_axi128_t av = a[t];
        const fpga_axi128_t bv = b[t];
        fpga_axi128_t out;
#if !defined(__SYNTHESIS__) && !defined(FPGA_USE_AP_INT)
        out.lo = 0; out.hi = 0;
#endif
        for (uint32_t lane = 0; lane < 8U; ++lane) {
#pragma HLS UNROLL
            const float sum = bf16_to_float(fpga_axi128_get_bf16(av, lane)) +
                              bf16_to_float(fpga_axi128_get_bf16(bv, lane));
            fpga_axi128_set_bf16(out, lane, float_to_bf16(sum));
        }
        y[t] = out;
    }
}

void residual_add_v2_hls(
    const fpga_axi128_t* a, const fpga_axi128_t* b, fpga_axi128_t* y,
    uint32_t run_mode, uint32_t valid_rows, uint32_t hidden_size,
    uint32_t residual_role, uint32_t abi_version, uint32_t job_id,
    uint32_t flags, uint32_t& result_status_code,
    uint32_t& result_processed_rows, uint64_t& result_perf_cycles_total,
    uint64_t& result_perf_cycles_compute, uint64_t& result_perf_read_beats,
    uint64_t& result_perf_write_beats, uint32_t& result_output_crc,
    uint32_t& result_build_config) {
    HLS_PRAGMA(HLS INTERFACE m_axi port=a offset=slave bundle=gmem0 depth=(RESIDUAL_DEPTH / 8) max_read_burst_length=64 num_read_outstanding=8 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=b offset=slave bundle=gmem1 depth=(RESIDUAL_DEPTH / 8) max_read_burst_length=64 num_read_outstanding=8 latency=64)
    HLS_PRAGMA(HLS INTERFACE m_axi port=y offset=slave bundle=gmem2 depth=(RESIDUAL_DEPTH / 8) max_write_burst_length=64 num_write_outstanding=8 latency=64)
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=b bundle=control
#pragma HLS INTERFACE s_axilite port=y bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=hidden_size bundle=control
#pragma HLS INTERFACE s_axilite port=residual_role bundle=control
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
    if (status == FPGA_STATUS_V2_OK && residual_role > 1U) {
        status = FPGA_STATUS_V2_BAD_DIMENSION;
    }
    common_result_v2 result;
    fpga_result_init_v2(&result, status, valid_rows, RESIDUAL_BUILD_CONFIG_V2);
    if (status == FPGA_STATUS_V2_OK) {
        residual_add_burst(a, b, y, valid_rows);
        const uint64_t beats = (uint64_t)valid_rows * H / 8U;
        result.perf_cycles_compute = beats;
        result.perf_cycles_total = beats;
        result.perf_read_beats = beats * 2U;
        result.perf_write_beats = beats;
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
