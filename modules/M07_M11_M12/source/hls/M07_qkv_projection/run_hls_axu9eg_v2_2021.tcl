# ABI v2 synthesis entry point. Legacy qkv_proj scripts remain unchanged.
open_project m07_v2
set_top qkv_projection_v2_hls
add_files qkv_kernel.cpp -cflags "-I../../include"
add_files -tb qkv_v2_tb.cpp -cflags "-I../../include"
open_solution f -flow_target vivado -reset
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_interface -m_axi_max_widen_bitwidth 128
csim_design
csynth_design
cosim_design -trace_level all
export_design -format ip_catalog -rtl verilog -version 3.1
exit
