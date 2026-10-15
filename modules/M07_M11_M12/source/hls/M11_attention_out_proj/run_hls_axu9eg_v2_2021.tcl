# ABI v2 synthesis entry point. Legacy attention_output_projection scripts remain unchanged.
open_project m11_v2
set_top attention_out_v2_hls
add_files attention_output_kernel.cpp -cflags "-I../../include"
add_files -tb attention_output_v2_tb.cpp -cflags "-I../../include"
open_solution f -flow_target vivado -reset
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_interface -m_axi_max_widen_bitwidth 128
csim_design
csynth_design
cosim_design -trace_level all
export_design -format ip_catalog -rtl verilog -version 3.1
exit
