#ifndef QWEN_ATTENTION_V2_H
#define QWEN_ATTENTION_V2_H
#include "attention_common_v2.h"
void attention_v2_hls(const kv_axi_t *q_rot,const kv_axi_t *k_cache,const kv_axi_t *v_cache,kv_axi_t *attn_out,
 uint32_t run_mode,uint32_t layer_id,uint32_t query_pos_base,uint32_t valid_rows,uint32_t cache_len,
 float scale,uint32_t causal,uint32_t abi_version,uint32_t job_id,uint32_t flags,V2_RESULT_DECL);
void attention_v2(const kv_axi_t*,const kv_axi_t*,const kv_axi_t*,kv_axi_t*,
 uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,float,uint32_t,uint32_t,uint32_t,uint32_t,common_result_v2*);
#endif
