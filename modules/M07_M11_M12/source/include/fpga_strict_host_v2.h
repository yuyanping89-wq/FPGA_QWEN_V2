#ifndef FPGA_STRICT_HOST_V2_H_
#define FPGA_STRICT_HOST_V2_H_

/* HLS has no allocation capacity/window descriptors. These host checks are
 * the mirror of PS M02/M03 preflight, not hardware pointer protection. */
#ifndef __SYNTHESIS__
#include "fpga_abi_v2.h"
struct fpga_host_range_v2 {const void* address;uint64_t bytes;};
static inline uint32_t fpga_strict_host_ranges_v2(const fpga_host_range_v2* inputs,
    unsigned ninputs,const fpga_host_range_v2* outputs,unsigned noutputs) {
    for(unsigned list=0;list<2;list++) {
        const fpga_host_range_v2* ranges=list?outputs:inputs;
        const unsigned count=list?noutputs:ninputs;
        for(unsigned i=0;i<count;i++) {
            const uint64_t address=(uint64_t)(uintptr_t)ranges[i].address;
            if(address==0||ranges[i].bytes==0||address>UINT64_MAX-ranges[i].bytes) return FPGA_STATUS_V2_BAD_ADDRESS_RANGE;
            if((address&15U)!=0) return FPGA_STATUS_V2_BAD_ALIGNMENT;
        }
    }
    for(unsigned o=0;o<noutputs;o++) {
        const uint64_t begin=(uint64_t)(uintptr_t)outputs[o].address,end=begin+outputs[o].bytes;
        for(unsigned i=0;i<ninputs;i++) {
            const uint64_t ibegin=(uint64_t)(uintptr_t)inputs[i].address;
            if(begin<ibegin+inputs[i].bytes&&ibegin<end) return FPGA_STATUS_V2_BAD_ALIAS;
        }
        for(unsigned j=0;j<o;j++) {
            const uint64_t jbegin=(uint64_t)(uintptr_t)outputs[j].address;
            if(begin<jbegin+outputs[j].bytes&&jbegin<end) return FPGA_STATUS_V2_BAD_ALIAS;
        }
    }
    return FPGA_STATUS_V2_OK;
}
#endif
#endif
