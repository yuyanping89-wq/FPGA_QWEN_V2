#include "qwen_fatfs_assets.h"
#include "qwen_debug.h"

#include "ff.h"
#if defined(FF_USE_LFN) && FF_USE_LFN == 0
#error "Enable xilffs long filenames (use_lfn=1 or 2) for qwen model assets"
#endif
#include <stdio.h>
#include <string.h>

static QwenStatus make_path(QwenFatfsAssetContext *context, QwenAssetId asset,
                            uint32_t layer, char *path, size_t capacity)
{
    int count;
    char file[80];
    if (context == 0 || path == 0 || capacity == 0U) return QWEN_E_ARGUMENT;
    switch (asset) {
    case QWEN_ASSET_SHARED_VOCAB:
        (void)snprintf(file, sizeof(file), "shared_vocab_weight.bf16.bin");
        break;
    case QWEN_ASSET_M07_WQKV:
        (void)snprintf(file, sizeof(file), "layer%02lu_m07_wqkv.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M07_BQKV:
        (void)snprintf(file, sizeof(file), "layer%02lu_m07_bqkv.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M11_WO:
        (void)snprintf(file, sizeof(file), "layer%02lu_m11_wo.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M06_ATTN_GAMMA:
        (void)snprintf(file, sizeof(file), "layer%02lu_m06_attn_gamma.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M06_FFN_GAMMA:
        (void)snprintf(file, sizeof(file), "layer%02lu_m06_ffn_gamma.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M13_W_GATE:
        (void)snprintf(file, sizeof(file), "layer%02lu_m13_w_gate.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M13_W_UP:
        (void)snprintf(file, sizeof(file), "layer%02lu_m13_w_up.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M13_W_DOWN:
        (void)snprintf(file, sizeof(file), "layer%02lu_m13_w_down.bin", (unsigned long)layer);
        break;
    case QWEN_ASSET_M06_FINAL_GAMMA:
        (void)snprintf(file, sizeof(file), "final_m06_gamma.bin");
        break;
    case QWEN_ASSET_M08_ROPE_COS:
        (void)snprintf(file, sizeof(file), "rope_cos.bf16.bin");
        break;
    case QWEN_ASSET_M08_ROPE_SIN:
        (void)snprintf(file, sizeof(file), "rope_sin.bf16.bin");
        break;
    default:
        return QWEN_E_ARGUMENT;
    }
    count = snprintf(path, capacity, "%s/%s", context->root, file);
    return count > 0 && (size_t)count < capacity ? QWEN_OK : QWEN_E_ARGUMENT;
}

static void report_file_error(const char *path,FRESULT result)
{
    char message[192];
    uint32_t i=0U;
    qwen_debug_state.fatfs_result=(uint32_t)result;
    while(path[i] && i<sizeof(qwen_debug_state.asset_path)-1U) {
        qwen_debug_state.asset_path[i]=path[i];++i;
    }
    qwen_debug_state.asset_path[i]='\0';
    (void)snprintf(message,sizeof(message),"SD error=%u file=%.145s\r\n",(unsigned)result,path);
    qwen_platform_log(message);
}

static uint64_t fatfs_asset_size(void *opaque, QwenAssetId asset, uint32_t layer)
{
    QwenFatfsAssetContext *context = (QwenFatfsAssetContext *)opaque;
    char path[192];
    FIL file;
    uint64_t bytes = 0U;
    FRESULT result;
    if (make_path(context, asset, layer, path, sizeof(path)) != QWEN_OK) return 0U;
    result=f_open(&file,path,FA_READ);
    if (result != FR_OK) {report_file_error(path,result);return 0U;}
    bytes = (uint64_t)f_size(&file);
    (void)f_close(&file);
    return bytes;
}

static QwenStatus fatfs_asset_read(void *opaque, QwenAssetId asset, uint32_t layer,
                                   uint64_t offset, void *dst, uint32_t bytes)
{
    QwenFatfsAssetContext *context = (QwenFatfsAssetContext *)opaque;
    char path[192];
    FIL file;
    UINT read_bytes = 0U;
    FRESULT result;
    if (dst == 0 || bytes == 0U || offset > 0xFFFFFFFFULL) return QWEN_E_ARGUMENT;
    if (make_path(context, asset, layer, path, sizeof(path)) != QWEN_OK) return QWEN_E_ARGUMENT;
    result = f_open(&file, path, FA_READ);
    if (result != FR_OK) {report_file_error(path,result);return QWEN_E_ASSET_MISSING;}
    result = f_lseek(&file, (FSIZE_t)offset);
    if (result == FR_OK) result = f_read(&file, dst, (UINT)bytes, &read_bytes);
    (void)f_close(&file);
    if (result != FR_OK || read_bytes != bytes) {
        report_file_error(path,result);return QWEN_E_ASSET_IO;
    }
    return QWEN_OK;
}

QwenStatus qwen_fatfs_assets_init(QwenFatfsAssetContext *context,
                                  const char *root,
                                  QwenAssetProvider *provider)
{
    size_t length;
    if (context == 0 || root == 0 || provider == 0) return QWEN_E_ARGUMENT;
    length = strlen(root);
    if (length == 0U || length >= sizeof(context->root)) return QWEN_E_ARGUMENT;
    memcpy(context->root, root, length + 1U);
    provider->context = context;
    provider->size = fatfs_asset_size;
    provider->read = fatfs_asset_read;
    return QWEN_OK;
}
