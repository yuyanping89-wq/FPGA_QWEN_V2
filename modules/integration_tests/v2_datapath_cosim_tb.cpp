#include "rope.h"
#include "attn.h"
#include <cstdio>
#include <cmath>

// Small valid jobs exercise the delivered RTL, including the final cache layer,
// causal visibility, head layouts, absolute RoPE positions and CRC readback.
// PERF is disabled here: the C model intentionally has no cycle/AXI counters.
// STRICT host-pointer checks are exercised separately in CSim/PS tests: the
// co-simulator relocates buffers and does not preserve 64-byte host alignment.
alignas(64) static kv_axi_t q[7168], output[7168];
#ifdef TEST_M08
alignas(64) static kv_axi_t k[1024], cosine[16384], sine[16384], k_output[1024];
#else
alignas(64) static kv_axi_t k[786432], v[786432];
#endif
static unsigned failed;
static const unsigned short marker = 0xdeadU;

static unsigned short exact_bf16(float f) {
    union { float f; unsigned u; } bits;
    bits.f = f;
    return (unsigned short)((bits.u+0x7fffU+((bits.u>>16)&1U))>>16);
}
static void put(kv_axi_t *p, unsigned n, unsigned short value) {
    p[n/8].range((n%8)*16+15, (n%8)*16) = value;
}
static unsigned short get(const kv_axi_t *p, unsigned n) {
    return (unsigned short)p[n/8].range((n%8)*16+15, (n%8)*16);
}
static void fill(kv_axi_t *p, unsigned beats, unsigned short value) {
    kv_axi_t word = 0;
    for (unsigned lane=0; lane<8; ++lane)
        word.range(lane*16+15,lane*16)=value;
    for (unsigned i=0; i<beats; ++i) p[i]=word;
}
static unsigned crc_update(unsigned crc, unsigned short value) {
    for (unsigned byte=0; byte<2; ++byte) {
        crc ^= (value>>(byte*8))&255U;
        for (unsigned bit=0; bit<8; ++bit)
            crc = (crc>>1)^((crc&1U)?0xedb88320U:0U);
    }
    return crc;
}
static void check(bool condition, const char *label) {
    if (!condition) { ++failed; if (failed<=8) std::printf("FAIL %s\n",label); }
}
static void result_ok(const common_result_v2 &r, unsigned rows, unsigned code) {
    check(r.status_code==0 && r.processed_rows==rows && r.build_config==code,
          "result status/rows/build identity");
    check(!r.perf_cycles_total && !r.perf_cycles_compute && !r.perf_read_beats &&
          !r.perf_write_beats, "PERF disabled returns zero counters");
}

#ifdef TEST_M08
static void run_rope(unsigned rows, unsigned position, bool general=false, unsigned flags=1) {
    fill(q,7168,0); fill(k,1024,0);
    fill(cosine,16384,0); fill(sine,16384,0);
    fill(output,7168,marker); fill(k_output,1024,marker);
    for (unsigned row=0; row<rows; ++row) {
        // Each row has a different table entry. Other absolute positions stay 0.
        for (unsigned dim=0; dim<64; ++dim) {
            put(cosine,(position+row)*64+dim,exact_bf16(general?0.5f:row%2?1.0f:0.0f));
            put(sine,(position+row)*64+dim,exact_bf16(general?0.25f:row%2?0.0f:1.0f));
        }
        for (unsigned head=0; head<14; ++head)
            for (unsigned dim=0; dim<64; ++dim)
                put(q,(head*rows+row)*64+dim,exact_bf16((head+1)*0.25f+row+dim*0.125f));
        for (unsigned head=0; head<2; ++head)
            for (unsigned dim=0; dim<64; ++dim)
                put(k,(head*rows+row)*64+dim,exact_bf16((head+1)*0.5f+row+dim*0.125f));
    }
    common_result_v2 result;
    rope_v2_hls(q,k,cosine,sine,output,k_output,rows==1,rows,position,2048,
                2,201+rows,flags,V2_RESULT_ARGS(result));
    result_ok(result,rows,0x30010011U);
    unsigned crc=0xffffffffU;
    for (unsigned kind=0; kind<2; ++kind) {
        unsigned heads=kind?2:14;
        const kv_axi_t *actual=kind?k_output:output;
        for (unsigned head=0; head<heads; ++head)
            for (unsigned row=0; row<rows; ++row)
                for (unsigned dim=0; dim<64; ++dim) {
                    unsigned other=dim<32?dim+32:dim-32;
                    float own=(head+1)*(kind?0.5f:0.25f)+row+dim*0.125f;
                    float paired=(head+1)*(kind?0.5f:0.25f)+row+other*0.125f;
                    float c=general?0.5f:row%2?1.0f:0.0f;
                    float s=general?0.25f:row%2?0.0f:1.0f;
                    float expected=dim<32?own*c-paired*s:own*c+paired*s;
                    unsigned short b=exact_bf16(expected);
                    check(get(actual,(head*rows+row)*64+dim)==b,"RoPE output layout/value");
                    crc=crc_update(crc,b);
                }
        unsigned used=heads*rows*64, capacity=kind?1024*8:7168*8;
        for (unsigned i=used; i<capacity; ++i)
            check(get(actual,i)==marker,"RoPE output tail remains untouched");
    }
    check(result.output_crc==((flags&1U)?~crc:0U),"RoPE CRC includes Q then K / clears when disabled");
}
#else
static void run_attention(unsigned rows, unsigned layer, unsigned position, unsigned length,
                          bool weighted=false, unsigned flags=1) {
    fill(q,7168,0); fill(k,786432,0); fill(v,786432,0);
    fill(output,7168,marker);
    if (weighted)
        for (unsigned head=0; head<14; ++head)
            for (unsigned row=0; row<rows; ++row)
                for (unsigned dim=0; dim<64; ++dim)
                    put(q,(head*rows+row)*64+dim,exact_bf16((head%3+1)*0.25f));
    for (unsigned head=0; head<2; ++head)
        for (unsigned token=0; token<length; ++token)
            for (unsigned dim=0; dim<64; ++dim) {
                unsigned index=((layer*2+head)*2048+token)*64+dim;
                put(k,index,exact_bf16(weighted?(head+1)*0.125f+token*0.5f:(dim+1)*0.125f));
                put(v,index,exact_bf16((head+1)*2.0f+token*3.0f+(dim%8)*0.125f));
            }
    common_result_v2 result;
    attention_v2_hls(q,k,v,output,rows==1,layer,position,rows,length,0.125f,
                    1,2,301+rows,flags,V2_RESULT_ARGS(result));
    result_ok(result,rows,0x30008011U);
    unsigned crc=0xffffffffU;
    for (unsigned row=0; row<rows; ++row)
        for (unsigned head=0; head<14; ++head)
            for (unsigned dim=0; dim<64; ++dim) {
                unsigned visible=position+row+1;
                double mean_token=(visible-1)*0.5;
                if (weighted) {
                    double denominator=0, numerator=0;
                    double q_value=(head%3+1)*0.25;
                    for (unsigned token=0; token<visible; ++token) {
                        double e=std::exp(4*q_value*((double)token-(visible-1)));
                        denominator+=e; numerator+=e*token;
                    }
                    mean_token=numerator/denominator;
                }
                float expected=(float)((head/7+1)*2.0+mean_token*3+(dim%8)*0.125);
                unsigned short b=exact_bf16(expected);
                unsigned short actual=get(output,row*896+head*64+dim);
                if (weighted) {
                    union {float f; unsigned u;} bits;
                    bits.u=(unsigned)actual<<16;
                    check(std::fabs(bits.f-expected)<=0.006f+0.006f*std::fabs(expected),
                          "nonuniform FP32 softmax / BF16 context reference");
                } else check(actual==b,"attention causal/cache-layer/GQA/ACT_TH value");
                crc=crc_update(crc,actual);
            }
    for (unsigned i=rows*896; i<7168*8; ++i)
        check(get(output,i)==marker,"attention output tail remains untouched");
    check(result.output_crc==((flags&1U)?~crc:0U),"attention ACT_TH CRC / clears when disabled");
}
#endif
int main() {
#ifdef TEST_M08
    run_rope(3,17);
    run_rope(1,2047);
    run_rope(2,63,true,0);
#else
    run_attention(2,0,0,2);
    run_attention(1,23,2,3);
    run_attention(1,23,4,5,true,0);
#endif
    std::printf("V2_DATAPATH_COSIM_TB_%s failures=%u\n",failed?"FAIL":"PASS",failed);
    return failed?1:0;
}
