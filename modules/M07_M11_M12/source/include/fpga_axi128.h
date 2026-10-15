#ifndef FPGA_AXI128_H_
#define FPGA_AXI128_H_

#include <stdint.h>

#if defined(__SYNTHESIS__) || defined(FPGA_USE_AP_INT)
#include <ap_int.h>
typedef ap_uint<128> fpga_axi128_t;

static inline uint16_t fpga_axi128_get_bf16(const fpga_axi128_t &beat,
                                             uint32_t lane) {
    return (uint16_t)beat.range(lane * 16U + 15U, lane * 16U);
}

static inline void fpga_axi128_set_bf16(fpga_axi128_t &beat, uint32_t lane,
                                        uint16_t value) {
    beat.range(lane * 16U + 15U, lane * 16U) = value;
}
#else
typedef struct {
    uint64_t lo;
    uint64_t hi;
} fpga_axi128_t;

static inline uint16_t fpga_axi128_get_bf16(const fpga_axi128_t &beat,
                                             uint32_t lane) {
    const uint64_t word = lane < 4U ? beat.lo : beat.hi;
    return (uint16_t)(word >> ((lane & 3U) * 16U));
}

static inline void fpga_axi128_set_bf16(fpga_axi128_t &beat, uint32_t lane,
                                        uint16_t value) {
    uint64_t *word = lane < 4U ? &beat.lo : &beat.hi;
    const uint32_t shift = (lane & 3U) * 16U;
    *word = (*word & ~((uint64_t)0xffffU << shift)) |
            ((uint64_t)value << shift);
}
#endif

#define FPGA_AXI128_BF16_LANES 8U

#endif
