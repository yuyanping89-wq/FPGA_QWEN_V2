# Run in Vivado 2021.1, not in Vitis HLS. Creates a reviewable RTL project only.
# No synthesis, implementation, simulation or IP export is launched by this file.
# Optional: set m05_perf_dir to a prepared output directory before sourcing.
set m05_v2_root [file normalize [file join [file dirname [info script]] ..]]
if {![info exists m05_perf_dir]} {
    set m05_perf_dir [file join $m05_v2_root build embedding_v2_perf]
}
set m05_perf_dir [file normalize $m05_perf_dir]
if {![file isfile [file join $m05_perf_dir manifest.json]]} {
    error "Run prepare_perf_rtl.py first; missing manifest.json in $m05_perf_dir"
}
set m05_project_dir [file join $m05_perf_dir vivado_project]
if {[file exists $m05_project_dir]} {
    error "Project directory already exists. Open its .xpr instead of sourcing this script again."
}
create_project m05_embedding_perf $m05_project_dir -part xczu9eg-ffvb1156-2-i
set_property target_language Verilog [current_project]
add_files -norecurse [glob [file join $m05_perf_dir rtl *.v]]
foreach m05_mem [glob -nocomplain [file join $m05_perf_dir rtl *.dat] [file join $m05_perf_dir rtl *.mem]] {
    add_files -norecurse $m05_mem
}
set_property top embedding_v2 [get_filesets sources_1]
add_files -fileset constrs_1 -norecurse [file join $m05_perf_dir clock.xdc]
update_compile_order -fileset sources_1
puts "M05: instrumented RTL project created. No synthesis or simulation has run."
puts "M05: existing HLS C/RTL co-simulation does not include these counters."
