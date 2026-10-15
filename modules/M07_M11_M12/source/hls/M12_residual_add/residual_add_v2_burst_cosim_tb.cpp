// Reduced-dimension RTL CoSim check for the burst-optimized M12 top.
// Random data, Decode rows=1 and Prefill rows=9, compared bit-exactly with
// the scalar reference residual_add_v2().
#include "residual_add_kernel.hpp"
#include <cstring>
#include <iostream>
#include <vector>

static uint32_t seed = 3;
static data_t rnd(float s) {
    seed = seed * 1664525U + 1013904223U;
    return float_to_bf16(((int32_t)(seed >> 8) - (1 << 23)) / (float)(1 << 23) * s);
}

static int run(uint32_t rows, uint32_t mode, uint32_t role) {
    const int GUARD = 8;
    std::vector<fpga_axi128_t> a((DEPTH + 7) / 8), b((DEPTH + 7) / 8), y((DEPTH + 7) / 8);
    data_t *a16 = reinterpret_cast<data_t*>(a.data());
    data_t *b16 = reinterpret_cast<data_t*>(b.data());
    data_t *y16 = reinterpret_cast<data_t*>(y.data());
    for (uint32_t i = 0; i < rows * H; ++i) { a16[i] = rnd(4.0f); b16[i] = rnd(4.0f); }
    for (size_t i = 0; i < y.size() * 8; ++i) y16[i] = 0x7bff;
    std::vector<data_t> ref(rows * H);
    common_result_v2 rr;
    residual_add_v2(a16, b16, ref.data(), mode, rows, H, role, FPGA_ABI_VERSION_V2, 1, 0, &rr);
    uint32_t status = 0, prow = 0, crc = 0, build = 0;
    uint64_t total = 0, compute = 0, reads = 0, writes = 0;
    residual_add_v2_hls(a.data(), b.data(), y.data(), mode, rows, H, role,
                        FPGA_ABI_VERSION_V2, 1, 0, status, prow, total, compute,
                        reads, writes, crc, build);
    if (status != FPGA_STATUS_V2_OK || prow != rows) return 1;
    if (memcmp(y16, ref.data(), rows * H * 2) != 0) return 2;
    for (int i = 0; i < GUARD; ++i) if (y16[rows * H + i] != 0x7bff) return 3;
    return 0;
}

int main() {
    int e = run(1, FPGA_RUN_DECODE, 0);
    if (!e) { e = run(9, FPGA_RUN_PREFILL, 1); if (e) e += 10; }
    if (e) { std::cout << "residual_add_v2 burst cosim FAIL " << e << "\n"; return e; }
    std::cout << "residual_add_v2 burst cosim PASS\n";
    return 0;
}
