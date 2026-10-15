// Reduced-dimension RTL CoSim check for the burst-optimized M11 top.
// Random data, Decode rows=1 and Prefill rows=9 (crosses the 8-row block),
// compared bit-exactly with the scalar reference attention_out_v2().
#include "attention_output_kernel.hpp"
#include <cstring>
#include <iostream>
#include <vector>

static uint32_t seed = 7;
static data_t rnd(float s) {
    seed = seed * 1664525U + 1013904223U;
    return float_to_bf16(((int32_t)(seed >> 8) - (1 << 23)) / (float)(1 << 23) * s);
}

static int run(uint32_t rows, uint32_t mode) {
    const int GUARD = 8;
    std::vector<fpga_axi128_t> x((ATTN_OUT_X_DEPTH + 7) / 8), w((H * H + 7) / 8),
        y((ATTN_OUT_Y_DEPTH + 7) / 8);
    data_t *x16 = reinterpret_cast<data_t*>(x.data());
    data_t *w16 = reinterpret_cast<data_t*>(w.data());
    data_t *y16 = reinterpret_cast<data_t*>(y.data());
    for (uint32_t i = 0; i < rows * H; ++i) x16[i] = rnd(1.0f);
    for (int i = 0; i < H * H; ++i) w16[i] = rnd(0.25f);
    for (size_t i = 0; i < y.size() * 8; ++i) y16[i] = 0x7bff;
    std::vector<data_t> ref(rows * H);
    common_result_v2 rr;
    attention_out_v2(x16, w16, ref.data(), mode, rows, H, FPGA_LAYOUT_W_OG32_I_L32_V2,
                     FPGA_ABI_VERSION_V2, 1, 0, &rr);
    uint32_t status = 0, prow = 0, crc = 0, build = 0;
    uint64_t total = 0, compute = 0, reads = 0, writes = 0;
    attention_out_v2_hls(x.data(), w.data(), y.data(), mode, rows, H,
                         FPGA_LAYOUT_W_OG32_I_L32_V2, FPGA_ABI_VERSION_V2, 1, 0,
                         status, prow, total, compute, reads, writes, crc, build);
    if (status != FPGA_STATUS_V2_OK || prow != rows) return 1;
    if (memcmp(y16, ref.data(), rows * H * 2) != 0) return 2;
    for (int i = 0; i < GUARD; ++i) if (y16[rows * H + i] != 0x7bff) return 3;
    return 0;
}

int main() {
    int e = run(1, FPGA_RUN_DECODE);
    if (!e) { e = run(9, FPGA_RUN_PREFILL); if (e) e += 10; }
    if (e) { std::cout << "attention_out_v2 burst cosim FAIL " << e << "\n"; return e; }
    std::cout << "attention_out_v2 burst cosim PASS\n";
    return 0;
}
