#include "include/embedding_v2.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// 本文件仅作为 HLS Test Bench，不作为待综合的设计源码。
// 使用 AMD 的真实 ap_uint<128>；不读取模型文件，也不需要 FPGA 板卡。
namespace {

using m05_v2::axi_word_t;

// 独立记录文档规格，不直接使用被测代码的行跨度计算作为参考答案。
const uint32_t kHidden = 896;
const uint32_t kVocab = 151936;
const uint32_t kRowsMax = 64;
const uint32_t kLanes = 8;
const uint32_t kRowWords = kHidden / kLanes;
const size_t kTableWords = size_t(kVocab) * kRowWords;
const size_t kGuardWords = 4;
// Independent expected profile: revision=1, clock=0, numeric=0,
// maximum burst=64, row tile=1, eight-lane code=1.
const uint32_t kExpectedBuildConfig = 0x10010011u;

// 公共状态码的预期值，独立于被测头文件中的枚举。
enum ExpectedStatus { OK = 0, BAD_ABI = 1, BAD_DIMENSION = 2,
                      BAD_ADDRESS_RANGE = 4, BAD_TOKEN = 7, BAD_FLAGS = 11 };
enum NullPointer { NONE, IDS_NULL, TABLE_NULL, OUTPUT_NULL, RESULT_NULL };

struct Params {
    uint32_t mode = 0;
    uint32_t rows = 1;
    uint32_t hidden = kHidden;
    uint32_t vocab = kVocab;
    uint32_t abi = 2;
    uint32_t job = 123;
    uint32_t flags = 0;
};

void require(bool condition, const std::string &message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// 给定 token 编号和 hidden 下标，独立生成一个 16-bit 参考值。
// 前 8 项故意包含正负零、正负无穷、不同 NaN 位模式、最小非正规数和 1。
// M05 只负责复制，不能对这些位模式进行浮点转换或规范化。
uint16_t reference_bf16(uint32_t token, uint32_t h) {
    const uint16_t special[] = {
        0x0000, 0x8000, 0x7f80, 0xff80, 0x7fc1, 0x7f81, 0x0001, 0x3f80
    };
    if (h < 8) {
        return special[h];
    }
    uint32_t bits = token * 0x9e3779b9u ^ h * 0x85ebca6bu;
    bits ^= bits >> 16;
    return static_cast<uint16_t>(bits);
}

axi_word_t guard_word() {
    axi_word_t word = 0;
    for (uint32_t lane = 0; lane < kLanes; ++lane) {
        word.range(lane * 16 + 15, lane * 16) = 0xa5e0u + lane;
    }
    return word;
}

bool same_result(const common_result_v2 &a, const common_result_v2 &b) {
    return a.status_code == b.status_code && a.processed_rows == b.processed_rows &&
           a.perf_cycles_total == b.perf_cycles_total &&
           a.perf_cycles_compute == b.perf_cycles_compute &&
           a.perf_read_beats == b.perf_read_beats && a.perf_write_beats == b.perf_write_beats &&
           a.output_crc == b.output_crc && a.build_config == b.build_config;
}

// 固定种子的可重复 token 序列，并覆盖词表首尾行。
std::vector<uint32_t> varied_ids(uint32_t rows) {
    std::vector<uint32_t> ids(rows);
    uint32_t state = 20210929u;
    for (uint32_t t = 0; t < rows; ++t) {
        state = state * 1664525u + 1013904223u;
        ids[t] = state % kVocab;
    }
    if (rows > 0) ids.front() = 0;
    if (rows > 1) ids.back() = kVocab - 1;
    return ids;
}

class TestBench {
public:
    TestBench()
        : guard_(guard_word()), table_(kTableWords + 2 * kGuardWords, guard_),
          output_(size_t(kRowsMax) * kRowWords + 2 * kGuardWords, guard_),
          seeded_(kVocab, false), passed_(0) {
        std::cout << "[INFO] Full logical table: " << kTableWords * 16
                  << " bytes; host ap_uint size: " << sizeof(axi_word_t)
                  << " bytes. Only selected rows need reference data." << std::endl;
    }

    // 每个用例都执行：准备输入 -> 调用真实函数 -> 检查结果和边界。
    void run(const std::string &name, const Params &p, const std::vector<uint32_t> &ids,
             uint32_t expected_status, uint32_t completed, NullPointer null_ptr = NONE) {
        require(ids.size() <= kRowsMax, name + ": too many test IDs");
        require(completed <= ids.size(), name + ": invalid test expectation");
        for (uint32_t id : ids) {
            if (id < kVocab) seed_row(id);
        }

        // 有效输入之后放非法 ID；若误处理了额外行，状态检查应发现错误。
        std::vector<uint32_t> input(kRowsMax + 1, kVocab);
        std::copy(ids.begin(), ids.end(), input.begin());
        const std::vector<uint32_t> input_before = input;
        std::fill(output_.begin(), output_.end(), guard_);

        // 用非零旧值污染结果，检查函数是否清掉上一次调用的数据。
        common_result_v2 result = {99, 99, 99, 99, 99, 99, 99, 99};
        const common_result_v2 result_before = result;
        embedding_v2(
            null_ptr == IDS_NULL ? nullptr : input.data(),
            null_ptr == TABLE_NULL ? nullptr : table_.data() + kGuardWords,
            null_ptr == OUTPUT_NULL ? nullptr : output_.data() + kGuardWords,
            p.mode, p.rows, p.hidden, p.vocab, p.abi, p.job, p.flags,
            null_ptr == RESULT_NULL ? nullptr : &result);

        require(input == input_before, name + ": token IDs were modified");
        if (null_ptr == RESULT_NULL) {
            require(same_result(result, result_before), name + ": unexpected result change");
        } else {
            require(result.status_code == expected_status, name + ": wrong status_code");
            require(result.processed_rows == completed, name + ": wrong processed_rows");
            // 性能字段尚未实现，CRC 未启用；配置标识应为固定非零值。
            require(result.perf_cycles_total == 0 && result.perf_cycles_compute == 0 &&
                    result.perf_read_beats == 0 && result.perf_write_beats == 0 &&
                    result.output_crc == 0,
                    name + ": stale or unexpected optional result fields");
            require(result.build_config == kExpectedBuildConfig,
                    name + ": wrong build_config (expected 0x10010011)");
        }

        // 不直接把 packed table 行当作 golden；逐个 16-bit 值计算参考结果。
        for (uint32_t t = 0; t < completed; ++t) {
            check_row(output_.data() + kGuardWords + size_t(t) * kRowWords,
                      ids[t], name + ": output row " + std::to_string(t));
        }
        for (size_t i = 0; i < kGuardWords; ++i) {
            require(output_[i] == guard_, name + ": write before output buffer");
        }
        // 含未使用行、失败行及后续行、尾部哨兵，均必须保持原值。
        for (size_t i = kGuardWords + size_t(completed) * kRowWords; i < output_.size(); ++i) {
            require(output_[i] == guard_, name + ": write beyond completed rows");
        }
        ++passed_;
        std::cout << "[PASS] " << name << std::endl;
    }

    void check_table_unchanged() {
        for (uint32_t id : seeded_ids_) {
            check_row(table_.data() + kGuardWords + size_t(id) * kRowWords, id,
                      "immutable table row " + std::to_string(id));
        }
        for (size_t i = 0; i < kGuardWords; ++i) {
            require(table_[i] == guard_ && table_[kGuardWords + kTableWords + i] == guard_,
                    "table guard modified");
        }
        ++passed_;
        std::cout << "[PASS] initialized table rows and guards unchanged" << std::endl;
    }

    unsigned passed() const { return passed_; }

private:
    void seed_row(uint32_t id) {
        if (seeded_[id]) return;
        for (uint32_t w = 0; w < kRowWords; ++w) {
            axi_word_t word = 0;
            for (uint32_t lane = 0; lane < kLanes; ++lane) {
                word.range(lane * 16 + 15, lane * 16) = reference_bf16(id, w * kLanes + lane);
            }
            table_[kGuardWords + size_t(id) * kRowWords + w] = word;
        }
        seeded_[id] = true;
        seeded_ids_.push_back(id);
    }

    void check_row(const axi_word_t *row, uint32_t id, const std::string &context) const {
        for (uint32_t h = 0; h < kHidden; ++h) {
            const uint32_t lane = h % kLanes;
            const ap_uint<16> actual = row[h / kLanes].range(lane * 16 + 15, lane * 16);
            const uint16_t expected = reference_bf16(id, h);
            if (actual.to_uint() != expected) {
                std::ostringstream message;
                message << context << ", hidden=" << h << ": expected=0x" << std::hex
                        << expected << ", actual=0x" << actual.to_uint();
                throw std::runtime_error(message.str());
            }
        }
    }

    axi_word_t guard_;
    std::vector<axi_word_t> table_;
    std::vector<axi_word_t> output_;
    std::vector<bool> seeded_;
    std::vector<uint32_t> seeded_ids_;
    unsigned passed_;
};

} // namespace

int main() {
    try {
        TestBench tb;
        Params p;

        // 1. 两种模式、不同有效行数、词表首尾、重复 ID 和原始 BF16 位模式。
        tb.run("prefill T=1, first token", p, {0}, OK, 1);
        p.mode = 1;
        tb.run("decode T=1, last token", p, {kVocab - 1}, OK, 1);
        p = Params(); p.rows = 3;
        tb.run("boundary tokens in non-sorted order", p, {kVocab - 1, 0, 42}, OK, 3);
        for (uint32_t rows : {36u, 64u}) {
            p = Params(); p.rows = rows;
            tb.run("prefill T=" + std::to_string(rows), p, varied_ids(rows), OK, rows);
        }
        p = Params(); p.rows = 64;
        tb.run("64 repeated tokens", p, std::vector<uint32_t>(64, 42), OK, 64);
        p = Params(); p.mode = 1; p.job = UINT32_MAX;
        tb.run("decode after prefill, changed job_id", p, {42}, OK, 1);

        // 2. 参数错误必须在读取数据或写出 hidden 之前返回。
        p = Params(); p.abi = 1;
        tb.run("bad ABI", p, {42}, BAD_ABI, 0);
        for (uint32_t mode : {2u, UINT32_MAX}) {
            p = Params(); p.mode = mode;
            tb.run("bad mode " + std::to_string(mode), p, {42}, BAD_DIMENSION, 0);
        }
        for (uint32_t rows : {0u, 65u, UINT32_MAX}) {
            p = Params(); p.rows = rows;
            tb.run("bad row count " + std::to_string(rows), p, {42}, BAD_DIMENSION, 0);
        }
        for (uint32_t rows : {2u, 64u}) {
            p = Params(); p.mode = 1; p.rows = rows;
            tb.run("decode rejects T=" + std::to_string(rows), p, varied_ids(rows), BAD_DIMENSION, 0);
        }
        for (uint32_t hidden : {0u, 895u, 897u}) {
            p = Params(); p.hidden = hidden;
            tb.run("bad hidden_size " + std::to_string(hidden), p, {42}, BAD_DIMENSION, 0);
        }
        for (uint32_t vocab : {0u, 151935u, 151937u}) {
            p = Params(); p.vocab = vocab;
            tb.run("bad vocab_size " + std::to_string(vocab), p, {42}, BAD_DIMENSION, 0);
        }
        // 当前只支持 flags=0；将来实现可选功能后应同步调整这些预期。
        for (uint32_t flags : {1u, 2u, 4u, 8u, 15u, 0x80000000u}) {
            p = Params(); p.flags = flags;
            tb.run("unsupported flags " + std::to_string(flags), p, {42}, BAD_FLAGS, 0);
        }
        for (NullPointer ptr : {IDS_NULL, TABLE_NULL, OUTPUT_NULL}) {
            p = Params();
            tb.run("null data pointer " + std::to_string(static_cast<int>(ptr)),
                   p, {42}, BAD_ADDRESS_RANGE, 0, ptr);
        }

        // 3. 非法 token 出现在首行、中间或末行；核对部分完成语义。
        for (uint32_t id : {151936u, UINT32_MAX}) {
            p = Params();
            tb.run("bad first token " + std::to_string(id), p, {id}, BAD_TOKEN, 0);
        }
        p = Params(); p.rows = 3;
        tb.run("bad middle token", p, {0, kVocab, kVocab - 1}, BAD_TOKEN, 1);
        tb.run("bad last token", p, {kVocab - 1, 42, kVocab}, BAD_TOKEN, 2);
        p.rows = 64;
        std::vector<uint32_t> ids = varied_ids(64); ids.back() = kVocab;
        tb.run("bad token at row 63", p, ids, BAD_TOKEN, 63);

        // 4. C++ 接口的空结果指针防护，以及失败之后的下一次成功调用。
        // 空指针用例属于 C 仿真；后续 RTL test bench 需按真实寄存器接口适配。
        p = Params();
        tb.run("null result pointer", p, {42}, OK, 0, RESULT_NULL);
        p.mode = 1;
        tb.run("successful decode after errors", p, {kVocab - 1}, OK, 1);
        tb.check_table_unchanged();

        std::cout << "\nALL " << tb.passed() << " CHECKS PASSED" << std::endl;
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "\n[FAIL] " << error.what() << std::endl;
        return 1;
    }
}
