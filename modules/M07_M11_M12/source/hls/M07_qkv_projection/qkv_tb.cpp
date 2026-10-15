// Module 4: Q/K/V projection — HLS testbench (also runs under plain g++).
//
// Reads the fp32 .bin files produced by ../qkv_reference.py --dump-bin,
// invokes the kernel, and checks kernel output against the Python fp32
// reference (q/k/v_ref_fp32.bin). Exit code 0 = PASS.
//
// Usage: ./qkv_tb <bin_dir>   (default: ../out)

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "qkv_kernel.hpp"

static std::vector<float> read_bin(const std::string& path, size_t n) {
    std::ifstream f(path, std::ios::binary);
    if (!f) { std::fprintf(stderr, "ERROR: open %s\n", path.c_str()); std::exit(2); }
    std::vector<float> b(n);
    f.read(reinterpret_cast<char*>(b.data()), (std::streamsize)(n * sizeof(float)));
    if ((size_t)f.gcount() != n * sizeof(float)) {
        std::fprintf(stderr, "ERROR: short read %s\n", path.c_str()); std::exit(2);
    }
    return b;
}

static std::vector<data_t> quantize(const std::vector<float>& values) {
    std::vector<data_t> result(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        result[i] = float_to_bf16(values[i]);
    }
    return result;
}

static std::vector<data_t> linear_reference(
    const std::vector<data_t>& x,
    const std::vector<data_t>& weight,
    const std::vector<data_t>& bias,
    int T,
    int OUT) {
    std::vector<data_t> result(static_cast<size_t>(T) * OUT);
    for (int t = 0; t < T; ++t) {
        for (int out = 0; out < OUT; ++out) {
            float accumulator = 0.0f;
            for (int h = 0; h < H; ++h) {
                accumulator +=
                    bf16_to_float(x[static_cast<size_t>(t) * H + h]) *
                    bf16_to_float(
                        weight[static_cast<size_t>(out) * H + h]);
            }
            result[static_cast<size_t>(t) * OUT + out] =
                float_to_bf16(accumulator + bf16_to_float(bias[out]));
        }
    }
    return result;
}

static bool check(const char* name, const std::vector<data_t>& got,
                  const std::vector<data_t>& ref, size_t n) {
    double maxabs = 0.0, sum = 0.0;
    size_t mismatches = 0;
    for (size_t i = 0; i < n; ++i) {
        double d = std::fabs(
            static_cast<double>(bf16_to_float(got[i])) -
            static_cast<double>(bf16_to_float(ref[i])));
        maxabs = d > maxabs ? d : maxabs;
        sum += d;
        if (got[i] != ref[i]) ++mismatches;
    }
    double mean = n == 0 ? 0.0 : sum / n;
    bool ok = mismatches == 0;
    std::printf("[%s] dtype=bf16 mismatches=%zu max_abs=%.3e "
                "mean_abs=%.3e -> %s\n",
                name, mismatches, maxabs, mean,
                ok ? "PASS" : "FAIL");
    return ok;
}

int main(int argc, char** argv) {
    std::string dir = (argc > 1) ? argv[1] : "../out";
    auto p = [&](const std::string& n) { return dir + "/" + n; };

    // dims.txt: T H Q_OUT K_OUT V_OUT
    int T, h, qo, ko, vo;
    { std::ifstream f(p("dims.txt"));
      if (!f || !(f >> T >> h >> qo >> ko >> vo)) {
          std::fprintf(stderr, "ERROR: read dims.txt\n"); return 2; } }
    if (h != H || qo != Q_OUT || ko != K_OUT || vo != V_OUT) {
        std::fprintf(stderr, "ERROR: dims mismatch vs kernel header\n"); return 2; }
    if (T > T_MAX) {
        std::fprintf(stderr, "ERROR: T=%d > T_MAX=%d (bump T_MAX)\n", T, T_MAX); return 2; }
    std::printf("[tb] T=%d H=%d Q_OUT=%d K_OUT=%d V_OUT=%d\n", T, H, Q_OUT, K_OUT, V_OUT);

    // Cosim wrapper uses the HLS interface depth, so runtime-T buffers must
    // be allocated to T_MAX even when the current test vector uses T < T_MAX.
    std::vector<data_t> x((size_t)T_MAX * H, 0);
    auto x_used = read_bin(p("x.bin"), (size_t)T * H);
    for (size_t i = 0; i < x_used.size(); ++i) {
        x[i] = float_to_bf16(x_used[i]);
    }

    auto Wq = quantize(read_bin(p("Wq.bin"), (size_t)Q_OUT * H));
    auto bq = quantize(read_bin(p("bq.bin"), (size_t)Q_OUT));
    auto Wk = quantize(read_bin(p("Wk.bin"), (size_t)K_OUT * H));
    auto bk = quantize(read_bin(p("bk.bin"), (size_t)K_OUT));
    auto Wv = quantize(read_bin(p("Wv.bin"), (size_t)V_OUT * H));
    auto bv = quantize(read_bin(p("bv.bin"), (size_t)V_OUT));

    const auto qref = linear_reference(x, Wq, bq, T, Q_OUT);
    const auto kref = linear_reference(x, Wk, bk, T, K_OUT);
    const auto vref = linear_reference(x, Wv, bv, T, V_OUT);

    std::vector<data_t> q((size_t)T_MAX * Q_OUT);
    std::vector<data_t> k((size_t)T_MAX * K_OUT);
    std::vector<data_t> v((size_t)T_MAX * V_OUT);
    qkv_proj(x.data(), Wq.data(), bq.data(), Wk.data(), bk.data(),
             Wv.data(), bv.data(), q.data(), k.data(), v.data(), T);

    bool ok = true;
    ok &= check("q", q, qref, (size_t)T * Q_OUT);
    ok &= check("k", k, kref, (size_t)T * K_OUT);
    ok &= check("v", v, vref, (size_t)T * V_OUT);
    std::printf("RESULT: %s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
