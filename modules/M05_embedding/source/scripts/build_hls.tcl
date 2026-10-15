# Run manually: vitis_hls -f <absolute script path> -tclargs csim|synth|cosim
# This entry runs ONLY when invoked by the recipient. Never launched by packaging.
set root [file dirname [file dirname [file normalize [info script]]]]
set v2 [file join $root V2]
set stage [lindex $argv 0]
if {$stage ni {csim synth cosim}} { error "Specify csim, synth or cosim." }
if {![string match "2021.1*" [version -short]]} { error "Use Vitis HLS 2021.1." }
set proj [file join $v2 hls reproduce_$stage]
file mkdir [file dirname $proj]
cd [file dirname $proj]
open_project reproduce_$stage
set_top embedding_v2
set flags "-std=c++11 -O2 -Wno-unknown-pragmas"
add_files [file join $v2 embedding_v2.cpp] -cflags $flags -csimflags $flags
if {$stage eq "cosim"} { set tb test_embedding_v2_cosim.cpp } else { set tb test_embedding_v2.cpp }
add_files -tb [file join $v2 $tb] -cflags $flags -csimflags $flags
open_solution solution1 -flow_target vivado
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source [file join $v2 scripts config_interfaces.tcl]
if {$stage eq "csim"} { csim_design -clean } else { csynth_design }
if {$stage eq "cosim"} { cosim_design -rtl verilog }
close_project
