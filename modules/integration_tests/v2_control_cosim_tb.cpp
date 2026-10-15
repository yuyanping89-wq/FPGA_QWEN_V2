#include "rope.h"
#include "attn.h"
#include <cstdio>

// Full declared capacities let HLS's AXI memory models marshal safely even
// though these early-reject transactions must never access the data ports.
static kv_axi_t q[7168], output[7168];
#ifdef TEST_M08
static kv_axi_t k[1024], cosine[16384], sine[16384], k_output[1024];
#else
static kv_axi_t k[786432], v[786432];
#endif

int main() {
    const unsigned flags[] = {4U, 6U, 0U, 0U};
    const unsigned abi[] = {2U, 2U, 1U, 2U};
    const unsigned mode[] = {1U, 1U, 1U, 2U};
    const unsigned expected[] = {11U, 11U, 1U, 2U};
    unsigned failed = 0;
    for (unsigned i=0;i<4;++i) {
        common_result_v2 r;
#ifdef TEST_M08
        rope_v2_hls(q,k,cosine,sine,output,k_output,mode[i],1,0,2048,
                    abi[i],100+i,flags[i],V2_RESULT_ARGS(r));
        const unsigned identity=0x30010011U;
#else
        attention_v2_hls(q,k,v,output,mode[i],0,0,1,1,0.125f,1,
                         abi[i],100+i,flags[i],V2_RESULT_ARGS(r));
        const unsigned identity=0x30008011U;
#endif
        if (r.status_code!=expected[i] || r.processed_rows || r.output_crc ||
            r.perf_cycles_total || r.perf_cycles_compute || r.perf_read_beats ||
            r.perf_write_beats || r.build_config!=identity) ++failed;
    }
    std::printf("V2_CONTROL_COSIM_TB_%s failures=%u\n",failed?"FAIL":"PASS",failed);
    return failed ? 1 : 0;
}
