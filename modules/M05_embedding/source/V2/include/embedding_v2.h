#ifndef M05_EMBEDDING_V2_H
#define M05_EMBEDDING_V2_H

#include <stdint.h>
#include <ap_int.h> // AMD HLS 提供的任意位宽整数类型。
#include "common_result_v2.h"
#include "m05_build_config.h"

namespace m05_v2 {

// 当前 V2 模型和接口合同；这些是编译时常量。
constexpr uint32_t kAbiVersion = 2;
constexpr uint32_t kHiddenSize = 896;
constexpr uint32_t kVocabSize = 151936;
constexpr uint32_t kMaxRows = 64;
constexpr uint32_t kPrefill = 0;
constexpr uint32_t kDecode = 1;

// 本阶段使用的公共状态码；数值与项目 V2 文档一致。
enum StatusCode : uint32_t {
    kStatusOk = 0,
    kStatusBadAbi = 1,
    kStatusBadDimension = 2,
    kStatusBadAddressRange = 4,
    kStatusBadToken = 7,
    kStatusBadFlags = 11
};

constexpr uint32_t kBf16Bits = 16;
constexpr uint32_t kAxiBits = 128;
constexpr uint32_t kBytesPerWord = kAxiBits / 8;                // 16 字节。
constexpr uint32_t kBf16PerWord = kAxiBits / kBf16Bits;         // 8 个 BF16。
constexpr uint32_t kWordsPerRow = kHiddenSize / kBf16PerWord;   // 112 个数据字。
constexpr uint32_t kBytesPerRow = kWordsPerRow * kBytesPerWord; // 1792 字节。

// 仅搬运 BF16 原始位模式，不把这个 128-bit 整数用于浮点计算。
using axi_word_t = ap_uint<kAxiBits>;

static_assert(kAxiBits % kBf16Bits == 0, "A word must contain whole BF16 values");
static_assert(kHiddenSize % kBf16PerWord == 0, "A row must contain whole words");
static_assert(kAxiBits == 128 && kBf16PerWord == 8 && kBuildLaneCode == 1,
              "Update the build profile when changing the data width");

} // namespace m05_v2

// 三级 DATAFLOW 基线已通过 38 项 C 回归和 12 项 RTL 回归。
// 当前增加 build_config 标识的版本待用户重新综合和仿真。
// Vivado IP 流程需要加载 scripts/config_interfaces.tcl，启用 64-bit 地址。
// flags 目前仅支持 0。C 仿真中的空指针保护不属于硬件地址范围检查。
// 遇到非法 token 不再发送后续任务；流水线排空此前合法行后返回错误。
// 此前完整复制的行数记入 processed_rows。
// 任意非 0 status_code 都表示本次输出不可交给下游使用。
// 调用方负责缓冲区容量、地址对齐、合法范围和不重叠检查。
void embedding_v2(
    const uint32_t *token_ids,
    const m05_v2::axi_word_t *table,
    m05_v2::axi_word_t *hidden_out,
    uint32_t run_mode,       // 0=PREFILL，1=DECODE。
    uint32_t valid_rows,     // PREFILL: 1..64；DECODE: 1。
    uint32_t hidden_size,    // 必须等于 896。
    uint32_t vocab_size,     // 必须等于 151936。
    uint32_t abi_version,    // 必须等于 2。
    uint32_t job_id,         // 调用方提供的任务编号。
    uint32_t flags,          // 公共功能标志，按 V2 合同处理。
    common_result_v2 *result);

#endif
