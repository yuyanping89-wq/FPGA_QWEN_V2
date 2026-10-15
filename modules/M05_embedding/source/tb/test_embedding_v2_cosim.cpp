#include "include/embedding_v2.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// Test Bench only: do not add this file to Design Sources.
// Select either this file OR test_embedding_v2.cpp; both define main().
// All pointers are valid and all allocations cover the interface depth.
// Keep the full vocabulary: this tests the existing RTL without a smaller model.
namespace {
using m05_v2::axi_word_t;

// Independent reference dimensions and status values from the V2 contract.
const uint32_t kHidden = 896;
const uint32_t kVocab = 151936;
const uint32_t kMaxRows = 64;
const uint32_t kLanes = 8;
const uint32_t kRowWords = 112;
const size_t kTableWords = size_t(kVocab) * kRowWords;
const size_t kOutputWords = size_t(kMaxRows) * kRowWords;
const uint32_t kCaseCount = 12;
// Independent expected value; do not reuse the implementation's constant.
const uint32_t kExpectedBuildConfig = 0x10010011u;
uint32_t passed_cases = 0;

void require(bool condition, const std::string &message) {
    if (!condition) throw std::runtime_error(message);
}

uint16_t reference_bf16(uint32_t token, uint32_t hidden) {
    const uint16_t special[8] = {
        0x0000, 0x8000, 0x7f80, 0xff80, 0x7fc1, 0x7f81, 0x0001, 0x3f80
    };
    if (hidden < 8) return special[hidden];
    uint32_t bits = token * 0x9e3779b9u ^ hidden * 0x85ebca6bu;
    bits ^= bits >> 16;
    return static_cast<uint16_t>(bits);
}

void seed_row(std::vector<axi_word_t> &table, uint32_t token) {
    require(token < kVocab, "Test bench tried to initialize an invalid token");
    for (uint32_t w = 0; w < kRowWords; ++w) {
        axi_word_t word = 0;
        for (uint32_t lane = 0; lane < kLanes; ++lane) {
            word.range(lane * 16 + 15, lane * 16) =
                reference_bf16(token, w * kLanes + lane);
        }
        table[size_t(token) * kRowWords + w] = word;
    }
}

void check_row(const axi_word_t *row, uint32_t token, uint32_t output_row,
               const std::string &name) {
    // Compare each BF16 bit pattern with an independent scalar reference.
    for (uint32_t h = 0; h < kHidden; ++h) {
        const uint32_t lane = h % kLanes;
        const ap_uint<16> actual = row[h / kLanes].range(lane * 16 + 15, lane * 16);
        const uint16_t expected = reference_bf16(token, h);
        if (actual.to_uint() != expected) {
            std::ostringstream message;
            message << name << ": row=" << output_row << ", token=" << token
                    << ", hidden=" << h << ", expected=0x" << std::hex
                    << expected << ", actual=0x" << actual.to_uint();
            throw std::runtime_error(message.str());
        }
    }
}

void check_scalar(uint32_t actual, uint32_t expected, const std::string &field,
                  const std::string &name) {
    if (actual != expected) {
        std::ostringstream message;
        message << name << ": " << field << " expected=" << expected
                << ", actual=" << actual;
        throw std::runtime_error(message.str());
    }
}

void run_case(const std::string &name, const std::vector<axi_word_t> &table,
              const std::vector<uint32_t> &tokens, uint32_t valid_rows,
              uint32_t mode, uint32_t job,
              uint32_t expected_status, uint32_t completed) {
    require(!tokens.empty() && tokens.size() <= kMaxRows, "Invalid test size");
    require(completed <= tokens.size(), "Invalid expected completed count");
    require(completed <= valid_rows, "Completed count exceeds valid_rows");
    require(expected_status != 0 || completed == valid_rows,
            "Successful case must complete all requested rows");
    require(table.size() == kTableWords, "Table must match m_axi depth");

    // Allocate all 64 IDs / 7168 output words even for a one-token transaction.
    // The co-simulation wrapper may serialize the full declared interface depth.
    std::vector<uint32_t> ids(kMaxRows, kVocab);
    std::copy(tokens.begin(), tokens.end(), ids.begin());
    std::vector<axi_word_t> output(kOutputWords, axi_word_t(0));
    common_result_v2 result = {99, 99, 99, 99, 99, 99, 99, 99};

    std::cout << "[RUN] " << name << std::endl;
    embedding_v2(ids.data(), table.data(), output.data(), mode,
                 valid_rows, kHidden, kVocab,
                 2, job, 0, &result);

    check_scalar(result.status_code, expected_status, "status_code", name);
    check_scalar(result.processed_rows, completed, "processed_rows", name);
    require(result.perf_cycles_total == 0 && result.perf_cycles_compute == 0 &&
            result.perf_read_beats == 0 && result.perf_write_beats == 0 &&
            result.output_crc == 0,
            name + ": optional result fields should be zero in this version");
    check_scalar(result.build_config, kExpectedBuildConfig, "build_config", name);
    for (uint32_t row = 0; row < completed; ++row) {
        check_row(output.data() + size_t(row) * kRowWords, tokens[row], row, name);
    }

    // Intentionally inspect only completed rows here. Host-side guards outside
    // the mapped depth cannot prove that RTL did not perform an out-of-range
    // AXI transaction. Bus-level bounds/no-write assertions are a later step.
    // In particular this partial-error case checks the copied prefix and status;
    // it is NOT a complete proof that every later output address was untouched.
    ++passed_cases;
    std::cout << "[PASS] " << name << std::endl;
}
} // namespace

int main() {
    try {
        std::cout << "[INFO] M05 RTL boundary regression: " << kCaseCount
                  << " transactions, full vocabulary."
                  << std::endl;
        std::cout << "[INFO] Table payload: " << kTableWords * 16
                  << " bytes; host ap_uint size: " << sizeof(axi_word_t)
                  << " bytes. Co-simulation files can occupy several GB."
                  << std::endl;
        // Deterministically initialize every word that the HLS wrapper can read.
        std::vector<axi_word_t> table(kTableWords, axi_word_t(0));

        std::vector<uint32_t> batch(kMaxRows);
        uint32_t state = 20210929u;
        for (uint32_t row = 0; row < kMaxRows; ++row) {
            state = state * 1664525u + 1013904223u;
            batch[row] = state % kVocab;
        }
        // Explicit boundary / non-sorted / repeated IDs in the maximum batch.
        batch[0] = kVocab - 1;
        batch[1] = 0;
        batch[2] = 42;
        batch[3] = 42;
        batch[63] = kVocab - 1;
        for (uint32_t token : batch) seed_row(table, token);

        run_case("prefill T=64, boundaries and repeated tokens", table,
                 batch, 64, 0, 100, 0, 64);

        // ID == vocab_size is illegal. One complete row precedes the failure.
        run_case("bad middle token, processed_rows=1", table,
                 {0, kVocab, 42}, 3, 0, 101, 7, 1);

        // A new successful start after an error must clear the previous status.
        run_case("decode T=1 after error, last token", table,
                 {kVocab - 1}, 1, 1, 0xffffffffu, 0, 1);

        // Consecutive DATAFLOW failures exercise both an empty valid prefix
        // and the longest valid prefix before an illegal ID. Keep these calls
        // adjacent so recovery cannot be masked by an intervening success.
        run_case("bad first token UINT32_MAX, processed_rows=0", table,
                 {0xffffffffu, 0, 42}, 3, 0, 103, 7, 0);
        std::vector<uint32_t> bad_last = batch;
        bad_last[63] = kVocab;
        run_case("bad last token at row 63, processed_rows=63", table,
                 bad_last, 64, 0, 104, 7, 63);
        run_case("prefill T=64 after two token errors", table,
                 batch, 64, 0, 105, 0, 64);

        run_case("prefill T=1, first token", table,
                 {0}, 1, 0, 106, 0, 1);
        const std::vector<uint32_t> batch36(batch.begin(), batch.begin() + 36);
        run_case("prefill T=36", table,
                 batch36, 36, 0, 107, 0, 36);

        // Scalar valid_rows is independent of the physical ID allocation.
        // Even rejected jobs keep all pointers valid and allocations at their
        // declared depth. There are no host-null-pointer cases in this suite.
        run_case("reject valid_rows=0", table,
                 {0}, 0, 0, 108, 2, 0);
        run_case("reject valid_rows=65", table,
                 batch, 65, 0, 109, 2, 0);
        run_case("reject decode T=2", table,
                 {0, 42}, 2, 1, 110, 2, 0);
        run_case("decode T=1 after three parameter errors", table,
                 {42}, 1, 1, 111, 0, 1);

        check_scalar(passed_cases, kCaseCount, "passed case count", "suite");
        std::cout << "\nALL " << passed_cases
                  << " RTL-SUITABLE CASES PASSED" << std::endl;
        std::cout << "[INFO] This message alone does not prove RTL PASS. "
                     "Also check the final HLS co-simulation report." << std::endl;
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "\n[FAIL] " << error.what() << std::endl;
        return 1;
    }
}
