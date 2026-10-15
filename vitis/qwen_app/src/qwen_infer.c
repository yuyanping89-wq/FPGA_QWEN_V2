#include "qwen_internal.h"
#include "qwen_debug.h"

#include <string.h>

static uint8_t g_history_bitmap[QWEN_HISTORY_BITMAP_BYTES]
    __attribute__((aligned(QWEN_CACHE_LINE_BYTES)));

static uint32_t qwen_layer_asset_addr(uint32_t layer, uint32_t offset)
{
    return QWEN_PL_LAYER_WEIGHT_BASE + layer * QWEN_LAYER_WEIGHT_STRIDE + offset;
}

uint32_t qwen_pl_layer_asset(uint32_t layer, uint32_t offset)
{
    if (layer >= QWEN_NUM_LAYERS) return 0U;
    return qwen_layer_asset_addr(layer, offset);
}

static uint32_t qwen_asset_bytes(QwenAssetId asset)
{
    switch (asset) {
    case QWEN_ASSET_SHARED_VOCAB: return (uint32_t)QWEN_SHARED_VOCAB_WEIGHT_BYTES;
    case QWEN_ASSET_M07_WQKV: return QWEN_M07_WQKV_BYTES;
    case QWEN_ASSET_M07_BQKV: return QWEN_M07_BQKV_BYTES;
    case QWEN_ASSET_M11_WO: return QWEN_M11_WO_BYTES;
    case QWEN_ASSET_M06_ATTN_GAMMA:
    case QWEN_ASSET_M06_FFN_GAMMA:
    case QWEN_ASSET_M06_FINAL_GAMMA: return QWEN_M06_GAMMA_BYTES;
    case QWEN_ASSET_M08_ROPE_COS:
    case QWEN_ASSET_M08_ROPE_SIN: return QWEN_ROPE_TABLE_BYTES;
    case QWEN_ASSET_M13_W_GATE:
    case QWEN_ASSET_M13_W_UP:
    case QWEN_ASSET_M13_W_DOWN: return QWEN_M13_WEIGHT_BYTES;
    default: return 0U;
    }
}

static QwenStatus qwen_stage_one(const QwenAssetProvider *assets, QwenAssetId asset,
                                 uint32_t layer, uint32_t destination)
{
    static const uint32_t modules[]={5,7,7,11,6,6,13,13,13,6,8,8};
    qwen_profile_context(QWEN_PHASE_LOAD,layer,(unsigned)asset<12U?modules[asset]:0U,
                         (uint32_t)asset,0U,0U,0U);
    qwen_profile_progress("load_asset",(uint32_t)asset);
    return qwen_stage_asset(assets, asset, layer, destination, qwen_asset_bytes(asset));
}

static QwenStatus qwen_check_asset_size(const QwenAssetProvider *assets,
                                       QwenAssetId asset, uint32_t layer)
{
    uint64_t actual = assets->size(assets->context, asset, layer);
    uint32_t expected = qwen_asset_bytes(asset);
    if (actual == 0U) return QWEN_E_ASSET_MISSING;
    return actual == expected ? QWEN_OK : QWEN_E_ASSET_IO;
}

static QwenStatus qwen_validate_model_assets(const QwenAssetProvider *assets)
{
    static const QwenAssetId layer_assets[] = {
        QWEN_ASSET_M07_WQKV,
        QWEN_ASSET_M07_BQKV,
        QWEN_ASSET_M11_WO,
        QWEN_ASSET_M06_ATTN_GAMMA,
        QWEN_ASSET_M06_FFN_GAMMA,
        QWEN_ASSET_M13_W_GATE,
        QWEN_ASSET_M13_W_UP,
        QWEN_ASSET_M13_W_DOWN
    };
    uint32_t layer;
    uint32_t i;
    QwenStatus status;

    status = qwen_check_asset_size(assets, QWEN_ASSET_SHARED_VOCAB, 0U);
    if (status != QWEN_OK) return status;
    status = qwen_check_asset_size(assets, QWEN_ASSET_M08_ROPE_COS, 0U);
    if (status != QWEN_OK) return status;
    status = qwen_check_asset_size(assets, QWEN_ASSET_M08_ROPE_SIN, 0U);
    if (status != QWEN_OK) return status;
    for (layer = 0U; layer < QWEN_NUM_LAYERS; ++layer) {
        for (i = 0U; i < sizeof(layer_assets) / sizeof(layer_assets[0]); ++i) {
            status = qwen_check_asset_size(assets, layer_assets[i], layer);
            if (status != QWEN_OK) return status;
        }
    }
    return qwen_check_asset_size(assets, QWEN_ASSET_M06_FINAL_GAMMA, 0U);
}

static QwenStatus model_load_impl(const QwenAssetProvider *assets, QwenModel *model)
{
    uint32_t layer;
    QwenStatus status;
    if (assets == 0 || assets->size == 0 || assets->read == 0 || model == 0) {
        return QWEN_E_ARGUMENT;
    }
    memset(model, 0, sizeof(*model));
    status = qwen_validate_model_assets(assets);
    if (status != QWEN_OK) return status;
    status = qwen_probe_pl_ddr();
    if (status != QWEN_OK) return status;

    status = qwen_stage_one(assets, QWEN_ASSET_SHARED_VOCAB, 0U,
                            QWEN_PL_SHARED_WEIGHT_ADDR);
    if (status != QWEN_OK) return status;
    status = qwen_stage_one(assets, QWEN_ASSET_M08_ROPE_COS, 0U,
                            QWEN_PL_ROPE_COS_ADDR);
    if (status != QWEN_OK) return status;
    status = qwen_stage_one(assets, QWEN_ASSET_M08_ROPE_SIN, 0U,
                            QWEN_PL_ROPE_SIN_ADDR);
    if (status != QWEN_OK) return status;

    for (layer = 0U; layer < QWEN_NUM_LAYERS; ++layer) {
        uint32_t base = qwen_layer_asset_addr(layer, 0U);
        status = qwen_stage_one(assets, QWEN_ASSET_M07_WQKV, layer,
                                base + QWEN_LAYER_OFF_M07_WQKV);
        if (status != QWEN_OK) return status;
        status = qwen_stage_one(assets, QWEN_ASSET_M07_BQKV, layer,
                                base + QWEN_LAYER_OFF_M07_BQKV);
        if (status != QWEN_OK) return status;
        status = qwen_stage_one(assets, QWEN_ASSET_M11_WO, layer,
                                base + QWEN_LAYER_OFF_M11_WO);
        if (status != QWEN_OK) return status;
        status = qwen_stage_one(assets, QWEN_ASSET_M06_ATTN_GAMMA, layer,
                                base + QWEN_LAYER_OFF_M06_ATTN_GAMMA);
        if (status != QWEN_OK) return status;
        status = qwen_stage_one(assets, QWEN_ASSET_M06_FFN_GAMMA, layer,
                                base + QWEN_LAYER_OFF_M06_FFN_GAMMA);
        if (status != QWEN_OK) return status;
        status = qwen_stage_one(assets, QWEN_ASSET_M13_W_GATE, layer,
                                base + QWEN_LAYER_OFF_M13_W_GATE);
        if (status != QWEN_OK) return status;
        status = qwen_stage_one(assets, QWEN_ASSET_M13_W_UP, layer,
                                base + QWEN_LAYER_OFF_M13_W_UP);
        if (status != QWEN_OK) return status;
        status = qwen_stage_one(assets, QWEN_ASSET_M13_W_DOWN, layer,
                                base + QWEN_LAYER_OFF_M13_W_DOWN);
        if (status != QWEN_OK) return status;
        qwen_platform_service();
    }
    status = qwen_stage_one(assets, QWEN_ASSET_M06_FINAL_GAMMA, 0U,
                            QWEN_PL_BUF_FINAL_GAMMA);
    if (status != QWEN_OK) return status;

    model->shared_vocab_address = QWEN_PL_SHARED_WEIGHT_ADDR;
    model->layer_weights_address = QWEN_PL_LAYER_WEIGHT_BASE;
    model->loaded = 1U;
    return QWEN_OK;
}

static QwenStatus qwen_run_layer(uint32_t layer, uint32_t rows, uint32_t run_mode,
                                 uint32_t start_pos, uint32_t job_id,
                                 uint32_t *hidden_current, uint32_t *failed_module)
{
    uint32_t layer_base = qwen_layer_asset_addr(layer, 0U);
    uint32_t hidden_next;
    QwenStatus status;

#define RUN_IP(call, module_id, role_id) do { \
        qwen_profile_context(run_mode?QWEN_PHASE_DECODE:QWEN_PHASE_PREFILL,layer, \
                             module_id,role_id,rows,start_pos,job_id); \
        *failed_module = (module_id); \
        status = (call); \
        if (status != QWEN_OK) return status; \
    } while (0)

    RUN_IP(qwen_ip_m06(*hidden_current, layer_base + QWEN_LAYER_OFF_M06_ATTN_GAMMA,
                       QWEN_PL_BUF_NORM, rows, run_mode, 0U, job_id), 6U, 0U);
    RUN_IP(qwen_ip_m07(QWEN_PL_BUF_NORM,
                       layer_base + QWEN_LAYER_OFF_M07_WQKV,
                       layer_base + QWEN_LAYER_OFF_M07_BQKV,
                       QWEN_PL_BUF_Q, QWEN_PL_BUF_K, QWEN_PL_BUF_V,
                       rows, run_mode, job_id), 7U, 0U);
    qwen_profile_context(run_mode?2U:1U,layer,8U,0U,rows,start_pos,job_id);
    *failed_module = 8U;
    status = qwen_m08_rope(QWEN_PL_BUF_Q, QWEN_PL_BUF_K,
                           QWEN_PL_ROPE_COS_ADDR, QWEN_PL_ROPE_SIN_ADDR,
                           QWEN_PL_BUF_Q_ROT, QWEN_PL_BUF_K_ROT,
                           run_mode, rows, start_pos, QWEN_MAX_SEQUENCE,
                           QWEN_ABI_VERSION, job_id, qwen_debug_ip_flags());
    if (status != QWEN_OK) return status;
    RUN_IP(qwen_ip_m09(QWEN_PL_BUF_K_ROT, QWEN_PL_BUF_V, layer,
                       start_pos, rows, run_mode, job_id), 9U, 0U);
    qwen_profile_context(run_mode?2U:1U,layer,10U,0U,rows,start_pos,job_id);
    *failed_module = 10U;
    status = qwen_m10_attention(QWEN_PL_BUF_Q_ROT,
                                QWEN_PL_K_CACHE_BASE, QWEN_PL_V_CACHE_BASE,
                                QWEN_PL_BUF_CONTEXT, run_mode, layer, start_pos,
                                rows, start_pos + rows, 0.125f, 1U,
                                QWEN_ABI_VERSION, job_id, qwen_debug_ip_flags());
    if (status != QWEN_OK) return status;
    RUN_IP(qwen_ip_m11(QWEN_PL_BUF_CONTEXT,
                       layer_base + QWEN_LAYER_OFF_M11_WO,
                       QWEN_PL_BUF_ATTN_OUT, rows, run_mode, job_id), 11U, 0U);
    RUN_IP(qwen_ip_m12(*hidden_current, QWEN_PL_BUF_ATTN_OUT,
                       QWEN_PL_BUF_RESIDUAL, rows, run_mode, 0U, job_id), 12U, 0U);
    RUN_IP(qwen_ip_m06(QWEN_PL_BUF_RESIDUAL,
                       layer_base + QWEN_LAYER_OFF_M06_FFN_GAMMA,
                       QWEN_PL_BUF_NORM, rows, run_mode, 1U, job_id), 6U, 1U);
    RUN_IP(qwen_ip_m13(QWEN_PL_BUF_NORM,
                       layer_base + QWEN_LAYER_OFF_M13_W_GATE,
                       layer_base + QWEN_LAYER_OFF_M13_W_UP,
                       layer_base + QWEN_LAYER_OFF_M13_W_DOWN,
                       QWEN_PL_BUF_FFN_OUT, rows, run_mode, job_id), 13U, 0U);
    hidden_next = (*hidden_current == QWEN_PL_BUF_HIDDEN_A)
                    ? QWEN_PL_BUF_HIDDEN_B : QWEN_PL_BUF_HIDDEN_A;
    RUN_IP(qwen_ip_m12(QWEN_PL_BUF_RESIDUAL, QWEN_PL_BUF_FFN_OUT,
                       hidden_next, rows, run_mode, 1U, job_id), 12U, 1U);
    *hidden_current = hidden_next;
#undef RUN_IP
    return QWEN_OK;
}

static QwenStatus qwen_run_block(const uint32_t *tokens, uint32_t rows,
                                 uint32_t start_pos, uint32_t job_id,
                                 uint32_t run_mode, uint32_t *hidden_out,
                                 uint32_t *failed_layer, uint32_t *failed_module)
{
    uint32_t layer;
    uint32_t hidden_current = QWEN_PL_BUF_HIDDEN_A;
    QwenStatus status;
    if (tokens == 0 || rows == 0U || rows > QWEN_MAX_PREFILL_ROWS ||
        start_pos + rows > QWEN_MAX_SEQUENCE) return QWEN_E_ARGUMENT;
    *failed_layer=UINT32_MAX;
    qwen_profile_context(run_mode?2U:1U,UINT32_MAX,5U,0U,rows,start_pos,job_id);
    *failed_module = 5U;
    status = qwen_upload_bytes(QWEN_PL_BUF_TOKEN_IDS, tokens,
                               rows * sizeof(uint32_t));
    if (status != QWEN_OK) return status;
    status = qwen_ip_m05(QWEN_PL_BUF_TOKEN_IDS, hidden_current, rows, run_mode, job_id);
    if (status != QWEN_OK) return status;

    for (layer = 0U; layer < QWEN_NUM_LAYERS; ++layer) {
        *failed_layer = layer;
        qwen_profile_progress("layer",layer);
        status = qwen_run_layer(layer, rows, run_mode, start_pos, job_id,
                                &hidden_current, failed_module);
        if (status != QWEN_OK) return status;
    }
    *hidden_out = hidden_current;
    return QWEN_OK;
}

static void qwen_history_add(uint32_t token)
{
    g_history_bitmap[token >> 3] |= (uint8_t)(1U << (token & 7U));
}

static QwenStatus qwen_select_token(uint32_t hidden, uint32_t job_id,
                                    uint32_t run_mode,
                                    float repetition_penalty, uint32_t *token)
{
    QwenStatus status;
    if (repetition_penalty > 1.0f) {
        status = qwen_upload_bytes(QWEN_PL_BUF_HISTORY, g_history_bitmap,
                                   QWEN_HISTORY_BITMAP_BYTES);
        if (status != QWEN_OK) return status;
    }
    return qwen_ip_m14(hidden, QWEN_PL_BUF_HISTORY, job_id, run_mode,
                       repetition_penalty, token);
}

static QwenStatus generate_impl(const QwenModel *model, const QwenGenerateRequest *request,
                         QwenGenerateResult *result)
{
    uint32_t cache_len = 0U;
    uint32_t prompt_offset = 0U;
    uint32_t job_id = 1U;
    uint32_t token = 0U;
    uint32_t new_token_index;
    QwenStatus status;

    if (result == 0) return QWEN_E_ARGUMENT;
    memset(result, 0, sizeof(*result));
    result->failed_layer = UINT32_MAX;
    if (model == 0 || model->loaded != 1U || request == 0 ||
        request->prompt_tokens == 0 || request->prompt_count == 0U ||
        request->prompt_count > QWEN_MAX_SEQUENCE || request->max_new_tokens == 0U ||
        request->max_new_tokens > QWEN_MAX_SEQUENCE - request->prompt_count ||
        !(request->repetition_penalty >= 1.0f && request->repetition_penalty <= 16.0f)) {
        return QWEN_E_ARGUMENT;
    }
    memset(g_history_bitmap, 0, sizeof(g_history_bitmap));
    for (prompt_offset = 0U; prompt_offset < request->prompt_count; ++prompt_offset) {
        uint32_t id = request->prompt_tokens[prompt_offset];
        if (id >= QWEN_VOCAB_SIZE) return QWEN_E_RANGE;
        qwen_history_add(id);
    }

    prompt_offset = 0U;
    while (prompt_offset < request->prompt_count) {
        uint32_t rows = request->prompt_count - prompt_offset;
        uint32_t hidden;
        uint32_t block_start = cache_len;
        if (rows > qwen_debug_prefill_rows()) rows = qwen_debug_prefill_rows();
        status = qwen_run_block(request->prompt_tokens + prompt_offset, rows,
                                block_start, job_id++, 0U, &hidden,
                                &result->failed_layer, &result->failed_module);
        if (status != QWEN_OK) {
            result->final_cache_length = cache_len;
            return status;
        }
        cache_len += rows;
        prompt_offset += rows;
        if (prompt_offset == request->prompt_count) {
            uint32_t last_row = hidden + (rows - 1U) * QWEN_ROW_BYTES;
            result->failed_layer=UINT32_MAX;
            qwen_profile_context(1U,UINT32_MAX,6U,2U,1U,cache_len-1U,job_id);
            result->failed_module = 6U;
            status = qwen_ip_m06(last_row, QWEN_PL_BUF_FINAL_GAMMA,
                                 QWEN_PL_BUF_FINAL_NORM, 1U, 0U, 2U, job_id++);
            if (status != QWEN_OK) {
                result->final_cache_length = cache_len;
                return status;
            }
            qwen_profile_context(1U,UINT32_MAX,14U,0U,1U,cache_len-1U,job_id);
            result->failed_module = 14U;
            status = qwen_select_token(QWEN_PL_BUF_FINAL_NORM, job_id++, 0U,
                                       request->repetition_penalty, &token);
            if (status != QWEN_OK) {
                result->final_cache_length = cache_len;
                return status;
            }
        }
    }

    qwen_profile_first_token();
    for (new_token_index = 0U; new_token_index < request->max_new_tokens; ++new_token_index) {
        uint32_t hidden;
        result->generated[result->generated_count++] = token;
        qwen_history_add(token);
        qwen_profile_token();
        qwen_profile_progress("generated",result->generated_count);
        if ((request->eos_token_id!=UINT32_MAX &&
             (token==request->eos_token_id || token==151643U)) ||
            new_token_index + 1U == request->max_new_tokens) {
            break;
        }
        status = qwen_run_block(&token, 1U, cache_len, job_id++, 1U, &hidden,
                                &result->failed_layer, &result->failed_module);
        if (status != QWEN_OK) {
            result->final_cache_length = cache_len;
            return status;
        }
        cache_len += 1U;
        result->failed_layer=UINT32_MAX;
        qwen_profile_context(2U,UINT32_MAX,6U,2U,1U,cache_len-1U,job_id);
        result->failed_module = 6U;
        status = qwen_ip_m06(hidden, QWEN_PL_BUF_FINAL_GAMMA,
                             QWEN_PL_BUF_FINAL_NORM, 1U, 1U, 2U, job_id++);
        if (status != QWEN_OK) {
            result->final_cache_length = cache_len;
            return status;
        }
        qwen_profile_context(2U,UINT32_MAX,14U,0U,1U,cache_len-1U,job_id);
        result->failed_module = 14U;
        status = qwen_select_token(QWEN_PL_BUF_FINAL_NORM, job_id++, 1U,
                                   request->repetition_penalty, &token);
        if (status != QWEN_OK) {
            result->final_cache_length = cache_len;
            return status;
        }
    }
    result->final_cache_length = cache_len;
    result->failed_layer = UINT32_MAX;
    result->failed_module = 0U;
    return QWEN_OK;
}

QwenStatus qwen_model_load(const QwenAssetProvider *assets,QwenModel *model)
{
    QwenDebugOptions opt={QWEN_DEBUG_UART,64U,0U};
    QwenStatus st;
    qwen_profile_begin(&opt,0U);
    qwen_debug_set_stage(QWEN_BOOT_MODEL,0);
    st=qwen_debug_faulted()?QWEN_E_NEEDS_RESET:model_load_impl(assets,model);
    qwen_profile_finish(st,0U);qwen_profile_save_boot();
    if(st!=QWEN_OK) qwen_debug_set_stage(QWEN_BOOT_ERROR,st);
    return st;
}
QwenStatus qwen_generate_debug(const QwenModel *model,const QwenGenerateRequest *request,
                               const QwenDebugOptions *opt,QwenGenerateResult *result)
{
    QwenStatus st;
    qwen_profile_begin(opt,request?request->prompt_count:0U);
    qwen_debug_set_stage(QWEN_BOOT_INFER,0);
    if(result) {memset(result,0,sizeof(*result));result->failed_layer=UINT32_MAX;}
    if(opt && ((opt->flags&~7U) || opt->prefill_rows>64U || opt->trace_limit>QWEN_PROFILE_MAX_TRACE))
        st=QWEN_E_ARGUMENT;
    else st=qwen_debug_faulted()?QWEN_E_NEEDS_RESET:generate_impl(model,request,result);
    qwen_profile_finish(st,result?result->generated_count:0U);
    qwen_debug_set_stage(st==QWEN_OK?QWEN_BOOT_READY:QWEN_BOOT_ERROR,st);
    return st;
}
QwenStatus qwen_generate(const QwenModel *model,const QwenGenerateRequest *request,
                         QwenGenerateResult *result)
{ return qwen_generate_debug(model,request,0,result); }
