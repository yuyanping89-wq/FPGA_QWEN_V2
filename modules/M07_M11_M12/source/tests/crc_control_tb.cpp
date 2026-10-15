#include <stdint.h>
#include <stdio.h>
#include <vector>
#include "../include/fpga_abi_v2.h"
#include "../include/fpga_axi128.h"
#include "../include/fpga_og32_linear_v2.h"

#if !defined(TEST_M07) && !defined(TEST_M11) && !defined(TEST_M12)
#define TEST_M07
#define TEST_M11
#define TEST_M12
#endif

#define RESULT_OUT uint32_t&,uint32_t&,uint64_t&,uint64_t&,uint64_t&,uint64_t&,uint32_t&,uint32_t&
void qkv_projection_v2_hls(const fpga_axi128_t*,const fpga_axi128_t*,const fpga_axi128_t*,
    fpga_axi128_t*,fpga_axi128_t*,fpga_axi128_t*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,RESULT_OUT);
void attention_out_v2_hls(const fpga_axi128_t*,const fpga_axi128_t*,fpga_axi128_t*,
    uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,RESULT_OUT);
void residual_add_v2_hls(const fpga_axi128_t*,const fpga_axi128_t*,fpga_axi128_t*,
    uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,RESULT_OUT);
struct Result {uint32_t status,rows,crc,cfg;uint64_t total,compute,reads,writes;};
#define RES(r) r.status,r.rows,r.total,r.compute,r.reads,r.writes,r.crc,r.cfg
struct Buf {
    std::vector<fpga_axi128_t> data;
    Buf(uint32_t n) : data((n+7)/8) {
        for (size_t b=0;b<data.size();b++) for(unsigned lane=0;lane<8;lane++) fpga_axi128_set_bf16(data[b],lane,0);
    }
    void set(uint32_t i,uint16_t value) {fpga_axi128_set_bf16(data[i/8],i%8,value);}
    uint16_t get(uint32_t i) const {return fpga_axi128_get_bf16(data[i/8],i%8);}
    fpga_axi128_t* p() {return data.data();}
};
static int failures=0;
static void check(bool ok,const char* what,unsigned rows) {
    if(!ok) {printf("FAIL %s rows=%u\n",what,rows);++failures;}
}
/* Independent bit-at-a-time checksum over golden BF16 values. */
static uint32_t golden_crc_update(uint32_t state,const std::vector<uint16_t>& values) {
    for(uint16_t value:values) for(unsigned byte=0;byte<2;byte++) {
        unsigned char input=(unsigned char)(value>>(byte*8));
        for(unsigned bit=0;bit<8;bit++) {
            const bool feedback=((state&1U)^((input>>bit)&1U))!=0;
            state>>=1;
            if(feedback) state^=0xedb88320U;
        }
    }
    return state;
}
static void result_check(const Result& r,unsigned rows,uint32_t expected_crc,uint32_t config,unsigned flags) {
    check(r.status==0&&r.rows==rows,"successful result",rows);
    check(r.crc==((flags&1U)?expected_crc:0U),"CRC enabled/disabled",rows);
    check(r.cfg==config,"build revision4",rows);
    check(r.total==0&&r.compute==0&&r.reads==0&&r.writes==0,"HLS counters unavailable as zero",rows);
}
static const unsigned H=896,QKV=1152;

#ifdef TEST_M07
static void test_m07(unsigned rows) {
    Buf x(rows*H),w(QKV*H),bias(QKV),q(rows*H+8),k(rows*128+8),v(rows*128+8);
    for(unsigned i=0;i<rows*H;i++) x.set(i,(uint16_t)(0x3f00+(i%113)));
    for(unsigned o=0;o<QKV;o++) w.set((o/32*H+o%H)*32+o%32,0x3f80);
    for(unsigned o=0;o<QKV;o++) bias.set(o,og32_f2b(((int)(o%9)-4)/32.0f));
    std::vector<uint16_t> eq(rows*H),ek(rows*128),ev(rows*128);
    for(unsigned h=0;h<14;h++) for(unsigned row=0;row<rows;row++) for(unsigned d=0;d<64;d++) {
        unsigned o=h*64+d;
        eq[(h*rows+row)*64+d]=og32_f2b(og32_b2f(x.get(row*H+o))+og32_b2f(bias.get(o)));
    }
    for(unsigned h=0;h<2;h++) for(unsigned row=0;row<rows;row++) for(unsigned d=0;d<64;d++) {
        unsigned ok=896+h*64+d,ov=1024+h*64+d;
        ek[(h*rows+row)*64+d]=og32_f2b(og32_b2f(x.get(row*H+ok%H))+og32_b2f(bias.get(ok)));
        ev[(h*rows+row)*64+d]=og32_f2b(og32_b2f(x.get(row*H+ov%H))+og32_b2f(bias.get(ov)));
    }
    uint32_t state=golden_crc_update(0xffffffffU,eq);
    state=golden_crc_update(state,ek);state=golden_crc_update(state,ev);
    Result r;
    for(unsigned flags:{0u,1u,3u,9u}) {
        q.set(rows*H,0x7bff);k.set(rows*128,0x7bff);v.set(rows*128,0x7bff);
        qkv_projection_v2_hls(x.p(),w.p(),bias.p(),q.p(),k.p(),v.p(),rows==1?1:0,rows,H,6,2,17,flags,RES(r));
        result_check(r,rows,~state,0x40008041,flags);
        bool equal=true;
        for(unsigned i=0;i<eq.size();i++) equal&=q.get(i)==eq[i];
        for(unsigned i=0;i<ek.size();i++) equal&=k.get(i)==ek[i]&&v.get(i)==ev[i];
        check(equal,"M07 head-major data",rows);
        check(q.get(rows*H)==0x7bff&&k.get(rows*128)==0x7bff&&v.get(rows*128)==0x7bff,"M07 output guard",rows);
    }
    for(unsigned flags:{4u,16u,0x80000000u}) {
        q.set(0,0x7bff);
        qkv_projection_v2_hls(x.p(),w.p(),bias.p(),q.p(),k.p(),v.p(),0,rows,H,6,2,17,flags,RES(r));
        check(r.status==11&&r.rows==0&&r.crc==0&&q.get(0)==0x7bff,"M07 unsupported flags/untouched output",rows);
    }
}
#endif
#ifdef TEST_M11
static void test_m11(unsigned rows) {
    Buf x(rows*H),w(H*H),y(rows*H+8);
    std::vector<uint16_t> expected(rows*H);
    for(unsigned i=0;i<rows*H;i++) {expected[i]=(uint16_t)(0x3f00+(i%113));x.set(i,expected[i]);}
    for(unsigned o=0;o<H;o++) w.set((o/32*H+o)*32+o%32,0x3f80);
    const uint32_t crc=~golden_crc_update(0xffffffffU,expected);
    Result r;
    for(unsigned flags:{0u,1u,3u,9u}) {
        y.set(rows*H,0x7bff);
        attention_out_v2_hls(x.p(),w.p(),y.p(),rows==1?1:0,rows,H,6,2,18,flags,RES(r));
        result_check(r,rows,crc,0x40008041,flags);
        bool equal=true;for(unsigned i=0;i<expected.size();i++) equal&=y.get(i)==expected[i];
        check(equal&&y.get(rows*H)==0x7bff,"M11 data/guard",rows);
    }
    for(unsigned flags:{4u,16u,0x80000000u}) {
        y.set(0,0x7bff);
        attention_out_v2_hls(x.p(),w.p(),y.p(),0,rows,H,6,2,18,flags,RES(r));
        check(r.status==11&&r.rows==0&&r.crc==0&&y.get(0)==0x7bff,"M11 unsupported flags/untouched output",rows);
    }
}
#endif
#ifdef TEST_M12
static void test_m12(unsigned rows) {
    Buf a(rows*H),b(rows*H),y(rows*H+8);
    std::vector<uint16_t> expected(rows*H,0x4000);
    for(unsigned i=0;i<rows*H;i++) {a.set(i,0x3f80);b.set(i,0x3f80);}
    const uint32_t crc=~golden_crc_update(0xffffffffU,expected);
    if(rows==1) check(crc==0x51849874U,"M12 independent Python zlib fixture",rows);
    Result r;
    for(unsigned flags:{0u,1u,3u,9u}) {
        y.set(rows*H,0x7bff);
        residual_add_v2_hls(a.p(),b.p(),y.p(),rows==1?1:0,rows,H,rows%2,2,19,flags,RES(r));
        result_check(r,rows,crc,0x30010001,flags);
        bool equal=true;for(unsigned i=0;i<expected.size();i++) equal&=y.get(i)==expected[i];
        check(equal&&y.get(rows*H)==0x7bff,"M12 data/guard",rows);
    }
    for(unsigned flags:{4u,16u,0x80000000u}) {
        y.set(0,0x7bff);
        residual_add_v2_hls(a.p(),b.p(),y.p(),0,rows,H,0,2,19,flags,RES(r));
        check(r.status==11&&r.rows==0&&r.crc==0&&y.get(0)==0x7bff,"M12 unsupported flags/untouched output",rows);
    }
}
#endif
int main() {
    for(unsigned rows:{1u,9u,64u}) {
#ifdef TEST_M07
        test_m07(rows);
#endif
#ifdef TEST_M11
        test_m11(rows);
#endif
#ifdef TEST_M12
        test_m12(rows);
#endif
    }
    printf(failures?"V2_CRC_CONTROL_FAIL %d\n":"V2_CRC_CONTROL_PASS\n",failures);
    return failures?1:0;
}
