#ifndef QWEN_ATTENTION_COMMON_V2_H
#define QWEN_ATTENTION_COMMON_V2_H
#include <ap_int.h>
#include <stdint.h>
#include "pspl_v2/v2_abi.h"
#include "common_numeric_v2.h"
typedef ap_uint<128> kv_axi_t;
typedef pspl_v2_common_result_t common_result_v2;
typedef union { float f; uint32_t u; } bits32;
enum { NUM_LAYERS=24, NUM_Q_HEADS=14, NUM_KV_HEADS=2,
       GQA_GROUP_SIZE=7, HEAD_DIM=64, MAX_SEQUENCE=2048, MAX_BLOCK_ROWS=64 };
static float v2_b2f(uint16_t b) {
#pragma HLS INLINE
    return common_numeric_v2::bf16_to_fp32(b);
}
static uint16_t v2_f2b(float f) {
#pragma HLS INLINE
    return common_numeric_v2::fp32_to_bf16_rne(f);
}
static bool v2_finite(float f) {
#pragma HLS INLINE
    bits32 t; t.f=f; return (t.u&0x7f800000U)!=0x7f800000U;
}
// Distinct specializations keep AXI port selection local to each CRC reader.
// HLS 2021.1 generated overlapping Q/K read mux conditions when one shared
// non-inlined function was called with two different output AXI bundles.
template<unsigned AXI_PORT>
static uint32_t v2_crc_words(uint32_t state, const kv_axi_t *p, unsigned beats) {
#pragma HLS INLINE off
    for (unsigned i=0;i<beats;++i) {
        kv_axi_t word=p[i];
        for (unsigned byte=0;byte<16;++byte) {
#pragma HLS PIPELINE II=1
            state^=(uint8_t)(word>>(byte*8));
            for (unsigned bit=0;bit<8;++bit) {
#pragma HLS UNROLL
                state=(state>>1)^((state&1U)?0xedb88320U:0U);
            }
        }
    }
    return state;
}
static uint32_t v2_check_job(unsigned mode, unsigned rows, unsigned abi, unsigned flags) {
#pragma HLS INLINE
    if (abi!=2U) return PSPL_V2_BAD_ABI;
    if (mode>1 || rows==0 || rows>64 || (mode==1 && rows!=1)) return PSPL_V2_BAD_DIMENSION;
    if (flags&~0xBU) return PSPL_V2_BAD_FLAGS;
    return PSPL_V2_OK;
}
#ifndef __SYNTHESIS__
// Physical windows and aliases are checked by the PS driver before AP_START.
// Host pointers have a different address space; only alignment/alias apply here.
static bool v2_overlap(const void *a, uint64_t na, const void *b, uint64_t nb) {
    uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
    return x<y+nb && y<x+na;
}
static bool v2_aligned(const void *a) { return a && !((uintptr_t)a&63U); }
#endif
#define V2_RESULT_DECL uint32_t &result_status_code, uint32_t &result_processed_rows, \
 uint64_t &result_perf_cycles_total, uint64_t &result_perf_cycles_compute, \
 uint64_t &result_perf_read_beats, uint64_t &result_perf_write_beats, \
 uint32_t &result_output_crc, uint32_t &result_build_config
#define V2_RESULT_ARGS(r) (r).status_code,(r).processed_rows,(r).perf_cycles_total, \
 (r).perf_cycles_compute,(r).perf_read_beats,(r).perf_write_beats,(r).output_crc,(r).build_config
#endif
