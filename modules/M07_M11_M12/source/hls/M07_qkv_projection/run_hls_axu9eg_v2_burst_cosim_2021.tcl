# Reduced-dimension RTL CoSim of the burst-optimized top with random data,
# Decode rows=1 and Prefill rows=9, checked bit-exactly against the reference.
set defs {-DQKV_H=64 -DQKV_A=2 -DQKV_KVH=1 -DQKV_D=32 -DQKV_T_MAX=16 -DQKV_X_DEPTH=1024 -DQKV_WQ_DEPTH=4096 -DQKV_WK_DEPTH=2048 -DQKV_WV_DEPTH=2048 -DQKV_BQ_DEPTH=64 -DQKV_BK_DEPTH=32 -DQKV_BV_DEPTH=32 -DQKV_Q_DEPTH=1024 -DQKV_K_DEPTH=512 -DQKV_V_DEPTH=512}
open_project m07_v2b_cosim
set_top qkv_projection_v2_hls
add_files qkv_kernel.cpp -cflags "-I../../include $defs"
add_files -tb qkv_v2_burst_cosim_tb.cpp -cflags "-I../../include $defs"
open_solution f -flow_target vivado -reset
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_interface -m_axi_max_widen_bitwidth 128
csim_design
csynth_design
cosim_design -trace_level none
exit
