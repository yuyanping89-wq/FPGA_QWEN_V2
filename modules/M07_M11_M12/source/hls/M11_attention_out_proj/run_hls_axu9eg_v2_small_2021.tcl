# Reduced-dimension RTL CoSim smoke test using the production ABI and layout.
set defs {-DATTN_OUT_H=64 -DATTN_OUT_T_MAX=2 -DATTN_OUT_X_DEPTH=128 -DATTN_OUT_W_DEPTH=4096 -DATTN_OUT_Y_DEPTH=128 -DATTN_OUT_PAR=8 -DATTN_OUT_COSIM_ZERO}
open_project m11_v2_small
set_top attention_out_v2_hls
add_files attention_output_kernel.cpp -cflags "-I../../include $defs"
add_files -tb attention_output_v2_tb.cpp -cflags "-I../../include $defs"
open_solution f -flow_target vivado -reset
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_interface -m_axi_max_widen_bitwidth 128
csim_design
csynth_design
cosim_design -trace_level none
exit
