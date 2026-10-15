#ifndef QWEN_ROPE_V2_H
#define QWEN_ROPE_V2_H
#include "attention_common_v2.h"
void rope_v2_hls(const kv_axi_t *q_raw,const kv_axi_t *k_raw,
 const kv_axi_t *cos_table,const kv_axi_t *sin_table,kv_axi_t *q_rot,kv_axi_t *k_rot,
 uint32_t run_mode,uint32_t valid_rows,uint32_t position_base,uint32_t max_position,
 uint32_t abi_version,uint32_t job_id,uint32_t flags,V2_RESULT_DECL);
void rope_v2(const kv_axi_t*,const kv_axi_t*,const kv_axi_t*,const kv_axi_t*,
 kv_axi_t*,kv_axi_t*,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,common_result_v2*);
#endif
