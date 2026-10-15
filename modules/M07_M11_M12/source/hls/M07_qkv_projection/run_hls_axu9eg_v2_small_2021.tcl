# Reduced-dimension RTL CoSim smoke test.  The full-size project uses the
# same source and interface contract; this build keeps XSIM runtime short.
set defs {-DQKV_H=64 -DQKV_A=2 -DQKV_KVH=1 -DQKV_D=32 -DQKV_T_MAX=2 -DQKV_X_DEPTH=128 -DQKV_WQ_DEPTH=2048 -DQKV_WK_DEPTH=2048 -DQKV_WV_DEPTH=2048 -DQKV_BQ_DEPTH=64 -DQKV_BK_DEPTH=32 -DQKV_BV_DEPTH=32 -DQKV_Q_DEPTH=128 -DQKV_K_DEPTH=64 -DQKV_V_DEPTH=64}
open_project m07_v2_small
set_top qkv_projection_v2_hls
add_files qkv_kernel.cpp -cflags "-I../../include $defs"
add_files -tb qkv_v2_tb.cpp -cflags "-I../../include $defs"
open_solution f -flow_target vivado -reset
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_interface -m_axi_max_widen_bitwidth 128
csim_design
csynth_design
cosim_design -trace_level none
exit
