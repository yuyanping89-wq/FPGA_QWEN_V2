#include "residual_add_kernel.hpp"
#include <iostream>
#include <vector>

int main() {
    const int T = 2;
    std::vector<fpga_axi128_t> a((DEPTH + 7) / 8), b((DEPTH + 7) / 8),
        y((DEPTH + 7) / 8);
    data_t *a16 = reinterpret_cast<data_t*>(a.data());
    data_t *b16 = reinterpret_cast<data_t*>(b.data());
    data_t *y16 = reinterpret_cast<data_t*>(y.data());
    uint32_t status = 0, rows = 0, crc = 0, build = 0;
    uint64_t total = 0, compute = 0, reads = 0, writes = 0;
    for (int i = 0; i < T * H; ++i) {
        a16[i] = float_to_bf16((float)((i % 17) - 8) * 0.03125f);
        b16[i] = float_to_bf16((float)((i % 11) - 5) * 0.015625f);
    }
    residual_add_v2_hls(a.data(), b.data(), y.data(), FPGA_RUN_PREFILL, T, H, 0,
                        FPGA_ABI_VERSION_V2, 1, 0, status, rows, total, compute,
                        reads, writes, crc, build);
    if (status != FPGA_STATUS_V2_OK) return 1;
    for (int i = 0; i < T * H; ++i)
        if (y16[i] != float_to_bf16(bf16_to_float(a16[i]) + bf16_to_float(b16[i]))) return 2;
    std::cout << "residual_add_v2_tb PASS\n";
    return 0;
}
