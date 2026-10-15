// Host check for the burst-optimized *_v2_hls tops: compares them bit-exactly
// with the scalar reference *_v2 functions (already board-verified) across
// row counts that cross the 8-row block boundary, plus real Qwen weights when
// a packed weight directory is given.
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "../include/fpga_abi_v2.h"
#include "../include/fpga_axi128.h"

typedef uint16_t data_t;
void qkv_projection_v2(const uint16_t*, const uint16_t*, const uint16_t*,
                       uint16_t*, uint16_t*, uint16_t*, uint32_t, uint32_t,
                       uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
void attention_out_v2(const uint16_t*, const uint16_t*, uint16_t*, uint32_t, uint32_t,
                      uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
void residual_add_v2(const uint16_t*, const uint16_t*, uint16_t*, uint32_t, uint32_t,
                     uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
#define RESULT_OUT uint32_t&, uint32_t&, uint64_t&, uint64_t&, uint64_t&, uint64_t&, uint32_t&, uint32_t&
void qkv_projection_v2_hls(const fpga_axi128_t*, const fpga_axi128_t*, const fpga_axi128_t*,
                           fpga_axi128_t*, fpga_axi128_t*, fpga_axi128_t*, uint32_t, uint32_t,
                           uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, RESULT_OUT);
void attention_out_v2_hls(const fpga_axi128_t*, const fpga_axi128_t*, fpga_axi128_t*, uint32_t,
                          uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, RESULT_OUT);
void residual_add_v2_hls(const fpga_axi128_t*, const fpga_axi128_t*, fpga_axi128_t*, uint32_t,
                         uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, RESULT_OUT);

static const uint32_t H = 896, QKV = 1152, GUARD = 64;
static const uint16_t FILL = 0x7bff;

struct Res { uint32_t st, rows, crc, cfg; uint64_t c0, c1, rb, wb; };
#define RES_ARGS(r) r.st, r.rows, r.c0, r.c1, r.rb, r.wb, r.crc, r.cfg

// 128-bit beat buffers viewed as BF16 arrays (host fpga_axi128_t is lo/hi LE).
struct Buf {
    std::vector<fpga_axi128_t> beats;
    explicit Buf(size_t n16) : beats((n16 + 7) / 8) {}
    uint16_t* p() { return reinterpret_cast<uint16_t*>(beats.data()); }
    fpga_axi128_t* b() { return beats.data(); }
};

static uint32_t rng = 12345;
static uint16_t rnd_bf16(float scale) {
    rng = rng * 1664525U + 1013904223U;
    float f = ((int32_t)(rng >> 8) - (1 << 23)) / (float)(1 << 23) * scale;
    union { float f; uint32_t u; } v = {f};
    return (uint16_t)((v.u + 0x7fffU + ((v.u >> 16) & 1U)) >> 16);
}
static void fill_rand(uint16_t* p, size_t n, float s) { for (size_t i = 0; i < n; ++i) p[i] = rnd_bf16(s); }
static int fails = 0;
static void expect(bool ok, const char* what, uint32_t rows) {
    if (!ok) { ++fails; printf("FAIL %s rows=%u\n", what, rows); }
}
static bool same(const uint16_t* a, const uint16_t* b, size_t n) { return memcmp(a, b, n * 2) == 0; }
static bool guard_ok(const uint16_t* p, size_t n) { for (size_t i = 0; i < GUARD; ++i) if (p[n + i] != FILL) return false; return true; }

static void test_m07(uint32_t rows, const uint16_t* w, const uint16_t* bias, uint32_t mode) {
    Buf x(rows * H), wb(QKV * H), bb(QKV), q(rows * 896 + GUARD), k(rows * 128 + GUARD), v(rows * 128 + GUARD);
    fill_rand(x.p(), rows * H, 2.0f);
    memcpy(wb.p(), w, QKV * H * 2); memcpy(bb.p(), bias, QKV * 2);
    std::vector<uint16_t> rq(rows * 896), rk(rows * 128), rv(rows * 128);
    common_result_v2 ref;
    qkv_projection_v2(x.p(), wb.p(), bb.p(), rq.data(), rk.data(), rv.data(), mode, rows, H, 6, 2, 7, 0, &ref);
    for (auto* bf : {&q, &k, &v}) std::fill(bf->p(), bf->p() + bf->beats.size() * 8, FILL);
    Res r;
    qkv_projection_v2_hls(x.b(), wb.b(), bb.b(), q.b(), k.b(), v.b(), mode, rows, H, 6, 2, 7, 0, RES_ARGS(r));
    expect(r.st == ref.status_code && r.rows == ref.processed_rows, "M07 status", rows);
    if (ref.status_code == 0) {
        expect(same(q.p(), rq.data(), rq.size()) && same(k.p(), rk.data(), rk.size()) && same(v.p(), rv.data(), rv.size()), "M07 data", rows);
        expect(guard_ok(q.p(), rq.size()) && guard_ok(k.p(), rk.size()) && guard_ok(v.p(), rv.size()), "M07 guard", rows);
    } else {
        expect(guard_ok(q.p(), 0) && q.p()[0] == FILL, "M07 untouched on error", rows);
    }
}

static void test_m11(uint32_t rows, const uint16_t* w, uint32_t mode) {
    Buf x(rows * H), wb(H * H), y(rows * H + GUARD);
    fill_rand(x.p(), rows * H, 1.0f);
    memcpy(wb.p(), w, H * H * 2);
    std::vector<uint16_t> ry(rows * H);
    common_result_v2 ref;
    attention_out_v2(x.p(), wb.p(), ry.data(), mode, rows, H, 6, 2, 8, 0, &ref);
    std::fill(y.p(), y.p() + y.beats.size() * 8, FILL);
    Res r;
    attention_out_v2_hls(x.b(), wb.b(), y.b(), mode, rows, H, 6, 2, 8, 0, RES_ARGS(r));
    expect(r.st == ref.status_code && r.rows == ref.processed_rows, "M11 status", rows);
    if (ref.status_code == 0) {
        expect(same(y.p(), ry.data(), ry.size()), "M11 data", rows);
        expect(guard_ok(y.p(), ry.size()), "M11 guard", rows);
    } else {
        expect(y.p()[0] == FILL, "M11 untouched on error", rows);
    }
}

static void test_m12(uint32_t rows, uint32_t role, uint32_t mode) {
    Buf a(rows * H), b(rows * H), y(rows * H + GUARD);
    fill_rand(a.p(), rows * H, 4.0f); fill_rand(b.p(), rows * H, 4.0f);
    std::vector<uint16_t> ry(rows * H);
    common_result_v2 ref;
    residual_add_v2(a.p(), b.p(), ry.data(), mode, rows, H, role, 2, 9, 0, &ref);
    std::fill(y.p(), y.p() + y.beats.size() * 8, FILL);
    Res r;
    residual_add_v2_hls(a.b(), b.b(), y.b(), mode, rows, H, role, 2, 9, 0, RES_ARGS(r));
    expect(r.st == ref.status_code && r.rows == ref.processed_rows, "M12 status", rows);
    if (ref.status_code == 0) {
        expect(same(y.p(), ry.data(), ry.size()) && guard_ok(y.p(), ry.size()), "M12 data/guard", rows);
    } else {
        expect(y.p()[0] == FILL, "M12 untouched on error", rows);
    }
}

static std::vector<uint16_t> read_file(const std::string& path, size_t n) {
    std::vector<uint16_t> v(n);
    FILE* f = fopen(path.c_str(), "rb");
    if (!f || fread(v.data(), 2, n, f) != n) { printf("cannot read %s\n", path.c_str()); exit(2); }
    fclose(f);
    return v;
}

int main(int argc, char** argv) {
    std::vector<uint16_t> wqkv(QKV * H), bias(QKV), wo(H * H);
    fill_rand(wqkv.data(), wqkv.size(), 0.05f); fill_rand(bias.data(), bias.size(), 0.5f); fill_rand(wo.data(), wo.size(), 0.05f);
    const uint32_t row_cases[] = {1, 2, 7, 8, 9, 17, 64};
    for (uint32_t rows : row_cases) {
        const uint32_t mode = rows == 1 ? FPGA_RUN_DECODE : FPGA_RUN_PREFILL;
        test_m07(rows, wqkv.data(), bias.data(), mode);
        test_m11(rows, wo.data(), mode);
        test_m12(rows, rows & 1U, mode);
    }
    // Error paths: Decode with rows=2, rows=0, rows=65 must not touch outputs.
    test_m07(2, wqkv.data(), bias.data(), FPGA_RUN_DECODE);
    test_m11(2, wo.data(), FPGA_RUN_DECODE);
    test_m12(2, 0, FPGA_RUN_DECODE);
    test_m12(65, 0, FPGA_RUN_PREFILL);
    test_m12(1, 2, FPGA_RUN_DECODE); // bad residual_role
    if (argc > 1) { // real packed Qwen weights (tools/pack_v2_weights.py output)
        std::string d = argv[1];
        auto rw = read_file(d + "/layer00_m07_wqkv.bin", QKV * H);
        auto rbias = read_file(d + "/layer00_m07_bqkv.bin", QKV);
        auto rwo = read_file(d + "/layer00_m11_wo.bin", H * H);
        for (uint32_t rows : {1u, 8u, 13u}) {
            const uint32_t mode = rows == 1 ? FPGA_RUN_DECODE : FPGA_RUN_PREFILL;
            test_m07(rows, rw.data(), rbias.data(), mode);
            test_m11(rows, rwo.data(), mode);
        }
        printf("real weights checked\n");
    }
    printf(fails ? "V2_BURST_HOST_FAIL (%d)\n" : "V2_BURST_HOST_PASS\n", fails);
    return fails ? 1 : 0;
}
