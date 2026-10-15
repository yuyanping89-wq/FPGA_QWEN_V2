// Feed packed real-model weights + golden inputs into the V2 HLS C++ kernels
// and compare outputs bit-exactly with pack_v2_weights.py golden files.
#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include "fpga_abi_v2.h"
typedef uint16_t data_t;
#include "kernel_decls.inc"
static std::vector<uint16_t> rd(const std::string& p, size_t n) {
    std::vector<uint16_t> v(n); FILE* f = fopen(p.c_str(), "rb");
    if (!f || fread(v.data(), 2, n, f) != n) { printf("READ_FAIL %s\n", p.c_str()); exit(2); }
    fclose(f); return v;
}
static int cmp(const char* tag, const std::vector<uint16_t>& a, const std::vector<uint16_t>& b) {
    size_t bad = 0; for (size_t i = 0; i < b.size(); ++i) if (a[i] != b[i]) ++bad;
    printf("%s mismatches=%zu/%zu\n", tag, bad, b.size()); return bad == 0;
}
int main(int argc, char** argv) {
    std::string w = argv[1], g = w + "/golden/layer00_rows";
    auto wqkv = rd(w + "/layer00_m07_wqkv.bin", 36 * 896 * 32);
    auto bqkv = rd(w + "/layer00_m07_bqkv.bin", 1152);
    auto wo = rd(w + "/layer00_m11_wo.bin", 28 * 896 * 32);
    int ok = 1;
    for (uint32_t rows : {1u, 8u}) {
        std::string p = g + std::to_string(rows) + "_";
        uint32_t mode = rows == 1 ? FPGA_RUN_DECODE : FPGA_RUN_PREFILL;
        common_result_v2 r;
        auto x = rd(p + "x.bin", rows * 896);
        std::vector<uint16_t> q(14 * rows * 64), k(2 * rows * 64), v(2 * rows * 64);
        qkv_projection_v2((const data_t*)x.data(), (const data_t*)wqkv.data(), (const data_t*)bqkv.data(),
                          (data_t*)q.data(), (data_t*)k.data(), (data_t*)v.data(),
                          mode, rows, 896, FPGA_LAYOUT_W_OG32_I_L32_V2, 2, 1, 0, &r);
        ok &= r.status_code == 0;
        ok &= cmp(("M07 q rows" + std::to_string(rows)).c_str(), q, rd(p + "q.bin", q.size()));
        ok &= cmp(("M07 k rows" + std::to_string(rows)).c_str(), k, rd(p + "k.bin", k.size()));
        ok &= cmp(("M07 v rows" + std::to_string(rows)).c_str(), v, rd(p + "v.bin", v.size()));
        auto attn = rd(p + "attn.bin", rows * 896);
        std::vector<uint16_t> o(rows * 896), y(rows * 896);
        attention_out_v2((const data_t*)attn.data(), (const data_t*)wo.data(), (data_t*)o.data(),
                         mode, rows, 896, FPGA_LAYOUT_W_OG32_I_L32_V2, 2, 2, 0, &r);
        ok &= r.status_code == 0;
        ok &= cmp(("M11 o rows" + std::to_string(rows)).c_str(), o, rd(p + "o.bin", o.size()));
        auto resid = rd(p + "resid.bin", rows * 896);
        residual_add_v2((const data_t*)resid.data(), (const data_t*)o.data(), (data_t*)y.data(),
                        mode, rows, 896, 0, 2, 3, 0, &r);
        ok &= r.status_code == 0;
        ok &= cmp(("M12 y rows" + std::to_string(rows)).c_str(), y, rd(p + "y.bin", y.size()));
    }
    printf(ok ? "REAL_WEIGHT_GOLDEN_PASS\n" : "REAL_WEIGHT_GOLDEN_FAIL\n");
    return ok ? 0 : 1;
}
