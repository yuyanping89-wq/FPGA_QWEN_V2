// Reduced-dimension RTL CoSim check for the burst-optimized M07 top.
// Random data, Decode rows=1 and Prefill rows=9 (crosses the 8-row block),
// compared bit-exactly with the scalar reference qkv_projection_v2().
#include "qkv_kernel.hpp"
#include <cstring>
#include <iostream>
#include <vector>

static uint32_t seed = 11;
static data_t rnd(float s) {
    seed = seed * 1664525U + 1013904223U;
    return float_to_bf16(((int32_t)(seed >> 8) - (1 << 23)) / (float)(1 << 23) * s);
}

static int run(uint32_t rows, uint32_t mode) {
    const int TOTAL = Q_OUT + K_OUT + V_OUT, GUARD = 8;
    std::vector<fpga_axi128_t> x((QKV_X_DEPTH + 7) / 8), w((TOTAL * H + 7) / 8),
        b((TOTAL + 7) / 8), q((QKV_Q_DEPTH + 7) / 8), k((QKV_K_DEPTH + 7) / 8),
        v((QKV_V_DEPTH + 7) / 8);
    data_t *x16 = reinterpret_cast<data_t*>(x.data());
    data_t *w16 = reinterpret_cast<data_t*>(w.data());
    data_t *b16 = reinterpret_cast<data_t*>(b.data());
    data_t *q16 = reinterpret_cast<data_t*>(q.data());
    data_t *k16 = reinterpret_cast<data_t*>(k.data());
    data_t *v16 = reinterpret_cast<data_t*>(v.data());
    for (uint32_t i = 0; i < rows * H; ++i) x16[i] = rnd(1.0f);
    for (int i = 0; i < TOTAL * H; ++i) w16[i] = rnd(0.25f);
    for (int i = 0; i < TOTAL; ++i) b16[i] = rnd(0.5f);
    for (auto *p : {&q, &k, &v}) {
        data_t *d = reinterpret_cast<data_t*>(p->data());
        for (size_t i = 0; i < p->size() * 8; ++i) d[i] = 0x7bff;
    }
    std::vector<data_t> rq(rows * Q_OUT), rk(rows * K_OUT), rv(rows * V_OUT);
    common_result_v2 rr;
    qkv_projection_v2(x16, w16, b16, rq.data(), rk.data(), rv.data(), mode, rows, H,
                      FPGA_LAYOUT_W_OG32_I_L32_V2, FPGA_ABI_VERSION_V2, 1, 0, &rr);
    uint32_t status = 0, prow = 0, crc = 0, build = 0;
    uint64_t total = 0, compute = 0, reads = 0, writes = 0;
    qkv_projection_v2_hls(x.data(), w.data(), b.data(), q.data(), k.data(), v.data(),
                          mode, rows, H, FPGA_LAYOUT_W_OG32_I_L32_V2,
                          FPGA_ABI_VERSION_V2, 1, 0, status, prow, total, compute,
                          reads, writes, crc, build);
    if (status != FPGA_STATUS_V2_OK || prow != rows) return 1;
    if (memcmp(q16, rq.data(), rq.size() * 2) || memcmp(k16, rk.data(), rk.size() * 2) ||
        memcmp(v16, rv.data(), rv.size() * 2)) return 2;
    for (int i = 0; i < GUARD; ++i)
        if (q16[rq.size() + i] != 0x7bff || k16[rk.size() + i] != 0x7bff || v16[rv.size() + i] != 0x7bff) return 3;
    return 0;
}

int main() {
    int e = run(1, FPGA_RUN_DECODE);
    if (!e) { e = run(9, FPGA_RUN_PREFILL); if (e) e += 10; }
    if (e) { std::cout << "qkv_projection_v2 burst cosim FAIL " << e << "\n"; return e; }
    std::cout << "qkv_projection_v2 burst cosim PASS\n";
    return 0;
}
