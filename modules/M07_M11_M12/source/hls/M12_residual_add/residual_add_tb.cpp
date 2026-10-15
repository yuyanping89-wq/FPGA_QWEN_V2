#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "residual_add_kernel.hpp"

static std::vector<float> read_bin(const std::string& path, size_t count) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) { std::fprintf(stderr, "ERROR: open %s\n", path.c_str()); std::exit(2); }
    std::vector<float> values(count);
    stream.read(reinterpret_cast<char*>(values.data()),
                static_cast<std::streamsize>(count * sizeof(float)));
    if (static_cast<size_t>(stream.gcount()) != count * sizeof(float)) {
        std::fprintf(stderr, "ERROR: short read %s\n", path.c_str()); std::exit(2);
    }
    return values;
}

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : "../out";
    int t, h;
    { std::ifstream dims(dir + "/dims.txt"); if (!(dims >> t >> h)) return 2; }
    if (h != H || t > T_MAX) return 2;
    const size_t used = static_cast<size_t>(t) * H;
    const size_t capacity = static_cast<size_t>(T_MAX) * H;
    std::vector<data_t> residual(capacity, 0);
    std::vector<data_t> attention(capacity, 0);
    std::vector<data_t> output(capacity, 0);
    std::vector<data_t> expected(used, 0);
    auto residual_used = read_bin(dir + "/residual.bin", used);
    auto attention_used = read_bin(dir + "/attention.bin", used);
    for (size_t i = 0; i < used; ++i) {
        residual[i] = float_to_bf16(residual_used[i]);
        attention[i] = float_to_bf16(attention_used[i]);
        expected[i] = float_to_bf16(
            bf16_to_float(residual[i]) + bf16_to_float(attention[i]));
    }

    attention_residual_add(residual.data(), attention.data(), output.data(), t);
    double max_abs = 0.0, mean_abs = 0.0;
    size_t mismatches = 0;
    for (size_t i = 0; i < used; ++i) {
        const double diff = std::fabs(
            static_cast<double>(bf16_to_float(output[i])) -
            static_cast<double>(bf16_to_float(expected[i])));
        if (diff > max_abs) max_abs = diff;
        mean_abs += diff;
        if (output[i] != expected[i]) ++mismatches;
    }
    mean_abs /= used;
    const bool ok = mismatches == 0;
    std::printf("[tb] T=%d H=%d dtype=bf16 mismatches=%zu "
                "max_abs=%.6e mean_abs=%.6e -> %s\n",
                t, H, mismatches, max_abs, mean_abs,
                ok ? "PASS" : "FAIL");
    std::printf("RESULT: %s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
