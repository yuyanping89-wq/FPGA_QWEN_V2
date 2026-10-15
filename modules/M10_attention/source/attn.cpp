#include "attn.h"
#include <hls_math.h>
static float sum8(const float a[8]) {
#pragma HLS INLINE
    return ((a[0]+a[1])+(a[2]+a[3]))+((a[4]+a[5])+(a[6]+a[7]));
}
void attention_v2_hls(const kv_axi_t *q_rot,const kv_axi_t *k_cache,const kv_axi_t *v_cache,kv_axi_t *attn_out,
 uint32_t run_mode,uint32_t layer_id,uint32_t query_pos_base,uint32_t valid_rows,uint32_t cache_len,
 float scale,uint32_t causal,uint32_t abi_version,uint32_t job_id,uint32_t flags,V2_RESULT_DECL) {
#pragma HLS INTERFACE m_axi port=q_rot offset=slave bundle=gmem0 depth=7168 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=k_cache offset=slave bundle=gmem1 depth=786432 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=v_cache offset=slave bundle=gmem2 depth=786432 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=attn_out offset=slave bundle=gmem3 depth=7168 max_read_burst_length=64 max_write_burst_length=64 num_read_outstanding=2 num_write_outstanding=4
#pragma HLS INTERFACE s_axilite port=q_rot bundle=control
#pragma HLS INTERFACE s_axilite port=k_cache bundle=control
#pragma HLS INTERFACE s_axilite port=v_cache bundle=control
#pragma HLS INTERFACE s_axilite port=attn_out bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=layer_id bundle=control
#pragma HLS INTERFACE s_axilite port=query_pos_base bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=cache_len bundle=control
#pragma HLS INTERFACE s_axilite port=scale bundle=control
#pragma HLS INTERFACE s_axilite port=causal bundle=control
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
    (void)job_id;
    result_status_code=v2_check_job(run_mode,valid_rows,abi_version,flags);
    result_processed_rows=0; result_perf_cycles_total=0; result_perf_cycles_compute=0;
    result_perf_read_beats=0; result_perf_write_beats=0; result_output_crc=0;
    result_build_config=0x30008011U;
    if (result_status_code) return;
    if (layer_id>=24) {result_status_code=PSPL_V2_BAD_LAYER;return;}
    if (!cache_len || cache_len>2048 || causal>1) {result_status_code=PSPL_V2_BAD_DIMENSION;return;}
    if (query_pos_base>=cache_len || valid_rows>cache_len-query_pos_base) {result_status_code=PSPL_V2_BAD_TOKEN;return;}
    if (!v2_finite(scale) || !(scale>0)) {result_status_code=PSPL_V2_NUMERIC_ERROR;return;}
#ifndef __SYNTHESIS__
    if (flags&8U) {
        const void *p[]={q_rot,k_cache,v_cache,attn_out};
        const uint64_t n[]={valid_rows*1792ULL,24ULL*2*2048*128,24ULL*2*2048*128,valid_rows*1792ULL};
        for (unsigned i=0;i<4;++i) if (!v2_aligned(p[i])) {result_status_code=PSPL_V2_BAD_ALIGNMENT;return;}
        for (unsigned j=0;j<3;++j) if (v2_overlap(p[3],n[3],p[j],n[j])) {result_status_code=PSPL_V2_BAD_ALIAS;return;}
    }
#endif
    float q[64],acc[64],scores[2048],exps[2048],partial[8];
#pragma HLS ARRAY_PARTITION variable=q cyclic factor=8
#pragma HLS ARRAY_PARTITION variable=acc cyclic factor=8
#pragma HLS ARRAY_PARTITION variable=partial complete
#pragma HLS BIND_STORAGE variable=scores type=ram_s2p impl=bram
#pragma HLS BIND_STORAGE variable=exps type=ram_s2p impl=bram
    for (unsigned row=0;row<valid_rows;++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=64
        const unsigned visible=causal?(query_pos_base+row+1):cache_len;
        for (unsigned head=0;head<14;++head) {
            const unsigned base=(layer_id*2+head/7)*2048*8;
            for (unsigned b=0;b<8;++b) {
#pragma HLS PIPELINE II=1
                kv_axi_t w=q_rot[(head*valid_rows+row)*8+b];
                for (unsigned l=0;l<8;++l) {
#pragma HLS UNROLL
                    q[b*8+l]=v2_b2f((uint16_t)(w>>(16*l))); acc[b*8+l]=0;
                }
            }
            float max_score=0;
        SCORE_KEYS:
            for (unsigned key=0;key<visible;++key) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=2048
                for (unsigned b=0;b<8;++b) {
#pragma HLS PIPELINE II=1
                    kv_axi_t w=k_cache[base+key*8+b];
                    float product[8];
#pragma HLS ARRAY_PARTITION variable=product complete
                    for (unsigned l=0;l<8;++l) {
#pragma HLS UNROLL
                        product[l]=q[b*8+l]*v2_b2f((uint16_t)(w>>(16*l)));
                    }
                    partial[b]=sum8(product);
                }
                float score=sum8(partial)*scale;
                if (!v2_finite(score)) {result_status_code=PSPL_V2_NUMERIC_ERROR;return;}
                scores[key]=score;
                if (!key || score>max_score) max_score=score;
            }
            float sum=0;
        SOFTMAX_KEYS:
            for (unsigned key=0;key<visible;++key) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=2048
                float e=hls::expf(scores[key]-max_score);
                exps[key]=e; sum+=e;
            }
            if (!v2_finite(sum) || !(sum>0)) {result_status_code=PSPL_V2_NUMERIC_ERROR;return;}
            const float factor=1.0f/sum;
        VALUE_KEYS:
            for (unsigned key=0;key<visible;++key) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=2048
                float weight=exps[key]*factor;
                for (unsigned b=0;b<8;++b) {
#pragma HLS PIPELINE II=1
                    kv_axi_t w=v_cache[base+key*8+b];
                    for (unsigned l=0;l<8;++l) {
#pragma HLS UNROLL
                        unsigned d=b*8+l;
                        acc[d]=acc[d]+weight*v2_b2f((uint16_t)(w>>(16*l)));
                    }
                }
            }
            for (unsigned b=0;b<8;++b) {
#pragma HLS PIPELINE II=1
                kv_axi_t w=0;
                for (unsigned l=0;l<8;++l) {
#pragma HLS UNROLL
                    w.range(l*16+15,l*16)=v2_f2b(acc[b*8+l]);
                }
                attn_out[(row*14+head)*8+b]=w;
            }
        }
        result_processed_rows=row+1;
    }
    if (flags&1U) result_output_crc=~v2_crc_words<3>(0xffffffffU,attn_out,valid_rows*112);
}
#ifndef __SYNTHESIS__
void attention_v2(const kv_axi_t*q,const kv_axi_t*k,const kv_axi_t*v,kv_axi_t*o,
 uint32_t m,uint32_t l,uint32_t p,uint32_t r,uint32_t n,float s,uint32_t c,uint32_t a,uint32_t j,uint32_t f,common_result_v2*z) {
    attention_v2_hls(q,k,v,o,m,l,p,r,n,s,c,a,j,f,V2_RESULT_ARGS(*z));
}
#endif
