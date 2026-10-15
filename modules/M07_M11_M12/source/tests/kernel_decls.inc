void qkv_projection_v2(const uint16_t*, const uint16_t*, const uint16_t*,
                       uint16_t*, uint16_t*, uint16_t*, uint32_t, uint32_t,
                       uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
void attention_out_v2(const uint16_t*, const uint16_t*, uint16_t*, uint32_t, uint32_t,
                      uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
void residual_add_v2(const uint16_t*, const uint16_t*, uint16_t*, uint32_t, uint32_t,
                     uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, common_result_v2*);
