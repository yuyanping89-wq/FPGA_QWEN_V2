#include "swiglu_v2.h"
#include "common_numeric_v2.h"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <new>

#ifdef _WIN32
#include <malloc.h>
#endif

class AlignedWords {
public:
    explicit AlignedWords(size_t count) : words_(0), count_(count) {
#ifdef _WIN32
        words_ = static_cast<swiglu_axi128_t *>(
            _aligned_malloc(count * sizeof(swiglu_axi128_t), 64));
#else
        void *storage = 0;
        if (posix_memalign(&storage, 64,
                           count * sizeof(swiglu_axi128_t)) == 0) {
            words_ = static_cast<swiglu_axi128_t *>(storage);
        }
#endif
        if (words_ == 0) throw std::bad_alloc();
        for (size_t i = 0; i < count_; ++i) {
            new (&words_[i]) swiglu_axi128_t(0);
        }
    }

    ~AlignedWords() {
        if (words_ != 0) {
            for (size_t i = 0; i < count_; ++i) words_[i].~swiglu_axi128_t();
#ifdef _WIN32
            _aligned_free(words_);
#else
            std::free(words_);
#endif
        }
    }

    swiglu_axi128_t *data() { return words_; }
    const swiglu_axi128_t *data() const { return words_; }
    swiglu_axi128_t &operator[](size_t index) { return words_[index]; }
    const swiglu_axi128_t &operator[](size_t index) const {
        return words_[index];
    }

private:
    AlignedWords(const AlignedWords &);
    AlignedWords &operator=(const AlignedWords &);
    swiglu_axi128_t *words_;
    size_t count_;
};

enum NumericCase {
    NUMERIC_NONE = 0,
    NUMERIC_SPARSE = 1,
    NUMERIC_NAN = 2
};

static void set_lane(swiglu_axi128_t &word, int lane, uint16_t value) {
    word |= swiglu_axi128_t(value) << (lane * 16);
}

static uint16_t get_lane(const swiglu_axi128_t &word, int lane) {
    return (uint16_t)(word >> (lane * 16));
}

static bool word_is_bf16_pattern(
    const swiglu_axi128_t &word,
    uint16_t pattern) {
    for (int lane = 0; lane < SWIGLU_V2_BF16_PER_BEAT; ++lane) {
        if (get_lane(word, lane) != pattern) return false;
    }
    return true;
}

static void set_og32_weight(
    AlignedWords &words,
    int output,
    int input,
    int input_size,
    uint16_t value) {
    const int output_group = output / 32;
    const int output_in_group = output % 32;
    const int beat = output_in_group / SWIGLU_V2_BF16_PER_BEAT;
    const int lane = output_in_group % SWIGLU_V2_BF16_PER_BEAT;
    const int word =
        (output_group * input_size + input) * 4 + beat;
    set_lane(words[word], lane, value);
}

static int run_case(uint32_t run_mode, uint32_t valid_rows, uint32_t abi_version,
                    uint32_t expected_status,
                    NumericCase numeric_case = NUMERIC_NONE,
                    uint32_t flags = 0u,
                    uint32_t layout_id = SWIGLU_V2_LAYOUT_W_OG32_I_L32,
                    uint32_t hidden_size = SWIGLU_V2_HIDDEN_SIZE,
                    uint32_t intermediate_size = SWIGLU_V2_INTERMEDIATE_SIZE) {
    const int activation_words =
        SWIGLU_V2_MAX_ROWS * SWIGLU_V2_HIDDEN_SIZE /
        SWIGLU_V2_BF16_PER_BEAT;
    const int weight_words =
        SWIGLU_V2_HIDDEN_SIZE * SWIGLU_V2_INTERMEDIATE_SIZE /
        SWIGLU_V2_BF16_PER_BEAT;

    AlignedWords x(activation_words);
    AlignedWords w_gate(weight_words);
    AlignedWords w_up(weight_words);
    AlignedWords w_down(weight_words);
    AlignedWords y(activation_words);

    swiglu_axi128_t unwritten_sentinel = 0;
    for (int lane = 0; lane < SWIGLU_V2_BF16_PER_BEAT; ++lane) {
        set_lane(unwritten_sentinel, lane, 0xdeadu);
    }
    for (int word = 0; word < activation_words; ++word) {
        y[word] = unwritten_sentinel;
    }

    if (numeric_case == NUMERIC_SPARSE) {
        // Exercise all five rotating accumulator lanes, the 63/64/65 tile
        // boundary, and the final intermediate element (index 4863).
        static const int mids[] = {0, 1, 2, 3, 4, 63, 64, 65, 4863};
        static const int mid_count = sizeof(mids) / sizeof(mids[0]);
        const uint16_t one = common_numeric_v2::fp32_to_bf16_rne(1.0f);
        for (uint32_t row = 0; row < valid_rows; ++row) {
            for (int input = 0; input < 5; ++input) {
                set_lane(x[row * (SWIGLU_V2_HIDDEN_SIZE /
                                  SWIGLU_V2_BF16_PER_BEAT)], input, one);
            }
        }
        for (int input = 0; input < 5; ++input) {
            for (int index = 0; index < mid_count; ++index) {
                const int mid = mids[index];
                set_og32_weight(
                    w_gate, mid, input, SWIGLU_V2_HIDDEN_SIZE, one);
                set_og32_weight(
                    w_up, mid, input, SWIGLU_V2_HIDDEN_SIZE, one);
            }
        }
        for (int index = 0; index < mid_count; ++index) {
            set_og32_weight(
                w_down, 0, mids[index], SWIGLU_V2_INTERMEDIATE_SIZE, one);
        }
    } else if (numeric_case == NUMERIC_NAN) {
        const uint16_t one = common_numeric_v2::fp32_to_bf16_rne(1.0f);
        set_lane(x[0], 0, 0x7f81u);
        set_og32_weight(
            w_gate, 0, 0, SWIGLU_V2_HIDDEN_SIZE, one);
        set_og32_weight(
            w_up, 0, 0, SWIGLU_V2_HIDDEN_SIZE, one);
        set_og32_weight(
            w_down, 0, 0, SWIGLU_V2_INTERMEDIATE_SIZE, one);
    }

    uint32_t status = 0;
    uint32_t processed_rows = 0;
    uint64_t cycles_total = 0;
    uint64_t cycles_compute = 0;
    uint64_t read_beats = 0;
    uint64_t write_beats = 0;
    uint32_t output_crc = 0;
    uint32_t build_config = 0;
    uint32_t mid_tiles = 0;
    uint64_t gate_up_cycles = 0;
    uint64_t down_cycles = 0;
    uint64_t weight_wait_cycles = 0;
    uint64_t fifo_stall_cycles = 0;

    swiglu_v2(
        x.data(), w_gate.data(), w_up.data(), w_down.data(), y.data(),
        run_mode, valid_rows, hidden_size,
        intermediate_size, layout_id,
        abi_version, 123u, flags, status, processed_rows, cycles_total,
        cycles_compute, read_beats, write_beats, output_crc, build_config,
        mid_tiles, gate_up_cycles, down_cycles, weight_wait_cycles,
        fifo_stall_cycles);

    if (status != expected_status) {
        std::printf("status mismatch: got=%u expected=%u\n", status,
                    expected_status);
        return 1;
    }
    if (status == SWIGLU_V2_OK && processed_rows != valid_rows) {
        std::printf("processed_rows mismatch: got=%u expected=%u\n",
                    processed_rows, valid_rows);
        return 1;
    }
    if (build_config == 0u) {
        std::printf("build_config was not reported\n");
        return 1;
    }
    const uint32_t expected_build_config =
        (uint32_t)COMMON_PAR_CODE_8 |
        ((uint32_t)SWIGLU_V2_ROW_TILE << 4) | (64u << 10) |
        (1u << 18) | ((uint32_t)COMMON_CLOCK_CODE_100_MHZ << 22) |
        ((uint32_t)SWIGLU_V2_MODULE_REVISION << 28);
    if (build_config != expected_build_config) {
        std::printf("build_config mismatch: got=0x%08x expected=0x%08x\n",
                    build_config, expected_build_config);
        return 1;
    }
    if (cycles_total != 0u || cycles_compute != 0u ||
         read_beats != 0u || write_beats != 0u ||
         gate_up_cycles != 0u || down_cycles != 0u) {
        std::printf("Csim must report unavailable measured counters as zero\n");
        return 1;
    }
    if (status == SWIGLU_V2_OK) {
        uint32_t crc=0xffffffffU;
        for (unsigned i=0;i<valid_rows*SWIGLU_V2_HIDDEN_SIZE;++i) {
            uint16_t v=get_lane(y[i/8],i%8);
            for (unsigned byte=0;byte<2;++byte) for(unsigned bit=0;bit<8;++bit) {
                unsigned feedback=(crc^(v>>(byte*8+bit)))&1U;
                crc>>=1;if(feedback)crc^=0xedb88320U;
            }
        }
        if(output_crc!=((flags&1U)?~crc:0U)) {std::printf("M13 CRC mismatch\n");return 1;}
    }
    if (status == SWIGLU_V2_OK) {
        const uint32_t scheduled_rows =
            run_mode == SWIGLU_V2_DECODE ? 1u : SWIGLU_V2_ROW_TILE;
        const uint32_t expected_row_blocks =
            (valid_rows + scheduled_rows - 1u) / scheduled_rows;
        const uint32_t expected_mid_tiles = expected_row_blocks *
            (SWIGLU_V2_INTERMEDIATE_SIZE / SWIGLU_V2_MID_TILE);
        if (mid_tiles != expected_mid_tiles ||
            weight_wait_cycles != 0u || fifo_stall_cycles != 0u) {
            std::printf("M13 performance counter semantics mismatch\n");
            return 1;
        }
        if (valid_rows < SWIGLU_V2_MAX_ROWS &&
            !word_is_bf16_pattern(
                y[valid_rows * (SWIGLU_V2_HIDDEN_SIZE /
                                SWIGLU_V2_BF16_PER_BEAT)],
                0xdeadu)) {
            std::printf("kernel wrote beyond valid_rows=%u\n", valid_rows);
            return 1;
        }
    }
    if (status == SWIGLU_V2_OK && numeric_case == NUMERIC_SPARSE) {
        const float silu5 = 5.0f / (1.0f + std::exp(-5.0f));
        const float expected = 45.0f * silu5;
        const uint16_t expected_bf16 =
            common_numeric_v2::fp32_to_bf16_rne(expected);
        for (uint32_t row = 0; row < valid_rows; ++row) {
            const uint16_t actual_bf16 = get_lane(
                y[row * (SWIGLU_V2_HIDDEN_SIZE /
                         SWIGLU_V2_BF16_PER_BEAT)], 0);
            if (actual_bf16 != expected_bf16) {
                std::printf(
                    "sparse BF16 result mismatch row=%u: got=0x%04x expected=0x%04x\n",
                    row, actual_bf16, expected_bf16);
                return 1;
            }
        }
    }
    if (status == SWIGLU_V2_OK && numeric_case == NUMERIC_NAN) {
        const uint16_t actual_bf16 = get_lane(y[0], 0);
        if (actual_bf16 != 0x7fc0u) {
            std::printf("NaN policy mismatch: got=0x%04x expected=0x7fc0\n",
                        actual_bf16);
            return 1;
        }
    }
    return 0;
}

static int test_common_numeric() {
    int failed = 0;
    const float tie_even_down =
        common_numeric_v2::fp32_from_bits(0x3f808000u);
    const float tie_even_up =
        common_numeric_v2::fp32_from_bits(0x3f818000u);
    const float positive_nan =
        common_numeric_v2::fp32_from_bits(0x7f800001u);
    const float negative_nan =
        common_numeric_v2::fp32_from_bits(0xff812345u);

    if (common_numeric_v2::fp32_to_bf16_rne(tie_even_down) != 0x3f80u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(tie_even_up) != 0x3f82u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(positive_nan) != 0x7fc0u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(negative_nan) != 0xffc0u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(
            common_numeric_v2::fp32_from_bits(0x7f800000u)) != 0x7f80u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(
            common_numeric_v2::fp32_from_bits(0xff800000u)) != 0xff80u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(
            common_numeric_v2::fp32_from_bits(0x7f7fffffu)) != 0x7f80u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(
            common_numeric_v2::fp32_from_bits(0x00008000u)) != 0x0000u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(
            common_numeric_v2::fp32_from_bits(0x00008001u)) != 0x0001u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(
            common_numeric_v2::fp32_from_bits(0x80000001u)) != 0x8000u)
        ++failed;
    if (common_numeric_v2::fp32_to_bf16_rne(
            common_numeric_v2::bf16_to_fp32(0x0001u)) != 0x0001u)
        ++failed;
    if (common_numeric_v2::fp32_to_bits(
            common_numeric_v2::bf16_to_fp32(0x7f81u)) != 0x7fc00000u)
        ++failed;
    if (common_numeric_v2::fp32_to_bits(
            common_numeric_v2::bf16_to_fp32(0xff81u)) != 0xffc00000u)
        ++failed;

    if (failed != 0) {
        std::printf("common numeric policy failures: %d\n", failed);
    }
    return failed;
}

int main() {
    int failed = test_common_numeric();

    // Keep exhaustive functional/boundary cases in C simulation. The RTL
    // smoke validates AXI/control/status behavior with an early-reject job.
    // Full fixed-size floating datapath RTL simulation is tracked separately
    // because the XSIM 2021.1 DSP model emits a warning every few cycles.
    if (std::getenv("SWIGLU_V2_COSIM_SMOKE") != 0) {
        failed += run_case(SWIGLU_V2_DECODE, 1, SWIGLU_V2_ABI_VERSION,
                           SWIGLU_V2_BAD_FLAGS, NUMERIC_NONE, 4u);
        failed += run_case(SWIGLU_V2_DECODE, 1, SWIGLU_V2_ABI_VERSION,
                           SWIGLU_V2_BAD_FLAGS, NUMERIC_NONE, 0x80000000u);
        failed += run_case(SWIGLU_V2_DECODE, 1, 1u,
                           SWIGLU_V2_BAD_ABI, NUMERIC_NONE);
        failed += run_case(2u, 1, SWIGLU_V2_ABI_VERSION,
                           SWIGLU_V2_BAD_DIMENSION, NUMERIC_NONE);
        std::printf("swiglu_v2 RTL control smoke test: %s\n",
                    failed == 0 ? "PASS" : "FAIL");
        return failed == 0 ? 0 : 1;
    }

    failed += run_case(SWIGLU_V2_DECODE, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_OK, NUMERIC_SPARSE);
    failed += run_case(SWIGLU_V2_PREFILL, SWIGLU_V2_ROW_TILE,
                       SWIGLU_V2_ABI_VERSION, SWIGLU_V2_OK, NUMERIC_SPARSE);
    failed += run_case(SWIGLU_V2_DECODE, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_OK, NUMERIC_NAN);
    failed += run_case(SWIGLU_V2_DECODE, 2, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_DIMENSION);
    failed += run_case(2u, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_DIMENSION);
    failed += run_case(SWIGLU_V2_PREFILL, 0, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_DIMENSION);
    failed += run_case(SWIGLU_V2_PREFILL, 65, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_DIMENSION);
    failed += run_case(SWIGLU_V2_PREFILL, 1, 1,
                       SWIGLU_V2_BAD_ABI);
    failed += run_case(SWIGLU_V2_PREFILL, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_LAYOUT, NUMERIC_NONE, 0u, 99u);
    failed += run_case(SWIGLU_V2_PREFILL, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_DIMENSION, NUMERIC_NONE, 0u,
                       SWIGLU_V2_LAYOUT_W_OG32_I_L32, 895u);
    failed += run_case(SWIGLU_V2_PREFILL, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_DIMENSION, NUMERIC_NONE, 0u,
                       SWIGLU_V2_LAYOUT_W_OG32_I_L32,
                       SWIGLU_V2_HIDDEN_SIZE, 4863u);
    // CRC, PERF and STRICT share the common flags with the other modules.
    failed += run_case(SWIGLU_V2_DECODE, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_OK, NUMERIC_SPARSE, 11u);
    failed += run_case(SWIGLU_V2_PREFILL, SWIGLU_V2_ROW_TILE+1,
                       SWIGLU_V2_ABI_VERSION, SWIGLU_V2_OK, NUMERIC_SPARSE, 2u);
    failed += run_case(SWIGLU_V2_DECODE, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_FLAGS, NUMERIC_NONE, 4u);
    failed += run_case(SWIGLU_V2_DECODE, 1, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_BAD_FLAGS, NUMERIC_NONE, 0x80000000u);

#ifdef SWIGLU_V2_EXTENDED_TESTS
    // Required block-size regression points. Zero weights make the expected
    // result exact while still executing every tile and every valid row.
    failed += run_case(SWIGLU_V2_PREFILL, 36, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_OK, NUMERIC_NONE);
    failed += run_case(SWIGLU_V2_PREFILL, 64, SWIGLU_V2_ABI_VERSION,
                       SWIGLU_V2_OK, NUMERIC_NONE);
#endif

    std::printf("swiglu_v2 interface test: %s\n",
                failed == 0 ? "PASS" : "FAIL");
    return failed == 0 ? 0 : 1;
}
