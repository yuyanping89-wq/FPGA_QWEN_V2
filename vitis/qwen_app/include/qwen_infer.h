#ifndef QWEN_INFER_H
#define QWEN_INFER_H

#include <stddef.h>
#include <stdint.h>
#include "qwen_board_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    QWEN_OK = 0,
    QWEN_E_ARGUMENT = -1,
    QWEN_E_RANGE = -2,
    QWEN_E_ALIGNMENT = -3,
    QWEN_E_CDMA = -4,
    QWEN_E_TIMEOUT = -5,
    QWEN_E_IP_STATUS = -6,
    QWEN_E_BAD_BUILD = -7,
    QWEN_E_ASSET_MISSING = -8,
    QWEN_E_ASSET_IO = -9,
    QWEN_E_M08_NOT_INSTALLED = -10,
    QWEN_E_M10_NOT_INSTALLED = -11,
    QWEN_E_MIG_PROBE = -12,
    QWEN_E_INTERNAL = -13,
    QWEN_E_NEEDS_RESET = -14,
    QWEN_E_PROTOCOL = -15
} QwenStatus;

typedef enum {
    QWEN_ASSET_SHARED_VOCAB = 0,
    QWEN_ASSET_M07_WQKV,
    QWEN_ASSET_M07_BQKV,
    QWEN_ASSET_M11_WO,
    QWEN_ASSET_M06_ATTN_GAMMA,
    QWEN_ASSET_M06_FFN_GAMMA,
    QWEN_ASSET_M13_W_GATE,
    QWEN_ASSET_M13_W_UP,
    QWEN_ASSET_M13_W_DOWN,
    QWEN_ASSET_M06_FINAL_GAMMA,
    QWEN_ASSET_M08_ROPE_COS,
    QWEN_ASSET_M08_ROPE_SIN
} QwenAssetId;

/* The asset provider reads source files in bounded, sequential chunks. */
typedef struct {
    void *context;
    uint64_t (*size)(void *context, QwenAssetId asset, uint32_t layer);
    QwenStatus (*read)(void *context, QwenAssetId asset, uint32_t layer,
                       uint64_t offset, void *dst, uint32_t bytes);
} QwenAssetProvider;

typedef struct {
    uint32_t loaded;
    uint32_t shared_vocab_address;
    uint32_t layer_weights_address;
} QwenModel;

typedef struct {
    const uint32_t *prompt_tokens;
    uint32_t prompt_count;
    uint32_t max_new_tokens;
    uint32_t eos_token_id;       /* UINT32_MAX disables EOS stop. */
    float repetition_penalty;    /* 1.0 disables penalty; >1 enables M14 bitmap. */
} QwenGenerateRequest;

typedef struct {
    uint32_t generated_count;
    uint32_t generated[QWEN_MAX_SEQUENCE];
    uint32_t final_cache_length;
    uint32_t failed_layer;
    uint32_t failed_module;
} QwenGenerateResult;

typedef void (*QwenServiceHook)(void *context);

void qwen_platform_set_service_hook(QwenServiceHook hook, void *context);
uint64_t qwen_platform_time_ms(void);
uint64_t qwen_platform_time_us(void);
void qwen_platform_log(const char *text);
void qwen_platform_service(void);
void qwen_platform_cache_flush(const void *address, uint32_t bytes);
void qwen_platform_cache_invalidate(void *address, uint32_t bytes);
uint32_t qwen_platform_read32(uint32_t address);
void qwen_platform_write32(uint32_t address, uint32_t value);
QwenStatus qwen_platform_cdma_copy(uint32_t source, uint32_t destination,
                                   uint32_t bytes, uint32_t timeout_ms);

QwenStatus qwen_probe_pl_ddr(void);
QwenStatus qwen_model_load(const QwenAssetProvider *assets, QwenModel *model);
QwenStatus qwen_generate(const QwenModel *model, const QwenGenerateRequest *request,
                         QwenGenerateResult *result);

const char *qwen_status_string(QwenStatus status);

#ifdef __cplusplus
}
#endif

#endif
