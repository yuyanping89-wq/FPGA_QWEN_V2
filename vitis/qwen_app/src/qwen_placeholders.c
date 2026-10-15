#include "qwen_internal.h"

/* The filename is retained for existing manually imported Vitis projects.
 * M08/M10 now have hardware drivers, with offsets checked against HLS *_hw.h.
 */
static void attention_write_ptr(uint32_t base, uint32_t offset, uint32_t address)
{
    qwen_platform_write32(base + offset, address);
    qwen_platform_write32(base + offset + 4U, 0U);
}

QwenStatus qwen_m08_rope(uint32_t q_raw, uint32_t k_raw,
                         uint32_t cos_table, uint32_t sin_table,
                         uint32_t q_rot, uint32_t k_rot,
                         uint32_t run_mode, uint32_t valid_rows,
                         uint32_t position_base, uint32_t max_position,
                         uint32_t abi_version, uint32_t job_id, uint32_t flags)
{
    uint32_t base=QWEN_M08_BASE;
    uint32_t p[6]={q_raw,k_raw,cos_table,sin_table,q_rot,k_rot};
    uint32_t n[6];
    QwenStatus status;
    if(abi_version!=QWEN_ABI_VERSION || run_mode>1U || valid_rows==0U ||
       valid_rows>QWEN_MAX_PREFILL_ROWS || (run_mode==1U && valid_rows!=1U) ||
       max_position==0U || max_position>QWEN_MAX_SEQUENCE ||
       position_base>=max_position || valid_rows>max_position-position_base ||
       (flags & ~0xBU)) return QWEN_E_ARGUMENT;
    n[0]=n[4]=valid_rows*QWEN_ROW_BYTES;
    n[1]=n[5]=valid_rows*QWEN_KV_HEADS*QWEN_KV_HEAD_ROW_BYTES;
    n[2]=n[3]=max_position*QWEN_KV_HEAD_ROW_BYTES;
    status=qwen_check_axi_buffers(p,n,6U,4U);
    if(status!=QWEN_OK) return status;
    attention_write_ptr(base,0x010U,q_raw); attention_write_ptr(base,0x01CU,k_raw);
    attention_write_ptr(base,0x028U,cos_table); attention_write_ptr(base,0x034U,sin_table);
    attention_write_ptr(base,0x040U,q_rot); attention_write_ptr(base,0x04CU,k_rot);
    qwen_platform_write32(base+0x058U,run_mode);
    qwen_platform_write32(base+0x060U,valid_rows);
    qwen_platform_write32(base+0x068U,position_base);
    qwen_platform_write32(base+0x070U,max_position);
    qwen_platform_write32(base+0x078U,abi_version);
    qwen_platform_write32(base+0x080U,job_id);
    qwen_platform_write32(base+0x088U,flags);
    return qwen_wait_hls(base,valid_rows,0x090U,0x0A0U,0x120U,
                         QWEN_M08_BUILD_CONFIG,QWEN_POLL_TIMEOUT_MS,0);
}

/* K/V pointers address the entire [24,2,2048,64] cache, never a shortened
 * per-layer slice. The hardware adds layer/head offsets using stride 2048. */
QwenStatus qwen_m10_attention(uint32_t q_rot, uint32_t k_cache, uint32_t v_cache,
                              uint32_t context_out, uint32_t run_mode,
                              uint32_t layer_id, uint32_t query_pos_base,
                              uint32_t valid_rows, uint32_t cache_len,
                              float scale, uint32_t causal,
                              uint32_t abi_version, uint32_t job_id,
                              uint32_t flags)
{
    uint32_t base=QWEN_M10_BASE;
    uint32_t p[4]={q_rot,k_cache,v_cache,context_out};
    uint32_t n[4];
    QwenStatus status;
    uint32_t scale_bits=qwen_float_bits(scale);
    if(abi_version!=QWEN_ABI_VERSION || run_mode>1U || valid_rows==0U ||
       valid_rows>QWEN_MAX_PREFILL_ROWS || (run_mode==1U && valid_rows!=1U) ||
       layer_id>=QWEN_NUM_LAYERS || cache_len==0U || cache_len>QWEN_MAX_SEQUENCE ||
       query_pos_base>=cache_len || valid_rows>cache_len-query_pos_base || causal>1U ||
       !(scale>0.0f) || (scale_bits&0x7F800000U)==0x7F800000U ||
       (flags & ~0xBU)) return QWEN_E_ARGUMENT;
    n[0]=n[3]=valid_rows*QWEN_ROW_BYTES;
    n[1]=n[2]=(uint32_t)QWEN_KV_CACHE_BYTES_PER_TENSOR;
    status=qwen_check_axi_buffers(p,n,4U,3U);
    if(status!=QWEN_OK) return status;
    attention_write_ptr(base,0x010U,q_rot); attention_write_ptr(base,0x01CU,k_cache);
    attention_write_ptr(base,0x028U,v_cache); attention_write_ptr(base,0x034U,context_out);
    qwen_platform_write32(base+0x040U,run_mode);
    qwen_platform_write32(base+0x048U,layer_id);
    qwen_platform_write32(base+0x050U,query_pos_base);
    qwen_platform_write32(base+0x058U,valid_rows);
    qwen_platform_write32(base+0x060U,cache_len);
    qwen_platform_write32(base+0x068U,scale_bits);
    qwen_platform_write32(base+0x070U,causal);
    qwen_platform_write32(base+0x078U,abi_version);
    qwen_platform_write32(base+0x080U,job_id);
    qwen_platform_write32(base+0x088U,flags);
    return qwen_wait_hls(base,valid_rows,0x090U,0x0A0U,0x120U,
                         QWEN_M10_BUILD_CONFIG,QWEN_POLL_TIMEOUT_MS,0);
}
