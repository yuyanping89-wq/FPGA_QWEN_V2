#include "rope.h"
#include "attn.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <vector>
#include <malloc.h>
struct Buffer {
    kv_axi_t *p; unsigned size;
    explicit Buffer(unsigned beats):size(beats) {
        p=(kv_axi_t*)_aligned_malloc(beats*sizeof(kv_axi_t),64);
        if(!p) throw std::bad_alloc();
        for(unsigned i=0;i<beats;++i) new(p+i) kv_axi_t(0);
    }
    ~Buffer(){for(unsigned i=0;i<size;++i)p[i].~kv_axi_t();_aligned_free(p);}
    void set(unsigned i,float f){p[i/8].range(i%8*16+15,i%8*16)=v2_f2b(f);}
    float get(unsigned i)const{return v2_b2f((uint16_t)(p[i/8]>>(i%8*16)));}
};
static int failures=0;
static void check(bool b,const char*s){if(!b){++failures;printf("FAIL %s\n",s);}}
// Independent, exhaustive BF16 boundary policy checks, including signed NaNs.
static void numeric_boundary_case() {
    for(unsigned b=0;b<65536;++b) {
        bits32 value; value.f=v2_b2f((uint16_t)b);
        bool nan=(b&0x7f80U)==0x7f80U && (b&0x7fU);
        unsigned expected=nan?((b&0x8000U)<<16)|0x7fc00000U:b<<16;
        check(value.u==expected,"canonical BF16 to FP32");
        check(v2_f2b(value.f)==(nan?((b&0x8000U)|0x7fc0U):b),"BF16 roundtrip");
    }
    const unsigned in[]={0x3f808000U,0x3f818000U,0x7fffffffU,0xffffffffU,
                         0x7f800000U,0xff800000U,0x80000000U,0x00008000U,
                         0x00018000U,0x7f7fffffU};
    const unsigned out[]={0x3f80U,0x3f82U,0x7fc0U,0xffc0U,0x7f80U,
                          0xff80U,0x8000U,0x0000U,0x0002U,0x7f80U};
    for(unsigned i=0;i<sizeof(in)/sizeof(in[0]);++i) {
        bits32 value;value.u=in[i];check(v2_f2b(value.f)==out[i],"FP32 RNE/special values");
    }
}
static uint32_t crc(uint32_t c,const Buffer& b,unsigned beats) {
    for(unsigned i=0;i<beats*16;++i){unsigned x=(unsigned)(b.p[i/16]>>(8*(i%16)))&255U;
        for(unsigned bit=0;bit<8;++bit){unsigned feedback=(c^(x>>bit))&1;c>>=1;if(feedback)c^=0xedb88320U;}}
    return c;
}
#ifdef TEST_M08
static void rope_case(unsigned rows,unsigned position,unsigned flags) {
    Buffer q(rows*112),k(rows*16),c(2048*8),s(2048*8),qo(rows*112+4),ko(rows*16+4);
    for(unsigned i=0;i<rows*896;++i)q.set(i,((int)(i%41)-20)/16.0f);
    for(unsigned i=0;i<rows*128;++i)k.set(i,((int)(i%23)-11)/16.0f);
    for(unsigned pos=0;pos<2048;++pos)for(unsigned d=0;d<64;++d){c.set(pos*64+d,std::cos((pos+1)*0.013f+(d%32)*0.009f));s.set(pos*64+d,std::sin((pos+1)*0.013f+(d%32)*0.009f));}
    qo.set(rows*896,42); ko.set(rows*128,42);
    common_result_v2 r;
    rope_v2_hls(q.p,k.p,c.p,s.p,qo.p,ko.p,rows==1?1:0,rows,position,2048,2,7,flags,V2_RESULT_ARGS(r));
    check(r.status_code==0 && r.processed_rows==rows,"M08 result rows");
    for(unsigned head=0;head<16;++head)for(unsigned row=0;row<rows;++row)for(unsigned d=0;d<32;++d){
        const Buffer &in=head<14?q:k; const Buffer &out=head<14?qo:ko;
        unsigned h=head<14?head:head-14,i=(h*rows+row)*64+d,t=(position+row)*64+d;
        float a=in.get(i),b=in.get(i+32);
        uint16_t lo=v2_f2b(a*c.get(t)-b*s.get(t)),hi=v2_f2b(b*c.get(t+32)+a*s.get(t+32));
        check(v2_f2b(out.get(i))==lo && v2_f2b(out.get(i+32))==hi,"M08 absolute position/rotation");
    }
    check(qo.get(rows*896)==42 && ko.get(rows*128)==42,"M08 output boundary");
    check(r.output_crc==((flags&1)?~crc(crc(0xffffffffU,qo,rows*112),ko,rows*16):0),"M08 independent CRC");
    check(r.build_config==0x30010011U && r.perf_cycles_total==0 && r.perf_read_beats==0,"M08 identity/Csim unavailable counters");
    for(unsigned bad:{4U,16U,0x80000000U}) {qo.set(0,42);rope_v2_hls(q.p,k.p,c.p,s.p,qo.p,ko.p,0,rows,position,2048,2,7,bad,V2_RESULT_ARGS(r));check(r.status_code==11&&r.processed_rows==0&&qo.get(0)==42,"M08 bad flags before writes");}
    rope_v2_hls(q.p,k.p,c.p,s.p,qo.p,ko.p,0,rows,2048,2048,2,7,0,V2_RESULT_ARGS(r));check(r.status_code==7,"M08 bad position");
    rope_v2_hls(q.p,k.p,c.p,s.p,qo.p,ko.p,0,rows,position,2048,1,7,0,V2_RESULT_ARGS(r));check(r.status_code==1,"M08 bad ABI");
    rope_v2_hls(q.p,k.p,c.p,s.p,qo.p,ko.p,1,2,position,2048,2,7,0,V2_RESULT_ARGS(r));check(r.status_code==2,"M08 decode rows");
    rope_v2_hls(q.p,k.p,c.p,s.p,q.p,ko.p,0,rows,position,2048,2,7,8,V2_RESULT_ARGS(r));check(r.status_code==8,"M08 alias");
}
#endif
#ifdef TEST_M10
static void attention_case(unsigned rows,unsigned position,unsigned length,unsigned layer,unsigned causal,unsigned flags) {
    Buffer q(rows*112),k(24*2*2048*8),v(24*2*2048*8),o(rows*112+4);
    for(unsigned i=0;i<rows*896;++i)q.set(i,((int)(i%31)-15)/32.0f);
    for(unsigned head=0;head<2;++head)for(unsigned key=0;key<length;++key)for(unsigned d=0;d<64;++d){
        unsigned i=((layer*2+head)*2048+key)*64+d;
        k.set(i,((int)((head*17+key*5+d)%37)-18)/32.0f);
        v.set(i,((int)((head*71+key*3+d)%101)-50)/16.0f);
    }
    o.set(rows*896,42);common_result_v2 r;
    attention_v2_hls(q.p,k.p,v.p,o.p,rows==1?1:0,layer,position,rows,length,0.125f,causal,2,8,flags,V2_RESULT_ARGS(r));
    check(r.status_code==0 && r.processed_rows==rows,"M10 result rows");
    double max_error=0;
    for(unsigned row=0;row<rows;++row)for(unsigned head=0;head<14;++head){
        unsigned n=causal?position+row+1:length;std::vector<double>w(n);double maximum=-1e300,sum=0;
        for(unsigned key=0;key<n;++key){double score=0;for(unsigned d=0;d<64;++d)score+=(double)q.get((head*rows+row)*64+d)*k.get(((layer*2+head/7)*2048+key)*64+d);w[key]=score/8;if(w[key]>maximum)maximum=w[key];}
        for(unsigned key=0;key<n;++key){w[key]=std::exp(w[key]-maximum);sum+=w[key];}
        for(unsigned d=0;d<64;++d){double expected=0;for(unsigned key=0;key<n;++key)expected+=w[key]/sum*v.get(((layer*2+head/7)*2048+key)*64+d);
            double e=std::fabs(o.get((row*14+head)*64+d)-expected);if(e>max_error)max_error=e;
            check(e<=0.006+0.006*std::fabs(expected),"M10 golden softmax/GQA/history");}
    }
    printf("M10 rows=%u base=%u len=%u layer=%u causal=%u max_abs_error=%.6g\n",rows,position,length,layer,causal,max_error);
    check(o.get(rows*896)==42,"M10 output boundary");
    check(r.output_crc==((flags&1)?~crc(0xffffffffU,o,rows*112):0),"M10 independent CRC");
    check(r.build_config==0x30008011U && r.perf_cycles_total==0 && r.perf_read_beats==0,"M10 identity/Csim unavailable counters");
    for(unsigned bad:{4U,16U,0x80000000U}){o.set(0,42);attention_v2_hls(q.p,k.p,v.p,o.p,0,layer,position,rows,length,0.125f,causal,2,8,bad,V2_RESULT_ARGS(r));check(r.status_code==11&&r.processed_rows==0&&o.get(0)==42,"M10 bad flags before writes");}
    attention_v2_hls(q.p,k.p,v.p,o.p,0,24,position,rows,length,0.125f,causal,2,8,0,V2_RESULT_ARGS(r));check(r.status_code==6,"M10 layer bound");
    attention_v2_hls(q.p,k.p,v.p,o.p,0,layer,position,rows,0,0.125f,causal,2,8,0,V2_RESULT_ARGS(r));check(r.status_code==2,"M10 cache bound");
    attention_v2_hls(q.p,k.p,v.p,o.p,0,layer,length,rows,length,0.125f,causal,2,8,0,V2_RESULT_ARGS(r));check(r.status_code==7,"M10 query position");
    attention_v2_hls(q.p,k.p,v.p,o.p,0,layer,position,rows,length,0.0f,causal,2,8,0,V2_RESULT_ARGS(r));check(r.status_code==9,"M10 scale");
    attention_v2_hls(q.p,k.p,v.p,q.p,0,layer,position,rows,length,0.125f,causal,2,8,8,V2_RESULT_ARGS(r));check(r.status_code==8,"M10 alias");
}
#endif
int main(){
    numeric_boundary_case();
#ifdef TEST_M08
    rope_case(1,2047,0);rope_case(9,17,3);rope_case(64,101,11);
#endif
#ifdef TEST_M10
    attention_case(1,0,1,0,1,0);
    attention_case(1,2047,2048,23,1,3);
    attention_case(9,17,26,7,1,11);
    attention_case(3,4,17,8,0,1);
    attention_case(64,0,64,1,1,2);
#endif
    printf("M08_M10_V2_INTERFACE_%s failures=%d\n",failures?"FAIL":"PASS",failures);return failures?1:0;
}
