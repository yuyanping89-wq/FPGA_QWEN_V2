#include "rope.h"
static void load_head(const kv_axi_t *p,float a[64]) {
#pragma HLS INLINE
    for (unsigned b=0;b<8;++b) {
#pragma HLS PIPELINE II=1
        kv_axi_t w=p[b];
        for (unsigned l=0;l<8;++l) {
#pragma HLS UNROLL
            a[b*8+l]=v2_b2f((uint16_t)(w>>(16*l)));
        }
    }
}
static void rotate_head(const kv_axi_t *src,kv_axi_t *dst,const float c[64],const float s[64]) {
#pragma HLS INLINE off
    float a[64],o[64];
#pragma HLS ARRAY_PARTITION variable=a cyclic factor=8
#pragma HLS ARRAY_PARTITION variable=o cyclic factor=8
    load_head(src,a);
    for (unsigned b=0;b<8;++b) {
#pragma HLS PIPELINE II=1
        for (unsigned l=0;l<4;++l) {
#pragma HLS UNROLL
            unsigned j=b*4+l,k=j+32;
            o[j]=a[j]*c[j]-a[k]*s[j];
            o[k]=a[k]*c[k]+a[j]*s[k];
        }
    }
    for (unsigned b=0;b<8;++b) {
#pragma HLS PIPELINE II=1
        kv_axi_t w=0;
        for (unsigned l=0;l<8;++l) {
#pragma HLS UNROLL
            w.range(l*16+15,l*16)=v2_f2b(o[b*8+l]);
        }
        dst[b]=w;
    }
}
void rope_v2_hls(const kv_axi_t *q_raw,const kv_axi_t *k_raw,
 const kv_axi_t *cos_table,const kv_axi_t *sin_table,kv_axi_t *q_rot,kv_axi_t *k_rot,
 uint32_t run_mode,uint32_t valid_rows,uint32_t position_base,uint32_t max_position,
 uint32_t abi_version,uint32_t job_id,uint32_t flags,V2_RESULT_DECL) {
#pragma HLS INTERFACE m_axi port=q_raw offset=slave bundle=gmem0 depth=7168 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=k_raw offset=slave bundle=gmem1 depth=1024 max_read_burst_length=64 num_read_outstanding=4
#pragma HLS INTERFACE m_axi port=cos_table offset=slave bundle=gmem2 depth=16384 max_read_burst_length=64 num_read_outstanding=2
#pragma HLS INTERFACE m_axi port=sin_table offset=slave bundle=gmem3 depth=16384 max_read_burst_length=64 num_read_outstanding=2
#pragma HLS INTERFACE m_axi port=q_rot offset=slave bundle=gmem4 depth=7168 max_read_burst_length=64 max_write_burst_length=64 num_read_outstanding=2 num_write_outstanding=4
#pragma HLS INTERFACE m_axi port=k_rot offset=slave bundle=gmem5 depth=1024 max_read_burst_length=64 max_write_burst_length=64 num_read_outstanding=2 num_write_outstanding=4
#pragma HLS INTERFACE s_axilite port=q_raw bundle=control
#pragma HLS INTERFACE s_axilite port=k_raw bundle=control
#pragma HLS INTERFACE s_axilite port=cos_table bundle=control
#pragma HLS INTERFACE s_axilite port=sin_table bundle=control
#pragma HLS INTERFACE s_axilite port=q_rot bundle=control
#pragma HLS INTERFACE s_axilite port=k_rot bundle=control
#pragma HLS INTERFACE s_axilite port=run_mode bundle=control
#pragma HLS INTERFACE s_axilite port=valid_rows bundle=control
#pragma HLS INTERFACE s_axilite port=position_base bundle=control
#pragma HLS INTERFACE s_axilite port=max_position bundle=control
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
#pragma HLS ALLOCATION instances=rotate_head limit=1 function
    (void)job_id;
    result_status_code=v2_check_job(run_mode,valid_rows,abi_version,flags);
    result_processed_rows=0; result_perf_cycles_total=0; result_perf_cycles_compute=0;
    result_perf_read_beats=0; result_perf_write_beats=0; result_output_crc=0;
    result_build_config=0x30010011U;
    if (result_status_code) return;
    if (max_position==0 || max_position>2048 || position_base>=max_position || valid_rows>max_position-position_base) {
        result_status_code=PSPL_V2_BAD_TOKEN; return;
    }
#ifndef __SYNTHESIS__
    if (flags&8U) {
        const void *p[]={q_raw,k_raw,cos_table,sin_table,q_rot,k_rot};
        const uint64_t n[]={valid_rows*1792ULL,valid_rows*256ULL,max_position*128ULL,max_position*128ULL,valid_rows*1792ULL,valid_rows*256ULL};
        for (unsigned i=0;i<6;++i) if (!v2_aligned(p[i])) {result_status_code=PSPL_V2_BAD_ALIGNMENT;return;}
        for (unsigned i=4;i<6;++i) for (unsigned j=0;j<i;++j)
            if (v2_overlap(p[i],n[i],p[j],n[j])) {result_status_code=PSPL_V2_BAD_ALIAS;return;}
    }
#endif
    float c[64],s[64];
#pragma HLS ARRAY_PARTITION variable=c cyclic factor=8
#pragma HLS ARRAY_PARTITION variable=s cyclic factor=8
    for (unsigned row=0;row<valid_rows;++row) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=64
        load_head(cos_table+(position_base+row)*8,c);
        load_head(sin_table+(position_base+row)*8,s);
        for (unsigned head=0;head<14;++head)
            rotate_head(q_raw+(head*valid_rows+row)*8,q_rot+(head*valid_rows+row)*8,c,s);
        for (unsigned head=0;head<2;++head)
            rotate_head(k_raw+(head*valid_rows+row)*8,k_rot+(head*valid_rows+row)*8,c,s);
        result_processed_rows=row+1;
    }
    if (flags&1U) {
        uint32_t state=v2_crc_words<4>(0xffffffffU,q_rot,valid_rows*112);
        result_output_crc=~v2_crc_words<5>(state,k_rot,valid_rows*16);
    }
}
#ifndef __SYNTHESIS__
void rope_v2(const kv_axi_t*q,const kv_axi_t*k,const kv_axi_t*c,const kv_axi_t*s,kv_axi_t*qo,kv_axi_t*ko,
 uint32_t m,uint32_t r,uint32_t p,uint32_t n,uint32_t a,uint32_t j,uint32_t f,common_result_v2*z) {
    rope_v2_hls(q,k,c,s,qo,ko,m,r,p,n,a,j,f,V2_RESULT_ARGS(*z));
}
#endif
