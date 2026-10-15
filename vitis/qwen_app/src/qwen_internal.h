#ifndef QWEN_INTERNAL_H
#define QWEN_INTERNAL_H

#include "qwen_infer.h"
#include "qwen_profile.h"

#define QWEN_HLS_AP_CTRL       0x000U
#define QWEN_HLS_AP_START      0x00000001U
#define QWEN_HLS_AP_DONE       0x00000002U
#define QWEN_PTR_HI(lo)        ((lo) + 4U)

QwenStatus qwen_check_pl_range(uint32_t address, uint32_t bytes);
QwenStatus qwen_check_axi_buffers(const uint32_t *addresses, const uint32_t *bytes,
                                  uint32_t count, uint32_t first_output);
QwenStatus qwen_stage_asset(const QwenAssetProvider *assets, QwenAssetId asset,
                            uint32_t layer, uint32_t destination, uint32_t expected_bytes);
QwenStatus qwen_upload_bytes(uint32_t destination, const void *source, uint32_t bytes);
QwenStatus qwen_wait_hls(uint32_t base, uint32_t expected_rows,
                         uint32_t status_offset, uint32_t processed_rows_offset,
                         uint32_t build_config_offset, uint32_t expected_build,
                         uint32_t timeout_ms, uint32_t *device_status);

QwenStatus qwen_ip_m05(uint32_t token_ids, uint32_t hidden_out, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id);
QwenStatus qwen_ip_m06(uint32_t x, uint32_t gamma, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t role, uint32_t job_id);
QwenStatus qwen_ip_m07(uint32_t x, uint32_t wqkv, uint32_t bqkv,
                       uint32_t q, uint32_t k, uint32_t v, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id);
QwenStatus qwen_ip_m09(uint32_t k_new, uint32_t v_new, uint32_t layer,
                       uint32_t start_pos, uint32_t rows, uint32_t run_mode,
                       uint32_t job_id);
QwenStatus qwen_ip_m11(uint32_t x, uint32_t wo, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id);
QwenStatus qwen_ip_m12(uint32_t a, uint32_t b, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t role, uint32_t job_id);
QwenStatus qwen_ip_m13(uint32_t x, uint32_t w_gate, uint32_t w_up,
                       uint32_t w_down, uint32_t y, uint32_t rows,
                       uint32_t run_mode, uint32_t job_id);
QwenStatus qwen_ip_m14(uint32_t hidden, uint32_t history, uint32_t job_id,
                       uint32_t run_mode, float repetition_penalty,
                       uint32_t *token);

/* M08/M10 hardware drivers; retain the established inference call signatures. */
QwenStatus qwen_m08_rope(uint32_t q_raw, uint32_t k_raw,
                         uint32_t cos_table, uint32_t sin_table,
                         uint32_t q_rot, uint32_t k_rot,
                         uint32_t run_mode, uint32_t valid_rows,
                         uint32_t position_base, uint32_t max_position,
                         uint32_t abi_version, uint32_t job_id, uint32_t flags);
QwenStatus qwen_m10_attention(uint32_t q_rot, uint32_t k_cache, uint32_t v_cache,
                              uint32_t context_out, uint32_t run_mode,
                              uint32_t layer_id, uint32_t query_pos_base,
                              uint32_t valid_rows, uint32_t cache_len,
                              float scale, uint32_t causal,
                              uint32_t abi_version, uint32_t job_id,
                              uint32_t flags);

uint32_t qwen_pl_layer_asset(uint32_t layer, uint32_t offset);
uint32_t qwen_float_bits(float value);

#endif
