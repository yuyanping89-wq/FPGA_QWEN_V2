#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../include/fpga_abi_v2.h"

void qkv_projection_v2(const uint16_t*, const uint16_t*, const uint16_t*,
                       uint16_t*, uint16_t*, uint16_t*, uint32_t, uint32_t,
                       uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
void attention_out_v2(const uint16_t*, const uint16_t*, uint16_t*, uint32_t, uint32_t,
                      uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
void residual_add_v2(const uint16_t*, const uint16_t*, uint16_t*, uint32_t, uint32_t,
                     uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);

static uint16_t f2b(float x) {
    union { float f; uint32_t u; } v = {x};
    uint32_t lsb = (v.u >> 16) & 1U;
    return (uint16_t)((v.u + 0x7fffU + lsb) >> 16);
}
static float b2f(uint16_t x) {
    union { uint32_t u; float f; } v = {(uint32_t)x << 16};
    return v.f;
}
static bool same(uint16_t a, uint16_t b) { return a == b; }

int main() {
    const int H = 896, Q = 896, K = 128, D = 64, QH = 14, KVH = 2;
    const int T = 3;
    std::vector<uint16_t> x(T * H), wq(Q * H), wk(K * H), wv(K * H);
    std::vector<uint16_t> bq(Q), bk(K), bv(K);
    for (size_t i = 0; i < x.size(); ++i) x[i] = f2b((float)((int)i % 17 - 8) * 0.03125f);
    for (size_t i = 0; i < wq.size(); ++i) wq[i] = f2b((float)((int)(i % 13) - 6) * 0.0078125f);
    for (size_t i = 0; i < wk.size(); ++i) wk[i] = f2b((float)((int)(i % 11) - 5) * 0.0078125f);
    for (size_t i = 0; i < wv.size(); ++i) wv[i] = f2b((float)((int)(i % 7) - 3) * 0.0078125f);
    for (size_t i = 0; i < bq.size(); ++i) bq[i] = f2b((float)((int)i % 5 - 2) * 0.015625f);
    for (size_t i = 0; i < bk.size(); ++i) bk[i] = f2b((float)((int)i % 3 - 1) * 0.015625f);
    for (size_t i = 0; i < bv.size(); ++i) bv[i] = f2b((float)((int)i % 3 - 1) * 0.01171875f);
    std::vector<uint16_t> wqkv(36 * H * 32), bqkv(Q + K + K);
    for (int o = 0; o < Q + K + K; ++o) {
        const uint16_t *src = o < Q ? wq.data() : (o < Q + K ? wk.data() : wv.data());
        const int local = o < Q ? o : (o < Q + K ? o - Q : o - Q - K);
        const int cols = o < Q ? Q : K;
        for (int i = 0; i < H; ++i) wqkv[(o / 32 * H + i) * 32 + (o % 32)] = src[local * H + i];
        bqkv[o] = o < Q ? bq[o] : (o < Q + K ? bk[local] : bv[local]);
    }
    std::vector<uint16_t> q(QH * T * D), k(KVH * T * D), v(KVH * T * D);
    common_result_v2 r{};
    qkv_projection_v2(x.data(), wqkv.data(), bqkv.data(), q.data(), k.data(), v.data(),
                      FPGA_RUN_PREFILL, T, H, FPGA_LAYOUT_W_OG32_I_L32_V2,
                      FPGA_ABI_VERSION_V2, 17U, FPGA_FLAG_STRICT_CHECK, &r);
    if (r.status_code != FPGA_STATUS_V2_OK || r.processed_rows != (uint32_t)T) return 1;
    for (int h = 0; h < QH; ++h) for (int t = 0; t < T; ++t) for (int d = 0; d < D; ++d) {
        float acc = b2f(bq[h * D + d]);
        for (int i = 0; i < H; ++i) acc += b2f(x[t * H + i]) * b2f(wq[(h * D + d) * H + i]);
        if (!same(q[(h * T + t) * D + d], f2b(acc))) return 2;
    }

    /* M11 consumes the packed W_OG32_I_L32 layout. */
    std::vector<uint16_t> packed(Q * H);
    for (int o = 0; o < H; ++o) for (int i = 0; i < H; ++i)
        packed[(o / 32 * H + i) * 32 + (o % 32)] = wq[o * H + i];
    std::vector<uint16_t> proj(T * H), expected(T * H);
    attention_out_v2(x.data(), packed.data(), proj.data(), FPGA_RUN_PREFILL, T, H,
                     FPGA_LAYOUT_W_OG32_I_L32_V2, FPGA_ABI_VERSION_V2, 17U, 0U, &r);
    if (r.status_code != FPGA_STATUS_V2_OK) return 3;
    for (int t = 0; t < T; ++t) for (int o = 0; o < H; ++o) {
        float acc = 0.0f;
        for (int i = 0; i < H; ++i) acc += b2f(x[t * H + i]) * b2f(wq[o * H + i]);
        expected[t * H + o] = f2b(acc);
        if (!same(proj[t * H + o], expected[t * H + o])) return 4;
    }

    std::vector<uint16_t> residual(T * H), out(T * H);
    for (size_t i = 0; i < residual.size(); ++i) residual[i] = f2b((float)((int)i % 19 - 9) * 0.0625f);
    residual_add_v2(proj.data(), residual.data(), out.data(), FPGA_RUN_PREFILL, T, H, 0,
                    FPGA_ABI_VERSION_V2, 17U, 0U, &r);
    if (r.status_code != FPGA_STATUS_V2_OK) return 5;
    for (size_t i = 0; i < out.size(); ++i)
        if (!same(out[i], f2b(b2f(proj[i]) + b2f(residual[i])))) return 6;

    /* Decode is the same ABI with exactly one valid row. */
    qkv_projection_v2(x.data(), wqkv.data(), bqkv.data(), q.data(), k.data(), v.data(),
                      FPGA_RUN_DECODE, 1U, H, FPGA_LAYOUT_W_OG32_I_L32_V2,
                      FPGA_ABI_VERSION_V2, 18U, 0U, &r);
    if (r.status_code != FPGA_STATUS_V2_OK || r.processed_rows != 1U) return 7;
    qkv_projection_v2(x.data(), wqkv.data(), bqkv.data(), q.data(), k.data(), v.data(),
                      FPGA_RUN_DECODE, 2U, H, FPGA_LAYOUT_W_OG32_I_L32_V2,
                      FPGA_ABI_VERSION_V2, 18U, 0U, &r);
    if (r.status_code != FPGA_STATUS_V2_BAD_DIMENSION) return 8;
    std::cout << "M07_M11_M12_V2_PASS\n";
    return 0;
}
