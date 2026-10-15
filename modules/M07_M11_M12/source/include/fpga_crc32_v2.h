#ifndef FPGA_CRC32_V2_H_
#define FPGA_CRC32_V2_H_

#include "fpga_axi128.h"

/* Reflected IEEE CRC-32: initial state ffffffff, polynomial edb88320,
 * final XOR ffffffff. Pass the unfinalized state between Q, K and V. */
static inline uint32_t fpga_crc32_byte_v2(uint32_t crc, uint8_t byte) {
    crc ^= byte;
    for (unsigned bit = 0; bit < 8; ++bit) {
#pragma HLS UNROLL
        const uint32_t mask = 0U - (crc & 1U);
        crc = (crc >> 1) ^ (0xedb88320U & mask);
    }
    return crc;
}

/* Keep sixteen byte updates out of the main datapath's timing path. */
static uint32_t fpga_crc32_beat_v2(uint32_t crc, const fpga_axi128_t& beat) {
#pragma HLS INLINE off
crc_bytes:
    for (unsigned byte = 0; byte < 16; ++byte) {
#pragma HLS PIPELINE II=1
        const uint16_t lane = fpga_axi128_get_bf16(beat, byte / 2U);
        crc = fpga_crc32_byte_v2(crc, (uint8_t)(lane >> ((byte & 1U) * 8U)));
    }
    return crc;
}

/* Outputs are reread in their final, contiguous memory order. This is
 * necessary for M07, whose calculation writes eight-row blocks per head.
 * The extra AXI reads occur only with ENABLE_OUTPUT_CRC. */
static uint32_t fpga_crc32_axi128_range_v2(uint32_t crc,
                                         const fpga_axi128_t* values,
                                         uint32_t beats) {
#pragma HLS INLINE
crc_readback:
    for (uint32_t beat = 0; beat < beats; ++beat) {
#pragma HLS LOOP_TRIPCOUNT min=16 max=7168
        crc = fpga_crc32_beat_v2(crc, values[beat]);
    }
    return crc;
}

static inline uint32_t fpga_crc32_bf16_range_v2(uint32_t crc,
                                               const uint16_t* values,
                                               uint32_t elements) {
    for (uint32_t i = 0; i < elements; ++i) {
        crc = fpga_crc32_byte_v2(crc, (uint8_t)values[i]);
        crc = fpga_crc32_byte_v2(crc, (uint8_t)(values[i] >> 8));
    }
    return crc;
}
#endif
