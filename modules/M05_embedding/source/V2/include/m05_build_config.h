#ifndef M05_BUILD_CONFIG_H
#define M05_BUILD_CONFIG_H

#include <stdint.h>

namespace m05_v2 {

// Module-local profile using the suggested encoding in the V2 field document.
// Final project-wide lane/clock/numeric mappings must be agreed with M02.
// This identifies compiled hardware; it is not a per-job input.
constexpr uint32_t kBuildLaneCode = 1;       // Eight BF16 values per 128-bit word.
constexpr uint32_t kBuildRowTile = 1;        // One row per read/write stage.
constexpr uint32_t kBuildMainTile = 64;      // Maximum table/output burst beats.
constexpr uint32_t kBuildNumericImpl = 0;    // Baseline, bit-exact BF16 copy.
constexpr uint32_t kBuildClockCode = 0;      // Target 100 MHz / 10 ns.
constexpr uint32_t kBuildModuleRevision = 1; // First explicitly identified build.

// Reject values that do not fit rather than silently masking/truncating them.
static_assert(kBuildLaneCode < 16, "PAR_LANES_CODE exceeds four bits");
static_assert(kBuildRowTile < 64, "ROW_TILE exceeds six bits");
static_assert(kBuildMainTile < 256, "MAIN_TILE exceeds eight bits");
static_assert(kBuildNumericImpl < 16, "NUMERIC_IMPL_ID exceeds four bits");
static_assert(kBuildClockCode < 64, "CLOCK_CODE exceeds six bits");
static_assert(kBuildModuleRevision < 16, "MODULE_REVISION exceeds four bits");

constexpr uint32_t kBuildConfig =
    kBuildLaneCode |
    (kBuildRowTile << 4) |
    (kBuildMainTile << 10) |
    (kBuildNumericImpl << 18) |
    (kBuildClockCode << 22) |
    (kBuildModuleRevision << 28);

} // namespace m05_v2

#endif
