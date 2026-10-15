# Run in Vivado 2021.1 after File > Close Project.
# Creates an independent OOC project; NEVER launches synthesis/implementation.
# Copies current instrumented RTL and constraints to freeze this experiment.
set m05_ooc_root [file normalize [file join [file dirname [info script]] ..]]
if {[llength [get_projects -quiet]] != 0} {
    error "Save/close the current project (File > Close Project), then run this script."
}
if {![string match "2021.1*" [version -short]]} {
    error "This project recipe targets Vivado 2021.1. Review before using another version."
}
set m05_ooc_input [file join $m05_ooc_root build embedding_v2_perf]
set m05_ooc_output [file join $m05_ooc_root build embedding_v2_perf_ooc]
if {[file exists $m05_ooc_output]} {
    error "OOC directory already exists. Reopen its vivado_project/m05_embedding_perf_ooc.xpr; do not recreate it."
}
foreach m05_required {manifest.json rtl/embedding_v2.v rtl/m05_perf_counters.v} {
    if {![file isfile [file join $m05_ooc_input $m05_required]]} {
        error "Missing instrumented input: $m05_required"
    }
}
set m05_handle [open [file join $m05_ooc_input rtl embedding_v2.v] r]
set m05_top_text [read $m05_handle]
close $m05_handle
if {[string first "m05_perf_counters_U" $m05_top_text] < 0 ||
    [string first "32'h20010011" $m05_top_text] < 0} {
    error "Expected instrumented M05 build 0x20010011."
}
foreach m05_name {m05_ooc_timing.xdc m05_ooc_context.xdc} {
    if {![file isfile [file join $m05_ooc_root constraints $m05_name]]} {
        error "Missing constraint source: $m05_name"
    }
}
if {![file isfile [file join $m05_ooc_root scripts m05_ooc_pre_opt.tcl]]} {
    error "Missing implementation clock-context hook."
}

file mkdir $m05_ooc_output
file copy [file join $m05_ooc_input rtl] [file join $m05_ooc_output rtl]
file copy [file join $m05_ooc_input manifest.json] [file join $m05_ooc_output source_manifest.json]
file mkdir [file join $m05_ooc_output constraints]
foreach m05_name {m05_ooc_timing.xdc m05_ooc_context.xdc} {
    file copy [file join $m05_ooc_root constraints $m05_name] [file join $m05_ooc_output constraints $m05_name]
}
set m05_handle [open [file join $m05_ooc_output ANALYSIS_SCOPE.txt] w]
puts $m05_handle "M05 instrumented RTL build 0x20010011; Vivado 2021.1; xczu9eg-ffvb1156-2-i."
puts $m05_handle "10 ns clock; provisional synchronous I/O budget: min 0 ns, max 2 ns."
puts $m05_handle "External clock buffer site is an analysis assumption, printed by the pre-opt Tcl hook."
puts $m05_handle "No boundary partition-pin placement: interface routing/signoff remains pending."
puts $m05_handle "No Pblock: standalone use of chip resources does not reserve space in the system."
puts $m05_handle "No synthesis, implementation, simulation or bitstream run by project-creation script."
puts $m05_handle "source_manifest.json is source provenance, NOT this run's validation status."
close $m05_handle

create_project m05_embedding_perf_ooc [file join $m05_ooc_output vivado_project] -part xczu9eg-ffvb1156-2-i
# Set the essential mode before adding optional project configuration.
set_property -name {STEPS.SYNTH_DESIGN.ARGS.MORE OPTIONS} -value {-mode out_of_context} -objects [get_runs synth_1]
set_property target_language Verilog [current_project]
add_files -norecurse [glob [file join $m05_ooc_output rtl *.v]]
foreach m05_mem [glob -nocomplain [file join $m05_ooc_output rtl *.dat] [file join $m05_ooc_output rtl *.mem]] {
    add_files -norecurse $m05_mem
}
set_property top embedding_v2 [get_filesets sources_1]
set m05_timing [file join $m05_ooc_output constraints m05_ooc_timing.xdc]
set m05_context [file join $m05_ooc_output constraints m05_ooc_context.xdc]
add_files -fileset constrs_1 -norecurse [list $m05_timing $m05_context]
set_property USED_IN_SYNTHESIS false [get_files $m05_context]
set_property USED_IN_IMPLEMENTATION true [get_files $m05_context]
set_property PROCESSING_ORDER LATE [get_files $m05_context]
file mkdir [file join $m05_ooc_output scripts]
set m05_hook [file join $m05_ooc_output scripts m05_ooc_pre_opt.tcl]
file copy [file join $m05_ooc_root scripts m05_ooc_pre_opt.tcl] $m05_hook
set_property STEPS.OPT_DESIGN.TCL.PRE $m05_hook [get_runs impl_1]
update_compile_order -fileset sources_1
puts "M05 OOC: project created; source and constraint snapshots copied."
puts "M05 OOC: no synthesis, implementation or simulation has run."
puts "M05 OOC: now use Run Synthesis, then Run Implementation (no bitstream)."
puts "M05 OOC: constraints are provisional; results are NOT full-system timing signoff."
