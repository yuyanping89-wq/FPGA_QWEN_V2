#include "qkv_kernel.hpp"
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

int main() {
    const int T = 2;
    const int total_out = Q_OUT + K_OUT + V_OUT;
    std::vector<fpga_axi128_t> x((QKV_X_DEPTH + 7) / 8),
        w((total_out * H + 7) / 8), bias((total_out + 7) / 8),
        q((QKV_Q_DEPTH + 7) / 8), k((QKV_K_DEPTH + 7) / 8),
        v((QKV_V_DEPTH + 7) / 8);
    data_t *x16 = reinterpret_cast<data_t*>(x.data());
    data_t *w16 = reinterpret_cast<data_t*>(w.data());
    data_t *bias16 = reinterpret_cast<data_t*>(bias.data());
    data_t *q16 = reinterpret_cast<data_t*>(q.data());
    uint32_t status = 0, rows = 0, crc = 0, build = 0;
    uint64_t total = 0, compute = 0, reads = 0, writes = 0;
    for (int i = 0; i < T * H; ++i) x16[i] = float_to_bf16((float)((i % 17) - 8) * 0.03125f);
    for (int o = 0; o < total_out; ++o) {
        bias16[o] = 0;
        for (int h = 0; h < H; ++h) {
            const int p = (o / 32 * H + h) * 32 + o % 32;
            w16[p] = (o < H && h == o) || (o >= H && h == (o - H) % H) ? float_to_bf16(1.0f) : 0;
        }
    }
    qkv_projection_v2_hls(x.data(), w.data(), bias.data(), q.data(), k.data(), v.data(),
                          FPGA_RUN_PREFILL, T, H, FPGA_LAYOUT_W_OG32_I_L32_V2,
                          FPGA_ABI_VERSION_V2, 1, 0, status, rows, total, compute,
                          reads, writes, crc, build);
    if (status != FPGA_STATUS_V2_OK || rows != T) return 1;
    for (int head = 0; head < A; ++head) for (int t = 0; t < T; ++t)
        for (int d = 0; d < D; ++d)
            if (q16[(head * T + t) * D + d] != x16[t * H + head * D + d]) return 2;
    qkv_projection_v2_hls(x.data(), w.data(), bias.data(), q.data(), k.data(), v.data(),
                      FPGA_RUN_DECODE, 2, H, FPGA_LAYOUT_W_OG32_I_L32_V2,
                      FPGA_ABI_VERSION_V2, 2, 0, status, rows, total, compute,
                      reads, writes, crc, build);
    if (status != FPGA_STATUS_V2_BAD_DIMENSION) return 3;
    std::cout << "qkv_v2_tb PASS\n";
    return 0;
}
