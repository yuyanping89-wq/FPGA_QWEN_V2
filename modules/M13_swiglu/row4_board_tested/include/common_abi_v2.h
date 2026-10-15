#ifndef COMMON_ABI_V2_H
#define COMMON_ABI_V2_H

#include <stdint.h>

// Shared ABI V2 values.  Module headers may provide module-prefixed aliases,
// but the numeric values and semantics originate here.
enum CommonRunModeV2 {
    COMMON_RUN_MODE_PREFILL = 0,
    COMMON_RUN_MODE_DECODE = 1
};

enum CommonStatusV2 {
    COMMON_STATUS_OK = 0,
    COMMON_STATUS_BAD_ABI = 1,
    COMMON_STATUS_BAD_DIMENSION = 2,
    COMMON_STATUS_BAD_ALIGNMENT = 3,
    COMMON_STATUS_BAD_ADDRESS_RANGE = 4,
    COMMON_STATUS_BAD_LAYOUT = 5,
    COMMON_STATUS_BAD_LAYER = 6,
    COMMON_STATUS_BAD_TOKEN = 7,
    COMMON_STATUS_BAD_ALIAS = 8,
    COMMON_STATUS_NUMERIC_ERROR = 9,
    COMMON_STATUS_DRIVER_TIMEOUT = 10,
    COMMON_STATUS_BAD_FLAGS = 11,
    COMMON_STATUS_INTERNAL_ERROR = 12
};

enum CommonFlagsV2 {
    COMMON_FLAG_ENABLE_OUTPUT_CRC = 1u << 0,
    COMMON_FLAG_DEBUG_SNAPSHOT = 1u << 1
};

enum CommonBuildConfigV2 {
    COMMON_PAR_CODE_8 = 1,
    COMMON_PAR_CODE_16 = 2,
    COMMON_PAR_CODE_32 = 3,
    COMMON_CLOCK_CODE_100_MHZ = 1
};

static const uint32_t COMMON_ABI_VERSION_V2 = 2u;

#endif  // COMMON_ABI_V2_H
