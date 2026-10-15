#include "attention_output_kernel.hpp"
#include <iostream>
#include <vector>

int main() {
    const int T = 2;
    std::vector<fpga_axi128_t> x((ATTN_OUT_X_DEPTH + 7) / 8),
        w((H * H + 7) / 8), y((ATTN_OUT_Y_DEPTH + 7) / 8);
    data_t *x16 = reinterpret_cast<data_t*>(x.data());
    data_t *w16 = reinterpret_cast<data_t*>(w.data());
    data_t *y16 = reinterpret_cast<data_t*>(y.data());
    uint32_t status = 0, rows = 0, crc = 0, build = 0;
    uint64_t total = 0, compute = 0, reads = 0, writes = 0;
#ifndef ATTN_OUT_COSIM_ZERO
    for (int i = 0; i < T * H; ++i) x16[i] = float_to_bf16((float)((i % 19) - 9) * 0.03125f);
    for (int o = 0; o < H; ++o) for (int i = 0; i < H; ++i)
        w16[(o / 32 * H + i) * 32 + o % 32] = o == i ? float_to_bf16(1.0f) : 0;
#endif
    attention_out_v2_hls(x.data(), w.data(), y.data(), FPGA_RUN_PREFILL, T, H,
                         FPGA_LAYOUT_W_OG32_I_L32_V2, FPGA_ABI_VERSION_V2, 1, 0,
                         status, rows, total, compute, reads, writes, crc, build);
    if (status != FPGA_STATUS_V2_OK) return 1;
#ifndef ATTN_OUT_COSIM_ZERO
    for (int i = 0; i < T * H; ++i) if (y16[i] != x16[i]) return 2;
#else
    for (int i = 0; i < T * H; ++i) if (y16[i] != 0) return 2;
#endif
    std::cout << "attention_output_v2_tb PASS\n";
    return 0;
}
