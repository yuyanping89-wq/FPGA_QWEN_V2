set workspace [file normalize [file join [file dirname [info script]] ..]]
setws $workspace
platform active qwen
domain active standalone_psu_cortexa53_0
bsp config mem_size 524288
bsp config memp_n_tcp_seg 512
bsp config pbuf_pool_size 512
bsp config tcp_snd_buf 32768
bsp config tcp_wnd 32768
bsp regenerate
puts QWEN_NETWORK_BSP_REGENERATED
