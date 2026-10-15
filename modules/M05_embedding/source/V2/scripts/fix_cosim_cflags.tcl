# Source in the HLS Tcl Console with embedding_v2_csim / solution1 open.
# Configuration only: no compilation, synthesis, simulation, or export.
# Set the standard for BOTH preprocessing and simulation compilation.
set m05_flags_script [string map {\\ /} [info script]]
if {[file pathtype $m05_flags_script] ne "absolute"} {
    error "Source fix_cosim_cflags.tcl using its absolute path."
}
set m05_flags_root [file dirname [file dirname $m05_flags_script]]
set m05_design_file [file join $m05_flags_root embedding_v2.cpp]
set m05_tb_file [file join $m05_flags_root test_embedding_v2_cosim.cpp]
if {![file exists $m05_design_file] || ![file exists $m05_tb_file]} {
    error "M05 source or co-simulation test bench is missing."
}
add_files $m05_design_file -cflags "-std=c++11 -O2 -Wno-unknown-pragmas" -csimflags "-std=c++11 -O2 -Wno-unknown-pragmas"
add_files -tb $m05_tb_file -cflags "-std=c++11 -O2 -Wno-unknown-pragmas" -csimflags "-std=c++11 -O2 -Wno-unknown-pragmas"
puts "M05: design and co-simulation test bench now use explicit C++11 CFLAGS and CSIMFLAGS."
puts "No simulation was run. Check the project file list, then run Clean C Simulation, C Synthesis if requested, and C/RTL Co-Simulation."
