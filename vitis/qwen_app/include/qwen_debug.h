#ifndef QWEN_DEBUG_H
#define QWEN_DEBUG_H
#include <stdint.h>
/* Inspect this object in Vitis Expressions / Memory when UART is absent. */
enum {QWEN_BOOT_RESET=0,QWEN_BOOT_NETWORK=1,QWEN_BOOT_SD=2,QWEN_BOOT_MODEL=3,
      QWEN_BOOT_READY=4,QWEN_BOOT_INFER=5,QWEN_BOOT_ERROR=6};
typedef struct {
    uint32_t magic,stage;
    int32_t status;
    uint32_t fatfs_result,ipv4[4],phase,layer,module,role,rows,position,job,log_sequence;
    char last_log[192];
    char asset_path[192];
} QwenDebugState;
extern volatile QwenDebugState qwen_debug_state;
void qwen_debug_set_stage(uint32_t stage,int32_t status);
#endif
