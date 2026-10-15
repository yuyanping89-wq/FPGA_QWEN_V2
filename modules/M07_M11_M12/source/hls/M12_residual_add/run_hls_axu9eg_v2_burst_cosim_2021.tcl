# Reduced-dimension RTL CoSim of the burst-optimized top with random data,
# Decode rows=1 and Prefill rows=9, checked bit-exactly against the reference.
set defs {-DRESIDUAL_H=64 -DRESIDUAL_T_MAX=16}
open_project m12_v2b_cosim
set_top residual_add_v2_hls
add_files residual_add_kernel.cpp -cflags "-I../../include $defs"
add_files -tb residual_add_v2_burst_cosim_tb.cpp -cflags "-I../../include $defs"
open_solution f -flow_target vivado -reset
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_interface -m_axi_max_widen_bitwidth 128
csim_design
csynth_design
cosim_design -trace_level none
exit
